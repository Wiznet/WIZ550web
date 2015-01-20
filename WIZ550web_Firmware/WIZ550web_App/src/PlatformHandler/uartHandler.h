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

#ifndef __UARTHANDLER_H
#define __UARTHANDLER_H

#include <stdint.h>
#include "stm32f10x.h"
#include "ConfigData.h"

#define UART_SRB_SIZE 1024	/* Send */
#define UART_RRB_SIZE 1024	/* Receive */

void serial_info_init(USART_TypeDef *pUART, struct __serial_info *serial);

void USART1_Configuration(void);
void USART2_Configuration(void);

int UART_read(void *data, int bytes);
uint32_t UART_write(void *data, int bytes);
int UART_RxRB_GetCount();
int UART_TxRB_GetCount();
void UART2_flush(void);

uint8_t * getUSART1buf(void);
void USART1_flush(void);

uint8_t Usart_putc(uint8_t ch);
uint8_t Usart_getc(void);

#endif

