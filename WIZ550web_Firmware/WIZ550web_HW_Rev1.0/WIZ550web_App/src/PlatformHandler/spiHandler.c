/**
 * @file	spiHandler.c
 * @brief	SPI Handler Example
 * @version 1.0
 * @date	2014/07/15
 * @par Revision
 *			2014/07/15 - 1.0 Release
 * @author	
 * \n\n @par Copyright (C) 1998 - 2014 WIZnet. All rights reserved.
 */

#include <stdio.h>
#include "stm32f10x_conf.h"
#include "stm32f10x.h"
#include "spiHandler.h"
#include "boardutil.h"
#include "wizchip_conf.h"

#ifdef STM32_WIZCHIP_USE_DMA
// #warning "Information only: using DMA"
/*-----------------------------------------------------------------------*/
/* Transmit/Receive Block using DMA (Platform dependent. STM32 here)     */
/*-----------------------------------------------------------------------*/
static void stm32_wizchip_dma_transfer(uint8_t receive, const uint8_t *buff, uint16_t btr);
#endif

/**
  * @brief  Initializes the peripherals used by the W5500 driver.
  * @param  None
  * @retval None
  */
void W5500_SPI_LowLevel_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  SPI_InitTypeDef  SPI_InitStructure;


  /*!< W5500_SPI_CS_GPIO, W5500_SPI_MOSI_GPIO, W5500_SPI_MISO_GPIO
       and W5500_SPI_SCK_GPIO Periph clock enable */
//  RCC_APB1PeriphClockCmd(W5500_CS_GPIO_CLK | W5500_SPI_MOSI_GPIO_CLK | W5500_SPI_MISO_GPIO_CLK |
//                         W5500_SPI_SCK_GPIO_CLK, ENABLE);

  /*!< W5500_SPI Periph clock enable */
//  RCC_APB1PeriphClockCmd(W5500_SPI_CLK, ENABLE);

  /*!< Configure W5500_SPI pins: SCK */
  GPIO_InitStructure.GPIO_Pin = W5500_SPI_SCK_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(W5500_SPI_SCK_GPIO_PORT, &GPIO_InitStructure);

  /*!< Configure W5500_SPI pins: MOSI */
  GPIO_InitStructure.GPIO_Pin = W5500_SPI_MOSI_PIN;
  GPIO_Init(W5500_SPI_MOSI_GPIO_PORT, &GPIO_InitStructure);

  /*!< Configure W5500_SPI pins: MISO */
  GPIO_InitStructure.GPIO_Pin = W5500_SPI_MISO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(W5500_SPI_MISO_GPIO_PORT, &GPIO_InitStructure);

  /*!< Configure W5500_CS_PIN pin: W5500 CS pin */
  GPIO_InitStructure.GPIO_Pin = W5500_CS_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(W5500_CS_GPIO_PORT, &GPIO_InitStructure);


  /*!< SPI configuration */
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
  // ## NEEEEEEEEEEEEEEEEEEEEED SPI init code modified for SD Card Recognize (SPI mode 0)
  //SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  //SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;

  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;

  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(W5500_SPI, &SPI_InitStructure);

  /*!< Enable the W5500_SPI  */
  SPI_Cmd(W5500_SPI, ENABLE);

  GPIO_ResetBits(GPIOB, W5500_RESET_PIN);
  //Delay(DELAY_COUNT); // Is this enough?
  delay_ms(1);
  GPIO_SetBits(GPIOB, W5500_RESET_PIN);

#ifdef STM32_WIZCHIP_USE_DMA
	/* enable DMA clock */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
#endif
}


static uint8_t wizchip_rw(uint8_t byte)
{
 /*!< Loop while DR register in not emplty */
  while (SPI_I2S_GetFlagStatus(W5500_SPI, SPI_I2S_FLAG_TXE) == RESET);

  /*!< Send byte through the SPI2 peripheral */
  SPI_I2S_SendData(W5500_SPI, byte);

  /*!< Wait to receive a byte */
  while (SPI_I2S_GetFlagStatus(W5500_SPI, SPI_I2S_FLAG_RXNE) == RESET);

  /*!< Return the byte read from the SPI bus */
  return SPI_I2S_ReceiveData(W5500_SPI);
}


static void  wizchip_select(void)
{
	GPIO_ResetBits(W5500_CS_GPIO_PORT, W5500_CS_PIN);
}

static void  wizchip_deselect(void)
{
	GPIO_SetBits(W5500_CS_GPIO_PORT, W5500_CS_PIN);
}


void  wizchip_write(uint8_t wb)
{
	wizchip_rw(wb);
}

uint8_t wizchip_read(void)
{
	return wizchip_rw(0xFF);
}

#ifdef STM32_WIZCHIP_USE_DMA
void wizchip_readburst(uint8_t* pBuf, uint16_t len)
{
	stm32_wizchip_dma_transfer(1, pBuf, len);  //FALSE(0) for buff->SPI, TRUE(1) for SPI->buff
}

void  wizchip_writeburst(uint8_t* pBuf, uint16_t len)
{
	stm32_wizchip_dma_transfer(0, pBuf, len);  //FALSE(0) for buff->SPI, TRUE(1) for SPI->buff
}
#endif

/**
  * @brief  Initializes the peripherals used by the W5500 driver.
  * @param  None
  * @retval None
  */
void W5500_Init(void)
{

  /*!< Deselect the FLASH: Chip Select high */
  wizchip_deselect();

  // Wiznet
  reg_wizchip_cs_cbfunc(wizchip_select, wizchip_deselect);
  reg_wizchip_spi_cbfunc(wizchip_read, wizchip_write);

#ifdef STM32_WIZCHIP_USE_DMA
  reg_wizchip_spiburst_cbfunc(wizchip_readburst, wizchip_writeburst);
#endif

  /* wizchip initialize*/
  uint8_t tmp;
  uint8_t memsize[2][8] = { {2,2,2,2,2,2,2,2},{2,2,2,2,2,2,2,2}};

  if(ctlwizchip(CW_INIT_WIZCHIP,(void*)memsize) == -1)
  {
    printf("WIZCHIP Initialized fail.\r\n");
    //return 0;
  }

  /* PHY link status check */
  do {
    if(ctlwizchip(CW_GET_PHYLINK, (void*)&tmp) == -1) {
    	;
    	//myprintf("Unknown PHY Link status.\r\n");
    	//return 0;
    }
  } while (tmp == PHY_LINK_OFF);

}

#ifdef STM32_WIZCHIP_USE_DMA
/*-----------------------------------------------------------------------*/
/* Transmit/Receive Block using DMA (Platform dependent. STM32 here)     */
/*-----------------------------------------------------------------------*/
static
void stm32_wizchip_dma_transfer(
	uint8_t receive,			/* FALSE(0) for buff->SPI, TRUE(1) for SPI->buff      */
	const uint8_t *buff,		/* receive TRUE  : 512 byte data block to be transmitted
						   		receive FALSE : Data buffer to store received data    */
	uint16_t btr 				/* Byte count */
)
{
	DMA_InitTypeDef DMA_InitStructure;
	uint16_t rw_workbyte[] = { 0xffff };

	/* shared DMA configuration values */
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&(W5500_SPI->DR));
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_BufferSize = btr;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

	DMA_DeInit(DMA_Channel_SPI_WIZCHIP_RX);
	DMA_DeInit(DMA_Channel_SPI_WIZCHIP_TX);

	if ( receive ) {

		/* DMA1 channel2 configuration SPI1 RX ---------------------------------------------*/
		/* DMA1 channel4 configuration SPI2 RX ---------------------------------------------*/
		DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)buff;
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
		DMA_Init(DMA_Channel_SPI_WIZCHIP_RX, &DMA_InitStructure);

		/* DMA1 channel3 configuration SPI1 TX ---------------------------------------------*/
		/* DMA1 channel5 configuration SPI2 TX ---------------------------------------------*/
		DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)rw_workbyte;
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
		DMA_Init(DMA_Channel_SPI_WIZCHIP_TX, &DMA_InitStructure);

	} else {

		/* DMA1 channel2 configuration SPI1 RX ---------------------------------------------*/
		/* DMA1 channel4 configuration SPI2 RX ---------------------------------------------*/
		DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)rw_workbyte;
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
		DMA_Init(DMA_Channel_SPI_WIZCHIP_RX, &DMA_InitStructure);

		/* DMA1 channel3 configuration SPI1 TX ---------------------------------------------*/
		/* DMA1 channel5 configuration SPI2 TX ---------------------------------------------*/
		DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)buff;
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
		DMA_Init(DMA_Channel_SPI_WIZCHIP_TX, &DMA_InitStructure);
	}

	/* Enable DMA RX Channel */
	DMA_Cmd(DMA_Channel_SPI_WIZCHIP_RX, ENABLE);
	/* Enable DMA TX Channel */
	DMA_Cmd(DMA_Channel_SPI_WIZCHIP_TX, ENABLE);

	/* Enable SPI TX/RX request */
	SPI_I2S_DMACmd(W5500_SPI, SPI_I2S_DMAReq_Rx | SPI_I2S_DMAReq_Tx, ENABLE);

	/* Wait until DMA1_Channel 3 Transfer Complete */
	/// not needed: while (DMA_GetFlagStatus(DMA_FLAG_SPI_WIZCHIP_TC_TX) == RESET) { ; }
	/* Wait until DMA1_Channel 2 Receive Complete */
	while (DMA_GetFlagStatus(DMA_FLAG_SPI_WIZCHIP_TC_RX) == RESET) { ; }
	// same w/o function-call:
	// while ( ( ( DMA1->ISR ) & DMA_FLAG_SPI_WIZCHIP_TC_RX ) == RESET ) { ; }

	/* Disable DMA RX Channel */
	DMA_Cmd(DMA_Channel_SPI_WIZCHIP_RX, DISABLE);
	/* Disable DMA TX Channel */
	DMA_Cmd(DMA_Channel_SPI_WIZCHIP_TX, DISABLE);

	/* Disable SPI RX/TX request */
	SPI_I2S_DMACmd(W5500_SPI, SPI_I2S_DMAReq_Rx | SPI_I2S_DMAReq_Tx, DISABLE);
}
#endif /* STM32_SD_USE_DMA */
