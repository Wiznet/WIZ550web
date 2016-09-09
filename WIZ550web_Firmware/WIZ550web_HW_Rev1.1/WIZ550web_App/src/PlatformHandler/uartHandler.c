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

#include <string.h>
#include "stm32f10x.h"
#include "boardutil.h"
#include "ring_buffer.h"
#include "uartHandler.h"
#include "ConfigData.h"

uint32_t baud_table[10] = {
	baud_600,
	baud_1200,
	baud_2400,
	baud_4800,
	baud_9600,
	baud_19200,
	baud_38400,
	baud_57600,
	baud_115200, 
	baud_230400
};

// Eric added for using printf() ///////////////////////////////////////////////
#define COMPILER_GCC_ARM

#ifdef COMPILER_GCC_ARM
#include <sys/unistd.h>
#include <errno.h>

//uint8_t Usart_getc(void);
//uint8_t Usart_putc(uint8_t ch);
int _read(int file, char *ptr, int len);
int _write(int file, char *ptr, int len);

#define USART1_RX_INTERRUPT VAL_ENABLE
//#define USART2_RX_INTERRUPT VAL_ENABLE
//#define SYSTICK_HZ			1000

//extern void EXTI_IMR_EMR_enable(void);

//static __IO uint32_t msTicks = 0;	// Max: about 50 days
static USART_TypeDef *std_usart = USART1;

#if (USART1_RX_INTERRUPT == VAL_ENABLE)
#define U1RX_BUF_SIZE		300

uint8_t  u1rx_buf[U1RX_BUF_SIZE];
uint16_t u1rx_wr=0, u1rx_rd=0;

uint8_t * getUSART1buf(void)
{
	uint8_t * u1rx_buf_ptr = u1rx_buf;
	uint16_t slen = strlen((char *)u1rx_buf);

	if(slen) u1rx_buf_ptr = u1rx_buf;
	else u1rx_buf_ptr = 0;

	return u1rx_buf_ptr;
}

void USART1_flush(void)
{
	u1rx_wr = 0;
	u1rx_rd = 0;
	memset(u1rx_buf, 0x00, U1RX_BUF_SIZE);
}

void USART1_IRQHandler(void)	// USART1 ISR
{
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
		if( (u1rx_wr > u1rx_rd && u1rx_wr-u1rx_rd >= U1RX_BUF_SIZE-1) ||
			(u1rx_wr < u1rx_rd && u1rx_rd == u1rx_wr+1) )	// Buffer Overflow
		{
			//USART_ClearITPendingBit(USART1, USART_IT_RXNE);
			//wizpf_led_set(WIZ_LED1, VAL_TOG);
			USART_SendData(USART1, (uint8_t)'@');
			return;
		}
		u1rx_buf[u1rx_wr] = (uint8_t)USART_ReceiveData(USART1);
		if(u1rx_wr < U1RX_BUF_SIZE-1) u1rx_wr++;
		else u1rx_wr = 0;
	}
}
#endif

uint8_t Usart_putc(uint8_t ch)
{
	USART_SendData(std_usart, (uint8_t)ch);
	while(USART_GetFlagStatus(std_usart, USART_FLAG_TXE) == RESET);
	return ch;
}

uint8_t Usart_getc(void)
{
#if (USART1_RX_INTERRUPT == VAL_ENABLE)
	if(std_usart == USART1) {
		while(u1rx_rd == u1rx_wr);
		if(u1rx_rd < U1RX_BUF_SIZE-1) {
			u1rx_rd++;
			return u1rx_buf[u1rx_rd-1];
		} else {
			u1rx_rd = 0;
			return u1rx_buf[U1RX_BUF_SIZE-1];
		}
	}
#endif

#if (USART2_RX_INTERRUPT == VAL_ENABLE)
	if(std_usart == USART2) {
		while(u2rx_rd == u2rx_wr);
		if(u2rx_rd < U2RX_BUF_SIZE-1) {
			u2rx_rd++;
			return u2rx_buf[u2rx_rd-1];
		} else {
			u2rx_rd = 0;
			return u2rx_buf[U2RX_BUF_SIZE-1];
		}
	}
#endif

#if (USART1_RX_INTERRUPT == VAL_DISABLE) || (USART2_RX_INTERRUPT == VAL_DISABLE)
	while(USART_GetFlagStatus(std_usart, USART_FLAG_RXNE) == RESET);
	return (uint8_t)USART_ReceiveData(std_usart);
#else
	return RET_FAIL;
#endif
}

int _read(int file, char *ptr, int len)
{
    int n;
    int num = 0;
    switch (file) {
    case STDIN_FILENO:
        for (n = 0; n < len; n++) {
            char c = Usart_getc();
            *ptr++ = c;
            num++;
        }
        break;
    default:
        errno = EBADF;
        return -1;
    }
    return num;
}

int _write(int file, char *ptr, int len)
{
    int n;
    switch (file) {
    case STDOUT_FILENO: /*stdout*/
        for (n = 0; n < len; n++) {
        	Usart_putc(*ptr++ & (uint16_t)0x01FF);
        }
        break;
    case STDERR_FILENO: /* stderr */
        for (n = 0; n < len; n++) {
        	Usart_putc(*ptr++ & (uint16_t)0x01FF);
        }
        break;
    default:
        errno = EBADF;
        return -1;
    }
    return len;
}

#endif
////////////////////////////////////////////////////////////////////////////////

void serial_info_init(USART_TypeDef *pUART, struct __serial_info *serial)
{
	USART_InitTypeDef USART_InitStructure;

	uint32_t i, loop, valid_arg = 0;

	loop = sizeof(baud_table) / sizeof(baud_table[0]);
	for(i = 0 ; i < loop ; i++) {
		if(serial->baud_rate == baud_table[i]) {
			USART_InitStructure.USART_BaudRate = serial->baud_rate;
			valid_arg = 1;
			break;
		}
	}
	if(!valid_arg)
		USART_InitStructure.USART_BaudRate = baud_115200;

	/* Set Data Bits */
	switch(serial->data_bits) {
		case word_len8:
			USART_InitStructure.USART_WordLength = USART_WordLength_8b;
			break;
		case word_len9:
			USART_InitStructure.USART_WordLength = USART_WordLength_9b;
			break;
		default:
			USART_InitStructure.USART_WordLength = USART_WordLength_8b;
			serial->data_bits = word_len8;
			break;
	}

	/* Set Stop Bits */
	switch(serial->stop_bits) {
		case stop_bit1:
			USART_InitStructure.USART_StopBits = USART_StopBits_1;
			break;
		case stop_bit2:
			USART_InitStructure.USART_StopBits = USART_StopBits_2;
			break;
		default:
			USART_InitStructure.USART_StopBits = USART_StopBits_1;
			serial->stop_bits = stop_bit1;
			break;
	}

	/* Set Parity Bits */
	switch(serial->parity) {
		case parity_none:
			USART_InitStructure.USART_Parity = USART_Parity_No;
			break;
		case parity_odd:
			USART_InitStructure.USART_Parity = USART_Parity_Odd;
			break;
		case parity_even:
			USART_InitStructure.USART_Parity = USART_Parity_Even;
			break;
		default:
			USART_InitStructure.USART_Parity = USART_Parity_No;
			serial->parity = parity_none;
			break;
	}

	/* Flow Contrtol */
	switch(serial->parity) {
	case flow_none:
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
		break;
	case flow_rts_cts:
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_RTS_CTS;
		break;
	default:
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
		break;
	}

	/* Configure the USARTx */
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(pUART, &USART_InitStructure);
}

/**
  * @brief  Configures the USART1
  * @param  None
  * @return None
  */

void USART1_Configuration(void)
{
  //USART_InitTypeDef USART_InitStructure;
  USART_ClockInitTypeDef USART_ClockInitStruct;
  GPIO_InitTypeDef GPIO_InitStructure;

  S2E_Packet *value = get_S2E_Packet_pointer();

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
  /*
  // Default USART1 Settings
  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No ;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USART1, &USART_InitStructure);
  */
  USART_ClockInitStruct.USART_Clock = USART_Clock_Disable;
  USART_ClockInitStruct.USART_CPOL = USART_CPOL_Low;
  USART_ClockInitStruct.USART_CPHA = USART_CPHA_2Edge;
  USART_ClockInitStruct.USART_LastBit = USART_LastBit_Disable;

  /* Configure the USARTx */
  serial_info_init(USART1, &(value->serial_info[0])); // Load USART1 Settings from Flash
  USART_ClockInit(USART1, &USART_ClockInitStruct);

  /* Enable the USARTx */
  USART_Cmd(USART1, ENABLE);

}

RINGBUFF_T txring2, rxring2;
static uint8_t rxbuff2[UART_RRB_SIZE], txbuff2[UART_SRB_SIZE];

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

int Chip_UART_ReadRB(USART_TypeDef *pUART, RINGBUFF_T *pRB, void *data, int bytes)
{
	(void) pUART;
	int ret;

	USART_ITConfig(pUART, USART_IT_RXNE, DISABLE);

	ret = RingBuffer_PopMult(pRB, (uint8_t *) data, bytes);

	USART_ITConfig(pUART, USART_IT_RXNE, ENABLE);

	return ret;
}

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

void USART2_IRQHandler(void)
{
	Chip_UART_IRQRBHandler(USART2, &rxring2, &txring2);
}

void USART2_Configuration(void)
{
	//USART_InitTypeDef USART_InitStructure;
	USART_ClockInitTypeDef USART_ClockInitStruct;

	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	S2E_Packet *value = get_S2E_Packet_pointer();

	/* Ring Buffer */
	RingBuffer_Init(&rxring2, rxbuff2, 1, UART_RRB_SIZE);
	RingBuffer_Init(&txring2, txbuff2, 1, UART_SRB_SIZE);

	/* Enable the USART Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* Configure USART Tx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin =  USART2_TX | USART2_RTS;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Configure USART Rx as input floating */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin = USART2_RX | USART2_CTS;
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
	/*
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART2, &USART_InitStructure);
	*/
	USART_ClockInitStruct.USART_Clock = USART_Clock_Disable;
	USART_ClockInitStruct.USART_CPOL = USART_CPOL_Low;
	USART_ClockInitStruct.USART_CPHA = USART_CPHA_2Edge;
	USART_ClockInitStruct.USART_LastBit = USART_LastBit_Disable;

	/* Configure the USARTx */
	serial_info_init(USART2, &(value->serial_info[1])); // Load USART2 Settings from Flash
	USART_ClockInit(USART2, &USART_ClockInitStruct);

	/* Enable USARTy Receive and Transmit interrupts */
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); 

	/* Enable the USARTx */
	USART_Cmd(USART2, ENABLE);
}

int UART_read(void *data, int bytes)
{
	return Chip_UART_ReadRB(USART2 , &rxring2, data, bytes);
}

uint32_t UART_write(void *data, int bytes)
{
	return Chip_UART_SendRB(USART2, &txring2, data, bytes);
}

int UART_RxRB_GetCount()
{
	return RingBuffer_GetCount(&rxring2);
}

int UART_TxRB_GetCount()
{
	return RingBuffer_GetCount(&txring2);
}

void UART2_flush(void)
{
	//RingBuffer_Init(&rxring2, rxbuff2, 1, UART_RRB_SIZE);
	RingBuffer_Init(&txring2, txbuff2, 1, UART_SRB_SIZE);
}
