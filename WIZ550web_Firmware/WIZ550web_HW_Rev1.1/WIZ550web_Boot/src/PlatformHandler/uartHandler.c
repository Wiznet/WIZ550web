/**
 * @file	uartHandler.c
 * @brief	UART Handler Example
 * @version 1.0
 * @date	2014/07/15
 * @par Revision
 *			2014/07/15 - 1.0 Release
 * @author	
 * \n\n @par Copyright (C) 1998 - 2014 WIZnet. All rights reserved.
 */

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "stm32f10x.h"
#include "uartHandler.h"

RINGBUFF_T txring1, rxring1;
static uint8_t rxbuff[UART_RRB_SIZE], txbuff[UART_SRB_SIZE];

uint32_t baud_table[11] = {
	300,
	600,
	1200,
	2400,
	4800,
	9600,
	19200,
	38400,
	57600,
	115200, 
	230400
};

#if 0
uint32_t Chip_UART_SendRB(USART_TypeDef *pUART, RINGBUFF_T *pRB, const void *data, int bytes)
{
	uint32_t ret;
	uint8_t *p8 = (uint8_t *) data;

	/* Don't let UART transmit ring buffer change in the UART IRQ handler */
	Chip_UART_IntDisable(pUART, UART_IER_THREINT);

	/* Move as much data as possible into transmit ring buffer */
	ret = RingBuffer_InsertMult(pRB, p8, bytes);
	Chip_UART_TXIntHandlerRB(pUART, pRB);

	/* Add additional data to transmit ring buffer if possible */
	ret += RingBuffer_InsertMult(pRB, (p8 + ret), (bytes - ret));

	/* Enable UART transmit interrupt */
	Chip_UART_IntEnable(pUART, UART_IER_THREINT);

	return ret;
}
#else
static uint32_t Chip_UART_SendRB(USART_TypeDef *pUART, RINGBUFF_T *pRB, const void *data, int bytes)
{
	uint32_t ret;
	uint8_t *p8 = (uint8_t *) data;

	/* Don't let UART transmit ring buffer change in the UART IRQ handler */
	USART_ITConfig(pUART, USART_IT_TXE, DISABLE);

	/* Move as much data as possible into transmit ring buffer */
	ret = RingBuffer_InsertMult(pRB, p8, bytes);

	/* Enable UART transmit interrupt */
	USART_ITConfig(pUART, USART_IT_TXE, ENABLE);

	return ret;
}
#endif

#if 0
int Chip_UART_ReadRB(USART_TypeDef *pUART, RINGBUFF_T *pRB, void *data, int bytes)
{
	(void) pUART;
	int ret, is_full;

	is_full = RingBuffer_IsFull(pRB);
	ret = RingBuffer_PopMult(pRB, (uint8_t *) data, bytes);

	if(is_full)
		Chip_UART_IntEnable(LPC_USART, (UART_IER_RBRINT | UART_IER_RLSINT));

	return ret;
}
#else
int Chip_UART_ReadRB(USART_TypeDef *pUART, RINGBUFF_T *pRB, void *data, int bytes)
{
	(void) pUART;
	int ret;

	USART_ITConfig(pUART, USART_IT_RXNE, DISABLE);

	ret = RingBuffer_PopMult(pRB, (uint8_t *) data, bytes);

	USART_ITConfig(pUART, USART_IT_RXNE, ENABLE);

	return ret;
}
#endif

int Chip_UART_ReadRB_BLK(USART_TypeDef *pUART, RINGBUFF_T *pRB, void *data, int bytes)
{
	(void) pUART;
	int ret;

	while(RingBuffer_IsEmpty(pRB));

	USART_ITConfig(pUART, USART_IT_RXNE, DISABLE);

	ret = RingBuffer_PopMult(pRB, (uint8_t *) data, bytes);

	USART_ITConfig(pUART, USART_IT_RXNE, ENABLE);

	return ret;
}

/**
 * @brief  USART Handler
 * @param  None
 * @return None
 */
void Chip_UART_IRQRBHandler(USART_TypeDef *pUART, RINGBUFF_T *pRXRB, RINGBUFF_T *pTXRB)
{
	uint8_t ch;

	/* Handle transmit interrupt if enabled */
	if(USART_GetITStatus(pUART, USART_IT_TXE) != RESET) {
		if(RingBuffer_Pop(pTXRB, &ch))
			USART_SendData(pUART, ch); 
		else												// RingBuffer Empty
			USART_ITConfig(pUART, USART_IT_TXE, DISABLE);
	}

	/* Handle receive interrupt */
	if(USART_GetITStatus(pUART, USART_IT_RXNE) != RESET) {
		if(RingBuffer_IsFull(pRXRB)) {
			// Buffer Overflow
		} else {
			ch = USART_ReceiveData(pUART);
			RingBuffer_Insert(pRXRB, &ch);
		}
	}
}

/**
 * @brief  USART1 Interrupt Handler
 * @param  None
 * @return None
 */
void USART1_IRQHandler(void)
{
	Chip_UART_IRQRBHandler(USART1, &rxring1, &txring1);
}

/**
 * @brief  Configures the USART1
 * @param  None
 * @return None
 */
void USART1_Configuration(void)
{
	USART_InitTypeDef USART_InitStructure;
	USART_ClockInitTypeDef USART_ClockInitStruct;

	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	/* Ring Buffer */
	RingBuffer_Init(&rxring1, rxbuff, 1, UART_RRB_SIZE);
	RingBuffer_Init(&txring1, txbuff, 1, UART_SRB_SIZE);

	/* Enable the USART Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* Configure USART Tx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin =  USART1_TX | USART1_RTS;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Configure USART Rx as input floating */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin = USART1_RX | USART1_CTS;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* USARTx configuration ------------------------------------------------------*/
	/* USARTx configured as follow:
	   - BaudRate = 115200 baud
	   - Word Length = 8 Bits
	   - One Stop Bit
	   - No parity
	   - Hardware flow control disabled (RTS and CTS signals)
	   - Receive and transmit enabled
	 */
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_ClockInitStruct.USART_Clock = USART_Clock_Disable;
	USART_ClockInitStruct.USART_CPOL = USART_CPOL_Low;
	USART_ClockInitStruct.USART_CPHA = USART_CPHA_2Edge;
	USART_ClockInitStruct.USART_LastBit = USART_LastBit_Disable;

	/* Configure the USARTx */
	USART_Init(USART1, &USART_InitStructure);
	USART_ClockInit(USART1, &USART_ClockInitStruct);

	/* Enable USARTy Receive and Transmit interrupts */
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); 

	/* Enable the USARTx */
	USART_Cmd(USART1, ENABLE);
}

int UART_read(void *data, int bytes)
{
	return Chip_UART_ReadRB(USART1 , &rxring1, data, bytes);
}

uint32_t UART_write(void *data, int bytes)
{
	return Chip_UART_SendRB(USART1, &txring1, data, bytes);
}

int UART_read_blk(void *data, int bytes)
{
	return Chip_UART_ReadRB_BLK(USART1 , &rxring1, data, bytes);
}

void myprintf(char *fmt, ...)
{
	va_list arg_ptr;
	char etxt[128]; // buffer size

	va_start(arg_ptr, fmt);
	vsprintf(etxt, fmt, arg_ptr);
	va_end(arg_ptr);

	UART_write(etxt, strlen(etxt));
}
