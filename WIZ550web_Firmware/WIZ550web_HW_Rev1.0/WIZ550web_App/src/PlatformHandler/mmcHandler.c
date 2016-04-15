#include "mmcHandler.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_spi.h"
#include "mmc_sd.h"   // mmc_sd memory card interface
#include "boardutil.h"
#include "dataflash.h"

#ifdef STM32_SD_USE_DMA
// #warning "Information only: using DMA"
/*-----------------------------------------------------------------------*/
/* Transmit/Receive Block using DMA (Platform dependent. STM32 here)     */
/*-----------------------------------------------------------------------*/
static void stm32_dma_transfer(uint8_t receive, const BYTE *buff, uint16_t btr);
#endif

FATFS Fatfs[1];
card_type_id_t card_type = NO_CARD;

enum speed_setting { INTERFACE_SLOW, INTERFACE_FAST };

//#include <stdio.h> // for debugging
FRESULT getMountedMemorySize(uint8_t mount_ret, uint32_t * totalSize, uint32_t * availableSize)
{
	FATFS *fs;
	DWORD fre_clust, fre_sect, tot_sect;
	FRESULT res;

	/* Get volume information and free clusters of drive 1 */
	if(mount_ret == SPI_FLASHM)
		res = f_getfree("0:", &fre_clust, &fs);
	else if((mount_ret >= CARD_MMC) && (mount_ret <= CARD_SDHC))
		res = f_getfree("1:", &fre_clust, &fs);

	if (!res)
	{
		/* Get total sectors and free sectors */
		tot_sect = (fs->n_fatent - 2) * fs->csize;
		fre_sect = fre_clust * fs->csize;

		/* Print the free space (assuming 512 bytes/sector) */
		//printf(" - Available memory size : %8ld / %8ld kB\r\n", fre_sect / 2, tot_sect / 2);

		*totalSize = tot_sect / 2;
		*availableSize = fre_sect / 2;
	}

	return res;
}

uint64_t getAvailableMemorySize(void)
{
	uint64_t size = 1024;

	return size;
}

static void SD_SPI_Interface_speed( enum speed_setting speed )
{
	DWORD tmp;

	tmp = SPI_SD->CR1;
	if ( speed == INTERFACE_SLOW ) {
		/* Set slow clock (100k-400k) */
		tmp = ( tmp | SPI_BaudRatePrescaler_256 );
	} else {
		/* Set fast clock (depends on the CSD) */
		tmp = ( tmp & ~SPI_BaudRatePrescaler_256 ) | SPI_BaudRatePrescaler_SPI_SD;
	}
	SPI_SD->CR1 = tmp;
}

/*
 * MMC/SDC Card functions
 */

// compute checksum for MMC commands
uint8_t mmc_crc7(uint8_t *data, uint8_t len)
{
	uint8_t c, n, crc = 0;

	while(len--)
	{
		c = *(data++);
		for(n = 8; n; --n)
		{
			crc <<= 1;
			if( (c ^ crc) & 0x80 )
				crc ^= 0x09;
			c <<= 1;
		}
	}
	return (crc << 1) | 1;
}

// rxtx byte to MMC card via SPI
uint8_t mmc_send(uint8_t data)
{

	/* Loop while DR register in not empty */
	while (SPI_I2S_GetFlagStatus(SPI_SD, SPI_I2S_FLAG_TXE) == RESET) { ; }

	/* Send byte through the SPI peripheral */
	SPI_I2S_SendData(SPI_SD, data);

	/* Wait to receive a byte */
	while (SPI_I2S_GetFlagStatus(SPI_SD, SPI_I2S_FLAG_RXNE) == RESET) { ; }

	/* Return the byte read from the SPI bus */
	return SPI_I2S_ReceiveData(SPI_SD);

}

// chip release MMC card
void mmc_release()
{
	SD_CS_DESELECT();
	mmc_send(0xff);
}

// chip select MMC card
uint8_t mmc_select()
{
	uint16_t timer;

	SD_CS_SELECT();

	// wait until previous operation completed
	for(timer = 50000; timer; --timer)
	{
		if(mmc_send(0xff) == 0xff)
			return 1;
		Delay_ms(1);
	}
	
	mmc_release();
	return 0;
}
#include<stdio.h>
// send command frame to MMC card
uint8_t mmc_command(uint8_t code, uint32_t arg)
{
	uint8_t frame[5], crc, n, res;
	//SD_ACT_ON();
	res=mmc_send(0xff);

	// send APP_CMD (for ACMD<n>)
	if(code & 0x40)
	{
		if((res = mmc_command(APP_CMD, 0)) > 1)
			return res; // err
	}

	// send command frame
	frame[0] = code | 0x40;
	frame[1] = arg >> 24;
	frame[2] = arg >> 16;
	frame[3] = arg >> 8;
	frame[4] = arg;
	crc = mmc_crc7(frame, sizeof(frame));
	
	for(n = 0; n < sizeof(frame); ++n)
		mmc_send(frame[n]);
	mmc_send(crc);

	// wait until card respond
	for(n = 10; n; --n)
	{
		if((res = mmc_send(0xff)) != 0xff)
			break;
	}
	
	//SD_ACT_OFF();
	return res;
}


// initialize MMC card & SPI
uint8_t mmc_init()
{
	card_type_id_t card = NO_CARD;
	uint8_t n, res, ocr[4];
	uint16_t timer;

#if 1
	GPIO_InitTypeDef GPIO_InitStructure;

	/*!< Configure SD_CS_PIN pin: SD_CS pin */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_CS;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;

	GPIO_Init(GPIO_CS, &GPIO_InitStructure);

	/* De-select the Card: Chip Select high */
	SD_CS_DESELECT();

	SD_SPI_Interface_speed(INTERFACE_SLOW);	// For SD Card Initialization

#ifdef STM32_SD_USE_DMA
	/* enable DMA clock */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
#endif

#else
	    SPI_InitTypeDef  SPI_InitStructure;
		GPIO_InitTypeDef GPIO_InitStructure;

		SPI_Cmd(SPI_SD, DISABLE);

		/* Enable GPIO clock for CS */
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIO_CS, ENABLE);
		/* Enable SPI clock, SPI1: APB2, SPI2: APB1 */
		RCC_APBPeriphClockCmd_SPI_SD(RCC_APBPeriph_SPI_SD, ENABLE);


	//	Delay_ms(250);

		/* Configure I/O for Flash Chip select */
		GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_CS;
		GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIO_CS, &GPIO_InitStructure);

		/* De-select the Card: Chip Select high */
		SD_CS_DESELECT();

		/* Configure SPI pins: SCK and MOSI with default alternate function (not re-mapped) push-pull */
		GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_SPI_SD_SCK | GPIO_Pin_SPI_SD_MOSI;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
		GPIO_Init(GPIO_SPI_SD, &GPIO_InitStructure);
		/* Configure MISO as Input with internal pull-up */
		GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_SPI_SD_MISO;
		GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
		GPIO_Init(GPIO_SPI_SD, &GPIO_InitStructure);

		/* SPI configuration */
		SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
		SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
		SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
		SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
		SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
		SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
		//SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64; //for initialization  < 400kHz
		SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256; //for initialization  < 400kHz
		SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
		SPI_InitStructure.SPI_CRCPolynomial = 7;

		SPI_Init(SPI_SD, &SPI_InitStructure);
		SPI_CalculateCRC(SPI_SD, DISABLE);
		SPI_Cmd(SPI_SD, ENABLE);

		Delay_ms(1);
          /*
		// Turn on GPIO for power-control pin connected to FET's gate
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIO_PWR, ENABLE);
			// Configure I/O for Power FET
			GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_PWR;
			GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_PWR;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_Init(GPIO_PWR, &GPIO_InitStructure);

			//here must be checked if card is inserted

			if (inserdet then power up P-channel mosfet w low level)
			{
				GPIO_ResetBits(GPIO_PWR, GPIO_Pin_PWR);
			}
			else
			{
				// Chip select internal pull-down (to avoid parasite powering)
				GPIO_InitStructure.GPIO_Pin = GPIO_Pin_CS;
				GPIO_Init(GPIO_CS, &GPIO_InitStructure);

				GPIO_SetBits(GPIO_PWR, GPIO_Pin_PWR);
			}
              */
#endif

	// send 80 clocks; dummy
	for(n = 10; n; --n)
		mmc_send(0xff);

	if(mmc_select())
	{
		//tmp=mmc_command(GO_IDLE_STATE, 0);
		//tmp=mmc_command(SEND_IF_COND, 0x1aa);


		// enter SPI mode
		if(mmc_command(GO_IDLE_STATE, 0) <= 1)
		{

            Delay_ms(250);

			// Set SPI rate Faster
            SD_SPI_Interface_speed(INTERFACE_FAST);

			/* SPI configuration */
            /*
			SPI_Cmd(SPI_SD, DISABLE);
		    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_SPI_SD;

		    SPI_Init(SPI_SD, &SPI_InitStructure);
			SPI_CalculateCRC(SPI_SD, DISABLE);
			SPI_Cmd(SPI_SD, ENABLE);
			*/

   		    // try SEND_IF_COND (for SDCv2 only)
			if((res=mmc_command(SEND_IF_COND, 0x1aa)) <= 1)
			{

			   	for(n = 0; n < 4; ++n)
					ocr[n] = mmc_send(0xff);

				// check voltage range (1 = 2,7..3,6 v)
				if((ocr[2] == 0x01) && (ocr[3] == 0xaa))
				{

					// wait until card initialized
					for(timer = 50000; timer; --timer)
					{
						// set HCS bit (support SDHC)
						if((res = mmc_command(SD_SEND_OP_COND, 1ul<<30)) == 0)
							break;

					}

					if(!res)
					{
						// read ocr
						if(!mmc_command(READ_OCR, 0))
						{
							for(n = 0; n < 4; ++n)
								ocr[n] = mmc_send(0xff);

							// check ccs bit (card capacity status)
							if(ocr[0] & 0x40)
								card = CARD_SDHC;
							else
								card = CARD_SD2;
						}
					}
				}
			}
			else
			{
				// try SD_SEND_OP_COND (for SDC only)
				if((res = mmc_command(SD_SEND_OP_COND, 0)) <= 1)
				{
					// wait until card initialized
					for(timer = 50000; timer; --timer)
					{
							if((res = mmc_command(SD_SEND_OP_COND, 0)) == 0)
							break;

					}

					if(!res)
						card = CARD_SD;
				}

				// card is MMC
				else
				{
					// wait until MMC initialized
					for(timer = 50000; timer; --timer)
					{

						if((res = mmc_command(SEND_OP_COND, 0)) == 0)
							break;

					}

					if(!res)
						card = CARD_MMC;
				}
			}
		}

		// set r/w block length to 512 bytes (for SDHC is always 512)
		if( (card == CARD_MMC) || (card == CARD_SD) || (card == CARD_SD2) )
			mmc_command(SET_BLOCKLEN, 512);

		mmc_release();
	}

	card_type = card;
	
	//if(card) SD_LED_ON();
	return card;
}

// initiate sector read
uint8_t mmc_read_init(uint32_t sector)
{
	uint8_t token;
	uint16_t timer;

	if(card_type != NO_CARD)
	{
		if(mmc_select())
		{
			// SDHC have block addressing
			if(card_type != CARD_SDHC)
				sector <<= 9;

			if(!mmc_command(READ_BLOCK, sector))
			{
				// wait data token
				for(timer = 50000; timer; --timer)
				{
					if((token = mmc_send(0xff)) != 0xff)
						break;
					//Delay_ms(1);
				}

				if(token == 0xfe)
					return 1;
			}
		
			mmc_release();
		}
	}
	
	return 0;
}

// finish sector read
void mmc_read_final()
{
	// skip crc
	mmc_send(0xff);
	mmc_send(0xff);
	
	mmc_release();
}

// initiate sector read
uint8_t mmc_write_init(uint32_t sector)
{
	if(card_type != NO_CARD)
	{
		if(mmc_select())
		{
			// SDHC addressed by 512-bytes blocks
			if(card_type != CARD_SDHC)
				sector <<= 9;

			if(mmc_command(WRITE_BLOCK, sector))
			{
				// byte space
				mmc_send(0xff);

				// data token
				mmc_send(0xfe);

				return 1;
			}
			
			mmc_release();
		}
	}

	return 0;
}

// finish sector read
uint8_t mmc_write_final()
{
	uint8_t res;

	// dummy crc
	mmc_send(0xff);
	mmc_send(0xff);

	//get data response
	res = mmc_send(0xff);

	mmc_release();

	return ((res & 0x1f) == 0x05);
}

#if 0
/*
 * FatFs Disk I/O functions
 */

// initialize card
DSTATUS disk_initialize(BYTE drive)
{
	if(!drive)
		return STA_NODISK;
	
	if( (card_type != NO_CARD) || (mmc_init() != NO_CARD) )
		return 0;
	return STA_NOINIT;
}

// check card initialized
DSTATUS disk_status(BYTE drive)
{
	if(!drive)
		return STA_NODISK;
	
	if( card_type != NO_CARD )
		return 0;
	return STA_NODISK;
}

// read sectors from card
DRESULT disk_read(BYTE drive, BYTE *buf, DWORD sector, BYTE count)
{
#ifndef STM32_SD_USE_DMA
	uint16_t i;
#endif

	if( (drive != 0) || (card_type == NO_CARD) )
		return RES_NOTRDY;
	
	while(count--)
	{
		if(!mmc_read_init(sector++))
			return RES_ERROR;
#ifdef STM32_SD_USE_DMA
		stm32_dma_transfer(1, buf, 512);  //FALSE(0) for buff->SPI, TRUE(1) for SPI->buff
#else
		for(i = 512; i; --i)
			*(buf++) = mmc_send(0xff);
#endif
		mmc_read_final();
	}
	return RES_OK;
}

// write sectors on card
DRESULT disk_write(BYTE drive, const BYTE *buf, DWORD sector, BYTE count)
{
#ifndef STM32_SD_USE_DMA
	uint16_t i;
#endif
	
	if( (drive != 0) || (card_type == NO_CARD) )
		return RES_NOTRDY;
	
	while(count--)
	{
		if(!mmc_write_init(sector++))
			return RES_ERROR;
#ifdef STM32_SD_USE_DMA
		stm32_dma_transfer(0, buf, 512);  //FALSE(0) for buff->SPI, TRUE(1) for SPI->buff
#else
		for(i = 512; i; --i)
			mmc_send(*(buf++));
#endif
		if(!mmc_write_final())
			return RES_ERROR;
	}
	return RES_OK;
}

// ctrl card
DRESULT disk_ioctl(BYTE drive, BYTE command, void *buf)
{
	if( (drive != 0) || (card_type == NO_CARD) )
		return RES_NOTRDY;
	
	switch(command)
	{
	case CTRL_SYNC:
		if(!mmc_select())
			return RES_ERROR;
		mmc_release();
		return RES_OK;
	}

	return RES_PARERR;
}

DWORD get_fattime()
{
	return 0;
}
#endif

/*
 * misc
 */
 
uint8_t mmc_mount()
{
#if 0
	if(mmc_init())
	{
		f_mount(0, &ff);
		return card_type;
	}
#else
	FRESULT res;
    u8 state;

	bsp_sd_gpio_init();

    //disk_initialize(0);
	/*
    if( disk_initialize(0) == 0 )
        printf("\r\nSD initialize success.\r\n");
    else
    	printf("\r\nSD initialize failed.\r\n");
	*/

#if !defined(F_SPI_FLASH_ONLY)
	state = SD_Init();
#if defined(_FS_DEBUG_)
    printf("SD_Init:%d\r\n", state);
#endif
	if(state == STA_NODISK)
	{
		return NO_CARD;
	}
	else if(state != 0)
	{
		return NO_CARD;
	}
	else
	{
		res = f_mount(&Fatfs[0],"1:",0);
#if defined(_FS_DEBUG_)
	    printf("f_mount:%d\r\n", res);
#endif
		g_sdcard_done = 1;

		return SD_Type;
	}
#endif
#endif

	return NO_CARD;//0
}

uint8_t flash_mount()
{
#if 0
	if(mmc_init())
	{
		f_mount(0, &ff);
		return card_type;
	}
#else
	FRESULT res;

	DataFlash_Init();

	//disk_initialize(1);

	res = f_mount(&Fatfs[0],"0:",0);
#if defined(_FS_DEBUG_)
    printf("f_mount:%d\r\n", res);
#endif

#if defined(F_SPI_FLASH)
    if(check_spiflash_flag() == 1)
        g_mkfs_done = 1;
    else
        g_mkfs_done = 0;

	res = f_mkfs("0:",0,512);

#if defined(_FS_DEBUG_)
    printf("f_mkfs:%d %d\r\n", res, g_mkfs_done);
#endif
    if(check_spiflash_flag() == 1)
    	save_spiflash_flag();
    g_mkfs_done = 1;
#endif

	return SPI_FLASHM;
#endif
}

//*******************************************************************************
void Delay_ms(uint32_t ms)
{
	volatile uint32_t nCount;
	RCC_ClocksTypeDef RCC_Clocks;
	RCC_GetClocksFreq (&RCC_Clocks);

	nCount=(RCC_Clocks.HCLK_Frequency/10000)*ms;
	for (; nCount!=0; nCount--);
}
//*****************************************************************************

#ifdef STM32_SD_USE_DMA
/*-----------------------------------------------------------------------*/
/* Transmit/Receive Block using DMA (Platform dependent. STM32 here)     */
/*-----------------------------------------------------------------------*/
static void stm32_dma_transfer(
	uint8_t receive,		/* FALSE(0) for buff->SPI, TRUE(1) for SPI->buff               */
	const BYTE *buff,		/* receive TRUE  : 512 byte data block to be transmitted
						   	receive FALSE : Data buffer to store received data    */
	uint16_t btr 			/* receive TRUE  : Byte count (must be multiple of 2)
						   	receive FALSE : Byte count (must be 512)              */
)
{
	DMA_InitTypeDef DMA_InitStructure;
	WORD rw_workbyte[] = { 0xffff };

	/* shared DMA configuration values */
	DMA_InitStructure.DMA_PeripheralBaseAddr = (DWORD)(&(SPI_SD->DR));
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_BufferSize = btr;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

	DMA_DeInit(DMA_Channel_SPI_SD_RX);
	DMA_DeInit(DMA_Channel_SPI_SD_TX);

	if ( receive ) {

		/* DMA1 channel2 configuration SPI1 RX ---------------------------------------------*/
		/* DMA1 channel4 configuration SPI2 RX ---------------------------------------------*/
		DMA_InitStructure.DMA_MemoryBaseAddr = (DWORD)buff;
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
		DMA_Init(DMA_Channel_SPI_SD_RX, &DMA_InitStructure);

		/* DMA1 channel3 configuration SPI1 TX ---------------------------------------------*/
		/* DMA1 channel5 configuration SPI2 TX ---------------------------------------------*/
		DMA_InitStructure.DMA_MemoryBaseAddr = (DWORD)rw_workbyte;
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
		DMA_Init(DMA_Channel_SPI_SD_TX, &DMA_InitStructure);

	} else {

#if _FS_READONLY == 0
		/* DMA1 channel2 configuration SPI1 RX ---------------------------------------------*/
		/* DMA1 channel4 configuration SPI2 RX ---------------------------------------------*/
		DMA_InitStructure.DMA_MemoryBaseAddr = (DWORD)rw_workbyte;
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
		DMA_Init(DMA_Channel_SPI_SD_RX, &DMA_InitStructure);

		/* DMA1 channel3 configuration SPI1 TX ---------------------------------------------*/
		/* DMA1 channel5 configuration SPI2 TX ---------------------------------------------*/
		DMA_InitStructure.DMA_MemoryBaseAddr = (DWORD)buff;
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
		DMA_Init(DMA_Channel_SPI_SD_TX, &DMA_InitStructure);
#endif

	}

	/* Enable DMA RX Channel */
	DMA_Cmd(DMA_Channel_SPI_SD_RX, ENABLE);
	/* Enable DMA TX Channel */
	DMA_Cmd(DMA_Channel_SPI_SD_TX, ENABLE);

	/* Enable SPI TX/RX request */
	SPI_I2S_DMACmd(SPI_SD, SPI_I2S_DMAReq_Rx | SPI_I2S_DMAReq_Tx, ENABLE);

	/* Wait until DMA1_Channel 3 Transfer Complete */
	/// not needed: while (DMA_GetFlagStatus(DMA_FLAG_SPI_SD_TC_TX) == RESET) { ; }
	/* Wait until DMA1_Channel 2 Receive Complete */
	while (DMA_GetFlagStatus(DMA_FLAG_SPI_SD_TC_RX) == RESET) { ; }
	// same w/o function-call:
	// while ( ( ( DMA1->ISR ) & DMA_FLAG_SPI_SD_TC_RX ) == RESET ) { ; }

	/* Disable DMA RX Channel */
	DMA_Cmd(DMA_Channel_SPI_SD_RX, DISABLE);
	/* Disable DMA TX Channel */
	DMA_Cmd(DMA_Channel_SPI_SD_TX, DISABLE);

	/* Disable SPI RX/TX request */
	SPI_I2S_DMACmd(SPI_SD, SPI_I2S_DMAReq_Rx | SPI_I2S_DMAReq_Tx, DISABLE);
}
#endif /* STM32_SD_USE_DMA */
