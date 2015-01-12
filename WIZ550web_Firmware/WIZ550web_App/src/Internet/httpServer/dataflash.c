#include <stdio.h>
#include <string.h>

#include "stm32f10x_conf.h"
#include "stm32F10x.h"

//#include "wizplatform.h"
#include "dataflash.h"
#include "boardutil.h"

void DF_BufferToPage (unsigned char BufferNo, unsigned int PageAdr);
unsigned char df_read_status (void);

static void DF_SPI_INIT(void);
static uint8_t  DF_SPI_RW(uint8_t byte);

uint16_t m_WriteAddress = 0;
uint16_t m_WritePage = 0;



void DataFlash_Init(void)
{
	m_WriteAddress = 0;
	m_WritePage = 0;

	DF_Init();
}

void DataFlash_ReInit(void)
{
	m_WriteAddress = 0;
	m_WritePage = 0;
}

void write_to_flash(unsigned char flash_data)
{
	DF_BufferWriteByte(1, m_WriteAddress, flash_data);

    m_WriteAddress++;

	// if buffer full write buffer into memory page
    if (m_WriteAddress>(PAGE_PER_BYTE-1))
    {
        m_WriteAddress = 0;

        if (m_WritePage < 2048)            // if memory is not full
        {
			DF_BufferToPage(1, m_WritePage);
            m_WritePage++;
        }
        else
        {
        	;
        }
    }
}

void write_end_page(void)
{
	DF_BufferToPage(1, m_WritePage);
}

void write_to_flashbuf(unsigned long Address, unsigned char* Buffer, unsigned int Size)
{
#if !defined(F_SPI_FLASH)
    unsigned int page = 0;
    unsigned int address = 0;

	unsigned int page_no = 0;
	unsigned int index = 0;
	unsigned int write_size = 0;
	unsigned int i;

	page_no = (unsigned int)Size/PAGE_PER_BYTE;

	if(Address == 0)
	{
		page = 0;
		address = 0;
	}
	else
	{
		page = (unsigned int)(Address/(PAGE_PER_BYTE));
		address	= (unsigned int)(Address%(PAGE_PER_BYTE));
	}

	write_size = PAGE_PER_BYTE-address;
	if(write_size >= Size)
	{
		write_size = Size;
	}

	DF_BufferWriteStr(1, address, write_size, &Buffer[0]);
	DF_BufferToPage(1, page);

// Phase 2
	index = write_size;
	page++;

	for(i = 0; i < page_no ; i++)
	{
		DF_BufferWriteStr(1, 0, PAGE_PER_BYTE, &Buffer[index]);
		DF_BufferToPage(1, page++);
		index += PAGE_PER_BYTE;
		write_size += PAGE_PER_BYTE;
	}

// Phase 3
	if(Size <= write_size) return;

	write_size = Size - write_size;
	if(write_size > 0)
	{
		DF_BufferWriteStr(1, 0, write_size, &Buffer[index]);
		DF_BufferToPage(1, page);
	}
#else
    unsigned int page = 0;
    unsigned int address = 0;

	unsigned int page_no = 0;
	unsigned int index = 0;
	unsigned int write_size = 0;
	unsigned int i;

	page_no = (unsigned int)Size/256;

	if(Address == 0)
	{
		page = 0;
		address = 0;
	}
	else
	{
		page = (unsigned int)(Address/(256));
		address	= (unsigned int)(Address%(256));
	}

	write_size = 256-address;
	if(write_size >= Size)
	{
		write_size = Size;
	}

	DF_BufferWriteStr(1, address, write_size, &Buffer[0]);
	DF_BufferToPage(1, page);

// Phase 2
	index = write_size;
	page++;

	for(i = 0; i < page_no ; i++)
	{
		DF_BufferWriteStr(1, 0, 256, &Buffer[index]);
		DF_BufferToPage(1, page++);
		index += 256;
		write_size += 256;
	}

// Phase 3
	if(Size <= write_size) return;

	write_size = Size - write_size;
	if(write_size > 0)
	{
		DF_BufferWriteStr(1, 0, write_size, &Buffer[index]);
		DF_BufferToPage(1, page);
	}
#endif
}

#if defined(F_SPI_FLASH)
void Flash_WaitReady(void)
{
   	DF_CS_LOW();
   	DF_SPI_RW(0xff);
   	while(!(df_read_status() & 0x80));
   	DF_CS_HIGH();
}
#endif

void read_from_flashbuf(unsigned long Address, unsigned char* Buffer, unsigned int Size)
{
#if !defined(F_SPI_FLASH)
    unsigned int page = 0;
    unsigned int address = 0;

	unsigned int page_no = 0;
	unsigned int index = 0;
	unsigned int read_size = 0;
	unsigned int i;

	page_no = (unsigned int)Size/PAGE_PER_BYTE;

	page = (unsigned int)(Address/(PAGE_PER_BYTE));
	address	= (unsigned int)(Address%(PAGE_PER_BYTE));

	if(Address == 0){page = 0;address=0;}

	DF_PageToBuffer(1, page);

	read_size = PAGE_PER_BYTE-address;
	if(read_size >= Size)
	{
		read_size = Size;
	}
	else read_size = PAGE_PER_BYTE-address;

	DF_BufferReadStr(1, address, read_size, &Buffer[0]);

	index = read_size;
	page++;

	for(i=0;i<page_no;i++)
	{
		DF_PageToBuffer(1, page++);
		DF_BufferReadStr(1, 0, PAGE_PER_BYTE, &Buffer[index]);
		index += PAGE_PER_BYTE;
		read_size += PAGE_PER_BYTE;
	}

	if(Size<=read_size)return;
	read_size = Size - read_size;

	if(read_size>0)
	{
		DF_PageToBuffer(1, page);
		DF_BufferReadStr(1, 0, read_size, &Buffer[index]);
	}
#else
    unsigned int page = 0;
    unsigned int address = 0;

	unsigned int page_no = 0;
	unsigned int index = 0;
	unsigned int read_size = 0;
	unsigned int i;

	page_no = (unsigned int)Size/256;

	page = (unsigned int)(Address/(256));
	address	= (unsigned int)(Address%(256));

	if(Address == 0){page = 0;address=0;}

	DF_PageToBuffer(1, page);

	read_size = 256-address;
	if(read_size >= Size)
	{
		read_size = Size;
	}
	else read_size = 256-address;

	DF_BufferReadStr(1, address, read_size, &Buffer[0]);

	index = read_size;
	page++;

	for(i=0;i<page_no;i++)
	{
		DF_PageToBuffer(1, page++);
		DF_BufferReadStr(1, 0, 256, &Buffer[index]);
		index += 256;
		read_size += 256;
	}

	if(Size<=read_size)return;
	read_size = Size - read_size;

	if(read_size>0)
	{
		DF_PageToBuffer(1, page);
		DF_BufferReadStr(1, 0, read_size, &Buffer[index]);
	}
#endif
}

//unsigned char search_file_rom(unsigned char *FileName, unsigned long *Address, unsigned long *Size)
/*
uint8_t search_file_rom(uint8_t * FileName, uint32_t * Address, uint32_t * Size)
{
	uint8_t fileidx = 1;

	ROM_DIR file;
	uint32_t addr = 0;

	while(1)
	{
		read_from_flashbuf((unsigned long)addr, (unsigned char*)&file.b, sizeof(file));

        if(file.b[0]==0xff && file.b[1]==0xff)
		{
			break;
		}

		//search file
		if(!strcmp((char *)FileName, file.f.name))
		{
			*Size = file.f.len;
			*Address = file.f.start;

			return fileidx;
		}
		addr += FILE_HEAD_SIZE;
		fileidx++;
	}

	// Second search phase for web upload
	uint8_t buf[512] = {0, };
	uint16_t i;
	uint32_t header_len = 0;

	addr = 0;
	read_from_flashbuf((unsigned long)addr, buf, sizeof(buf));
	for(i = 0; i < sizeof(buf)-3; i++)
	{
		if(buf[i] == '\r')
		{
			if(buf[i+1] == '\n' && buf[i+2] == '\r' && buf[i+3] == '\n')
			{
				header_len = addr = i+3+1;
				break;
			}
		}
	}

	while(addr)
	{
		read_from_flashbuf((unsigned long)addr, (unsigned char*)&file.b, sizeof(file));

		if(file.b[0]==0xff && file.b[1]==0xff)
		{
			break;
		}

		//search file
		if(!strcmp((char *)FileName, file.f.name))
		{
			*Size = file.f.len;
			*Address = file.f.start + header_len;

			return fileidx;
		}
		addr += FILE_HEAD_SIZE;
		fileidx++;
	}

	fileidx = 0;

	return fileidx;
}
*/
/*
uint8_t search_file_rom(uint8_t * FileName, uint32_t * Address, uint32_t * Size)
{
	uint8_t fileidx = 1;

	ROM_DIR file;
	uint32_t addr = 0;

	while(1)
	{
		read_from_flashbuf((unsigned long)addr, (unsigned char*)&file.b, sizeof(file));

        if(file.b[0]==0xff && file.b[1]==0xff)
		{
			break;
		}

		//search file
		if(!strcmp((char *)FileName, file.f.name))
		{
			*Size = file.f.len;
			*Address = file.f.start;

			return fileidx;
		}
		addr += FILE_HEAD_SIZE;
		fileidx++;
	}

	fileidx = 0;

	return fileidx;
}
*/



///////////////////////////////////////////////////////////////////////////////
// Dataflash access functions
///////////////////////////////////////////////////////////////////////////////


/**
  * @brief  Initializes the peripherals used by the W5500 driver.
  * @param  None
  * @retval None
  */

static void DF_SPI_INIT(void)
{

	GPIO_InitTypeDef GPIO_InitStructure;

	/*!< Configure DF_CS_PIN pin: DF_CS pin */
	GPIO_InitStructure.GPIO_Pin = DF_CS_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;

	GPIO_Init(DF_CS_PORT, &GPIO_InitStructure);
}

static uint8_t  DF_SPI_RW(uint8_t byte)
{
	/*!< Loop while DR register in not empty */
	while (SPI_I2S_GetFlagStatus(DF_SPI, SPI_I2S_FLAG_TXE) == RESET);

	/*!< Send byte through the SPI2 peripheral */
	SPI_I2S_SendData(DF_SPI, byte);

	/*!< Wait to receive a byte */
	while (SPI_I2S_GetFlagStatus(DF_SPI, SPI_I2S_FLAG_RXNE) == RESET);

	/*!< Return the byte read from the SPI bus */
	return (uint8_t)SPI_I2S_ReceiveData(DF_SPI);
}


/*
uint8_t DF_SPI_RD(void)
{
	uint8_t rb;
	Chip_SSP_ReadFrames_Blocking(LPC_SSP0, &rb, 1);
	return rb;
}
*/


void DF_Init(void)
{
	DF_SPI_INIT();
	DF_CS_HIGH();
}

void DF_Chip_Erase(void)
{
	DF_CS_LOW();
	__disable_irq();	// Global interrupt disable (ARM)

	DF_SPI_RW(0xc7);	// cmd
	DF_SPI_RW(0x94);	// cmd
	DF_SPI_RW(0x80);	// cmd
	DF_SPI_RW(0x9a);	// cmd

	while(!(df_read_status() & 0x80));							//monitor the status register, wait until busy-flag is high

	__enable_irq();		// Global interrupt enable (ARM)
	DF_CS_HIGH();
}

unsigned char DF_BufferReadByte(unsigned char BufferNo, unsigned int IntPageAdr)
{
	unsigned char data = 0;

	DF_CS_LOW();
	__disable_irq();	// Global interrupt disable (ARM)

	if(1 == BufferNo)							//read byte from buffer 1
	{
		DF_SPI_RW(Buf1Read);					//buffer 1 read op-code
		DF_SPI_RW(0x00); 						//don't cares
		DF_SPI_RW((unsigned char)(IntPageAdr>>8));//upper part of internal buffer address
		DF_SPI_RW((unsigned char)(IntPageAdr));	//lower part of internal buffer address
		DF_SPI_RW(0x00); 						//don't cares
		data = DF_SPI_RW(0x00);					//read byte
	}

#ifdef USE_BUFFER2
	else
	if (2 == BufferNo)							//read byte from buffer 2
	{
		DF_SPI_RW(Buf2Read);					//buffer 2 read op-code
		DF_SPI_RW(0x00);						//don't cares
		DF_SPI_RW((unsigned char)(IntPageAdr>>8));//upper part of internal buffer address
		DF_SPI_RW((unsigned char)(IntPageAdr));	//lower part of internal buffer address
		DF_SPI_RW(0x00);						//don't cares
		data = DF_SPI_RW(0x00);					//read byte
	}
#endif

	__enable_irq();		// Global interrupt enable (ARM)
	DF_CS_HIGH();
	return data;								//return the read data byte
}


void DF_BufferWriteByte(unsigned char BufferNo, unsigned int IntPageAdr, unsigned char Data)
{
	DF_CS_LOW();
	__disable_irq();	// Global interrupt disable (ARM)

	if (1 == BufferNo)							//write byte to buffer 1
	{
		DF_SPI_RW(Buf1Write);					//buffer 1 write op-code
		DF_SPI_RW(0x00); 						//don't cares
		DF_SPI_RW((unsigned char)(IntPageAdr>>8));//upper part of internal buffer address
		DF_SPI_RW((unsigned char)(IntPageAdr));	//lower part of internal buffer address
		DF_SPI_RW(Data);						//write data byte
	}

#ifdef USE_BUFFER2
	else
	if (2 == BufferNo)							//write byte to buffer 2
	{
		DF_SPI_RW(Buf2Write);					//buffer 2 write op-code
		DF_SPI_RW(0x00);						//don't cares
		DF_SPI_RW((unsigned char)(IntPageAdr>>8));//upper part of internal buffer address
		DF_SPI_RW((unsigned char)(IntPageAdr));	//lower part of internal buffer address
		DF_SPI_RW(Data);						//write data byte
	}
#endif

	__enable_irq();		// Global interrupt enable (ARM)
	DF_CS_HIGH();
}


// Globals
unsigned char PageBits;
unsigned int  PageSize;
// Functions

/*****************************************************************************
*
*	Function name : df_read_status
*
*	Returns :		One status byte. Consult Dataflash datasheet for further
*					decoding info
*
*	Parameters :	None
*
*	Purpose :		Status info concerning the Dataflash is busy or not.
*					Status info concerning compare between buffer and flash page
*					Status info concerning size of actual device
*
******************************************************************************/
unsigned char df_read_status (void)
{
	unsigned char result,index_copy;
	//Look-up table for these sizes ->  512k, 1M,  2M,  4M,  8M, 16M, 32M, 64M
	unsigned int DF_pagesize[] ={264, 264, 264, 264, 264, 528, 528, 1056};
	//Look-up table for these sizes ->  512k, 1M, 2M, 4M, 8M, 16M, 32M, 64M
	unsigned char DF_pagebits[] ={  9,  9,  9,  9,  9,  10,  10,  11};


							//make sure to toggle CS signal in order
	DF_CS_LOW();							//to reset dataflash command decoder
	//__disable_irq();	// Global interrupt disable (ARM)

	result = DF_SPI_RW(StatusReg);			//send status register read op-code
	result = DF_SPI_RW(0x00);				//dummy write to get result


	//DF_SPI_RW(StatusReg);
	//result = DF_SPI_RD();

	index_copy = ((result & 0x38) >> 3);	//get the size info from status register

	PageBits   = DF_pagebits[index_copy];	//get number of internal page address bits from look-up table
	PageSize   = DF_pagesize[index_copy];   //get the size of the page (in bytes)

	//__enable_irq();	// Global interrupt enable (ARM)
	DF_CS_HIGH();
	return result;							//return the read status register value
}


/*****************************************************************************
*
*	Function name : DF_PageToBuffer
*
*	Returns :		None
*
*	Parameters :	BufferNo	->	Decides usage of either buffer 1 or 2
*					PageAdr		->	Address of page to be transferred to buffer
*
*	Purpose :		Transfers a page from flash to dataflash SRAM buffer
*
******************************************************************************/
void DF_PageToBuffer(unsigned char BufferNo, unsigned int PageAdr)
{
	DF_CS_LOW();												//to reset dataflash command decoder
	__disable_irq();	// Global interrupt disable (ARM)

	if (1 == BufferNo)											//transfer flash page to buffer 1
	{
		DF_SPI_RW(FlashToBuf1Transfer);							//transfer to buffer 1 op-code
		DF_SPI_RW((unsigned char)(PageAdr >> (16 - PageBits)));	//upper part of page address
		DF_SPI_RW((unsigned char)(PageAdr << (PageBits - 8)));	//lower part of page address
		DF_SPI_RW(0x00);  										//don't cares
	}
#ifdef USE_BUFFER2
	else
	if (2 == BufferNo)											//transfer flash page to buffer 2
	{
		DF_SPI_RW(FlashToBuf2Transfer);							//transfer to buffer 2 op-code
		DF_SPI_RW((unsigned char)(PageAdr >> (16 - PageBits)));	//upper part of page address
		DF_SPI_RW((unsigned char)(PageAdr << (PageBits - 8)));	//lower part of page address
		DF_SPI_RW(0x00);										//don't cares
	}
#endif

	DF_CS_HIGH();												//initiate the transfer
	DF_CS_LOW();

	while(!(df_read_status() & 0x80));							//monitor the status register, wait until busy-flag is high

	__enable_irq();	// Global interrupt enable (ARM)
	DF_CS_HIGH();
}

void DF_BufferToPage (unsigned char BufferNo, unsigned int PageAdr)
{
	DF_CS_LOW();												//to reset dataflash command decoder
	__disable_irq();	// Global interrupt disable (ARM)

	if (1 == BufferNo)											//program flash page from buffer 1
	{
		DF_SPI_RW(Buf1ToFlashWE);								//buffer 1 to flash with erase op-code
		DF_SPI_RW((unsigned char)(PageAdr >> (16 - PageBits)));	//upper part of page address
		DF_SPI_RW((unsigned char)(PageAdr << (PageBits - 8)));	//lower part of page address
		DF_SPI_RW(0x00); 						//don't cares
	}

#ifdef USE_BUFFER2
	else
	if (2 == BufferNo)											//program flash page from buffer 2
	{
		DF_SPI_RW(Buf2ToFlashWE);								//buffer 2 to flash with erase op-code
		DF_SPI_RW((unsigned char)(PageAdr >> (16 - PageBits)));	//upper part of page address
		DF_SPI_RW((unsigned char)(PageAdr << (PageBits - 8)));	//lower part of page address
		DF_SPI_RW(0x00);										//don't cares
	}
#endif

	DF_CS_HIGH();												//initiate flash page programming
	DF_CS_LOW();

	while(!(df_read_status() & 0x80));							//monitor the status register, wait until busy-flag is high

	__enable_irq();	// Global interrupt disable (ARM)
	DF_CS_HIGH();
}


/*****************************************************************************
*
*	Function name : DF_BufferReadStr
*
*	Returns :		None
*
*	Parameters :	BufferNo	->	Decides usage of either buffer 1 or 2
*					IntPageAdr	->	Internal page address
*					No_of_bytes	->	Number of bytes to be read
*					*BufferPtr	->	address of buffer to be used for read bytes
*
*	Purpose :		Reads one or more bytes from one of the dataflash
*					internal SRAM buffers, and puts read bytes into
*					buffer pointed to by *BufferPtr
*
******************************************************************************/
void DF_BufferReadStr (unsigned char BufferNo, unsigned int IntPageAdr, unsigned int No_of_bytes, unsigned char *BufferPtr)
{
	unsigned int i;

	DF_CS_LOW();								//to reset dataflash command decoder
	__disable_irq();	// Global interrupt disable (ARM)

	if (1 == BufferNo)							//read byte(s) from buffer 1
	{
		DF_SPI_RW(Buf1Read);					//buffer 1 read op-code
		DF_SPI_RW(0x00); 						//don't cares
		DF_SPI_RW((unsigned char)(IntPageAdr>>8));//upper part of internal buffer address
		DF_SPI_RW((unsigned char)(IntPageAdr));	//lower part of internal buffer address
		DF_SPI_RW(0x00); 						//don't cares
		for( i=0; i<No_of_bytes; i++)
		{
			*(BufferPtr) = DF_SPI_RW(0x00);		//read byte and put it in AVR buffer pointed to by *BufferPtr
			BufferPtr++;						//point to next element in AVR buffer
		}
	}

#ifdef USE_BUFFER2
	else
	if (2 == BufferNo)							//read byte(s) from buffer 2
	{
		DF_SPI_RW(Buf2Read);					//buffer 2 read op-code
		DF_SPI_RW(0x00);						//don't cares
		DF_SPI_RW((unsigned char)(IntPageAdr>>8));//upper part of internal buffer address
		DF_SPI_RW((unsigned char)(IntPageAdr));	//lower part of internal buffer address
		DF_SPI_RW(0x00);						//don't cares
		for( i=0; i<No_of_bytes; i++)
		{
			*(BufferPtr) = DF_SPI_RW(0x00);		//read byte and put it in AVR buffer pointed to by *BufferPtr
			//*(BufferPtr) = DF_SPI_RD();			//read byte and put it in AVR buffer pointed to by *BufferPtr
			BufferPtr++;						//point to next element in AVR buffer
		}
	}
#endif

	__enable_irq();	// Global interrupt enable (ARM)
	DF_CS_HIGH();
}
//NB : Sjekk at (IntAdr + No_of_bytes) < buffersize, hvis ikke blir det bare ball..


/*****************************************************************************
*
*	Function name : DF_BufferWriteEnable
*
*	Returns :		None
*
*	Parameters :	IntPageAdr	->	Internal page address to start writing from
*					BufferAdr	->	Decides usage of either buffer 1 or 2
*
*	Purpose :		Enables continous write functionality to one of the dataflash buffers
*					buffers. NOTE : User must ensure that CS goes high to terminate
*					this mode before accessing other dataflash functionalities
*
******************************************************************************/
void DF_BufferWriteEnable (unsigned char BufferNo, unsigned int IntPageAdr)
{
	DF_CS_LOW();								//to reset dataflash command decoder
	__disable_irq();	// Global interrupt disable (ARM)

	if (1 == BufferNo)							//write enable to buffer 1
	{
		DF_SPI_RW(Buf1Write);					//buffer 1 write op-code
		DF_SPI_RW(0x00); 						//don't cares
		DF_SPI_RW((unsigned char)(IntPageAdr>>8));//upper part of internal buffer address
		DF_SPI_RW((unsigned char)(IntPageAdr));	//lower part of internal buffer address
	}

#ifdef USE_BUFFER2
	else
	if (2 == BufferNo)							//write enable to buffer 2
	{
		DF_SPI_RW(Buf2Write);					//buffer 2 write op-code
		DF_SPI_RW(0x00);						//don't cares
		DF_SPI_RW((unsigned char)(IntPageAdr>>8));//upper part of internal buffer address
		DF_SPI_RW((unsigned char)(IntPageAdr));	//lower part of internal buffer address
	}
#endif

	__enable_irq();	// Global interrupt enable (ARM)
	DF_CS_HIGH();
}


/*****************************************************************************
*
*	Function name : DF_BufferWriteStr
*
*	Returns :		None
*
*	Parameters :	BufferNo	->	Decides usage of either buffer 1 or 2
*					IntPageAdr	->	Internal page address
*					No_of_bytes	->	Number of bytes to be written
*					*BufferPtr	->	address of buffer to be used for copy of bytes
*									from AVR buffer to dataflash buffer 1 (or 2)
*
*	Purpose :		Copies one or more bytes to one of the dataflash
*					internal SRAM buffers from AVR SRAM buffer
*					pointed to by *BufferPtr
*
******************************************************************************/
void DF_BufferWriteStr (unsigned char BufferNo, unsigned int IntPageAdr, unsigned int No_of_bytes, unsigned char *BufferPtr)
{
	unsigned int i;

	DF_CS_LOW();								//to reset dataflash command decoder
	__disable_irq();	// Global interrupt disable (ARM)

	if (1 == BufferNo)							//write byte(s) to buffer 1
	{
		DF_SPI_RW(Buf1Write);					//buffer 1 write op-code
		DF_SPI_RW(0x00); 						//don't cares
		DF_SPI_RW((unsigned char)(IntPageAdr>>8));//upper part of internal buffer address
		DF_SPI_RW((unsigned char)(IntPageAdr));	//lower part of internal buffer address
		for( i=0; i<No_of_bytes; i++)
		{
			DF_SPI_RW(*(BufferPtr));			//write byte pointed at by *BufferPtr to dataflash buffer 1 location
			BufferPtr++;						//point to next element in AVR buffer
		}
	}

#ifdef USE_BUFFER2
	else
	if (2 == BufferNo)							//write byte(s) to buffer 2
	{
		DF_SPI_RW(Buf2Write);					//buffer 2 write op-code
		DF_SPI_RW(0x00);						//don't cares
		DF_SPI_RW((unsigned char)(IntPageAdr>>8));//upper part of internal buffer address
		DF_SPI_RW((unsigned char)(IntPageAdr));	//lower part of internal buffer address
		for( i=0; i<No_of_bytes; i++)
		{
			DF_SPI_RW(*(BufferPtr));			//write byte pointed at by *BufferPtr to dataflash buffer 2 location
			BufferPtr++;						//point to next element in AVR buffer
		}
	}
#endif

	__enable_irq();	// Global interrupt enable (ARM)
	DF_CS_HIGH();
}
//NB : Monitorer busy-flag i status-reg.
//NB : Sjekk at (IntAdr + No_of_bytes) < buffersize, hvis ikke blir det bare ball..






/*****************************************************************************
*
*	Function name : Cont_Flash_Read_Enable
*
*	Returns :		None
*
*	Parameters :	PageAdr		->	Address of flash page where cont.read starts from
*					IntPageAdr	->	Internal page address where cont.read starts from
*
*	Purpose :		Initiates a continuous read from a location in the DataFlash
*
******************************************************************************/
void Cont_Flash_Read_Enable (unsigned int PageAdr, unsigned int IntPageAdr)
{
	DF_CS_LOW();																//to reset dataflash command decoder
	__disable_irq();	// Global interrupt disable (ARM)

	DF_SPI_RW(ContArrayRead);													//Continuous Array Read op-code
	DF_SPI_RW((unsigned char)(PageAdr >> (16 - PageBits)));						//upper part of page address
	DF_SPI_RW((unsigned char)((PageAdr << (PageBits - 8))+ (IntPageAdr>>8)));	//lower part of page address and MSB of int.page adr.
	DF_SPI_RW((unsigned char)(IntPageAdr));										//LSB byte of internal page address
	DF_SPI_RW(0x00);															//perform 4 dummy writes
	DF_SPI_RW(0x00);															//in order to initiate DataFlash
	DF_SPI_RW(0x00);															//address pointers
	DF_SPI_RW(0x00);

	__enable_irq();	// Global interrupt enable (ARM)
	DF_CS_HIGH();
}



