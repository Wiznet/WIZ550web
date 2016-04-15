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

#ifndef __UARTHANDLER_H__
#define __UARTHANDLER_H__

#include "ring_buffer.h"

#define USART1_TX		GPIO_Pin_9	// out
#define USART1_RX		GPIO_Pin_10	// in
#define USART1_CTS		GPIO_Pin_11	// in
#define USART1_RTS		GPIO_Pin_12	// out

#define UART_SRB_SIZE 1024	/* Send */
#define UART_RRB_SIZE 1024	/* Receive */

extern RINGBUFF_T txring1, rxring1;
extern uint32_t baud_table[11];

void USART1_Configuration(void);

int UART_read(void *data, int bytes);
uint32_t UART_write(void *data, int bytes);
int UART_read_blk(void *data, int bytes);
void myprintf(char *fmt, ...);

#endif

