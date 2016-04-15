/**
 * @file	uartHandler.h
 * @brief	Header File for UART Handler Example
 * @version 1.0
 * @date	2014/07/15
 * @par Revision
 *			2014/07/15 - 1.0 Release
 * @author	
 * \n\n @par Copyright (C) 1998 - 2014 WIZnet. All rights reserved.
 */

#ifndef __SPIHANDLER_H
#define __SPIHANDLER_H

#define STM32_WIZCHIP_USE_DMA
#ifdef STM32_WIZCHIP_USE_DMA
#define DMA_Channel_SPI_WIZCHIP_RX    	DMA1_Channel4
#define DMA_Channel_SPI_WIZCHIP_TX    	DMA1_Channel5
#define DMA_FLAG_SPI_WIZCHIP_TC_RX    	DMA1_FLAG_TC4
#define DMA_FLAG_SPI_WIZCHIP_TC_TX    	DMA1_FLAG_TC5
#endif


void  wizchip_write(uint8_t wb);
uint8_t wizchip_read(void);
void W5500_Init(void);
void W5500_SPI_LowLevel_Init(void);

#endif

