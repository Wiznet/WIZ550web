/*
*
@file		util.h
@brief
*/


#ifndef __BOARDUTIL_H__ 
#define __BOARDUTIL_H__ 

#include <stdint.h>
#include "types.h"

//#define FACTORY_FW
//#define FACTORY_FW_ONCE
//#define FACTORY_FW_FLASH
#define F_SPI_FLASH
#ifdef F_SPI_FLASH
	//#define F_SPI_FLASH_ONLY
#endif
//#define _FS_DEBUG_


#define DELAY_COUNT    0xEFFFF  // just for test


// Port A
#define USART2_CTS		GPIO_Pin_0	// in
#define USART2_RTS		GPIO_Pin_1	// out
#define USART2_TX		GPIO_Pin_2	// out
#define USART2_RX		GPIO_Pin_3	// in

#define ADC_0			GPIO_Pin_7
#define ADC_1			GPIO_Pin_6
#define ADC_2			GPIO_Pin_5
#define ADC_3			GPIO_Pin_4

#define USART1_TX		GPIO_Pin_9	// out
#define USART1_RX		GPIO_Pin_10	// in
#define USART1_CTS		GPIO_Pin_11	// in
#define USART1_RTS		GPIO_Pin_12	// out


// Port B
//#define RESET_W5500		GPIO_Pin_0	//out
#define INT_W5500		GPIO_Pin_1	//in

#define CS_EX_Flash		GPIO_Pin_11	//out

// Digital I/O
#define DATA_0			GPIO_Pin_6	// inout
#define DATA_1			GPIO_Pin_7	// inout
#define DATA_2			GPIO_Pin_8	// inout
#define DATA_3			GPIO_Pin_9	// inout
#define DATA_4			GPIO_Pin_8	// inout
#define DATA_5			GPIO_Pin_15	// inout
#define DATA_6			GPIO_Pin_10	// inout
#define DATA_7			GPIO_Pin_11	// inout
#define DATA_8			GPIO_Pin_12	// inout
#define DATA_9			GPIO_Pin_2	// inout
#define DATA_10			GPIO_Pin_3	// inout
#define DATA_11			GPIO_Pin_4	// inout
#define DATA_12			GPIO_Pin_5	// inout
#define DATA_13			GPIO_Pin_6	// inout
#define DATA_14			GPIO_Pin_7	// inout
#define DATA_15			GPIO_Pin_8	// inout

// LED
typedef enum
{
  LED1 = 0,
  LED2 = 1,
  LED3 = 2,
  LED4 = 3
} Led_TypeDef;

#define LEDn                             4

// ADC
typedef enum
{
  A0 = 0,
  A1 = 1,
  A2 = 2,
  A3 = 3
} ADC_Type;

//Data IO
typedef enum
{
  D0 = 0,
  D1 = 1,
  D2 = 2,
  D3 = 3,
  D4 = 4,
  D5 = 5,
  D6 = 6,
  D7 = 7,
  D8 = 8,
  D9 = 9,
  D10 = 10,
  D11 = 11,
  D12 = 12,
  D13 = 13,
  D14 = 14,
  D15 = 15
} IO_Type;

#define IOn							 16

// Data IO Direction
typedef enum
{
  NotUsed 	= 0,
  Input 	= 1,
  Output 	= 2
} IO_Direction_Type;

// Data IO Status
typedef enum
{
  Off 	= 0,
  On 	= 1
} IO_Status_Type;

//#define CHINA_BOARD

#ifdef CHINA_BOARD

#define LED1_PIN                         GPIO_Pin_0
#define LED1_GPIO_PORT                   GPIOA
#define LED1_GPIO_CLK                    RCC_APB2Periph_GPIOC

#define LED2_PIN                         GPIO_Pin_1
#define LED2_GPIO_PORT                   GPIOA
#define LED2_GPIO_CLK                    RCC_APB2Periph_GPIOC

#define LED3_PIN                         GPIO_Pin_2
#define LED3_GPIO_PORT                   GPIOA
#define LED3_GPIO_CLK                    RCC_APB2Periph_GPIOC

#define LED4_PIN                         GPIO_Pin_3
#define LED4_GPIO_PORT                   GPIOA
#define LED4_GPIO_CLK                    RCC_APB2Periph_GPIOC

#else

//#define LED1_PIN                         GPIO_Pin_13
//#define LED1_GPIO_PORT                   GPIOA
//#define LED1_GPIO_CLK                    RCC_APB2Periph_GPIOC

//#define LED2_PIN                         GPIO_Pin_14
//#define LED2_GPIO_PORT                   GPIOA
//#define LED2_GPIO_CLK                    RCC_APB2Periph_GPIOC

#define LED1_PIN						GPIO_Pin_4
#define LED1_GPIO_PORT					GPIOC
#define LED1_GPIO_CLK					RCC_APB2Periph_GPIOC

#define LED2_PIN						GPIO_Pin_5
#define LED2_GPIO_PORT					GPIOC
#define LED2_GPIO_CLK					RCC_APB2Periph_GPIOC

#define D0_PIN							GPIO_Pin_6
#define D0_GPIO_PORT					GPIOC
#define D0_GPIO_CLK						RCC_APB2Periph_GPIOC

#define D1_PIN							GPIO_Pin_7
#define D1_GPIO_PORT					GPIOC
#define D1_GPIO_CLK 					RCC_APB2Periph_GPIOC

#define D2_PIN							GPIO_Pin_8
#define D2_GPIO_PORT					GPIOC
#define D2_GPIO_CLK 					RCC_APB2Periph_GPIOC

#define D3_PIN							GPIO_Pin_9
#define D3_GPIO_PORT					GPIOC
#define D3_GPIO_CLK 					RCC_APB2Periph_GPIOC

#define D4_PIN							GPIO_Pin_8
#define D4_GPIO_PORT					GPIOA
#define D4_GPIO_CLK 					RCC_APB2Periph_GPIOA

#define D5_PIN							GPIO_Pin_15
#define D5_GPIO_PORT					GPIOA
#define D5_GPIO_CLK 					RCC_APB2Periph_GPIOA

#define D6_PIN							GPIO_Pin_10
#define D6_GPIO_PORT					GPIOC
#define D6_GPIO_CLK 					RCC_APB2Periph_GPIOC

#define D7_PIN							GPIO_Pin_11
#define D7_GPIO_PORT					GPIOC
#define D7_GPIO_CLK 					RCC_APB2Periph_GPIOC

#define D8_PIN							GPIO_Pin_12
#define D8_GPIO_PORT					GPIOC
#define D8_GPIO_CLK 					RCC_APB2Periph_GPIOC

#define D9_PIN							GPIO_Pin_2
#define D9_GPIO_PORT					GPIOD
#define D9_GPIO_CLK 					RCC_APB2Periph_GPIOD

#define D10_PIN							GPIO_Pin_3
#define D10_GPIO_PORT					GPIOB
#define D10_GPIO_CLK 					RCC_APB2Periph_GPIOB

#define D11_PIN							GPIO_Pin_4
#define D11_GPIO_PORT					GPIOB
#define D11_GPIO_CLK 					RCC_APB2Periph_GPIOB

#define D12_PIN							GPIO_Pin_5
#define D12_GPIO_PORT					GPIOB
#define D12_GPIO_CLK 					RCC_APB2Periph_GPIOB

#define D13_PIN							GPIO_Pin_6
#define D13_GPIO_PORT					GPIOB
#define D13_GPIO_CLK 					RCC_APB2Periph_GPIOB

#define D14_PIN							GPIO_Pin_7
#define D14_GPIO_PORT					GPIOB
#define D14_GPIO_CLK 					RCC_APB2Periph_GPIOB

#define D15_PIN							GPIO_Pin_8
#define D15_GPIO_PORT					GPIOB
#define D15_GPIO_CLK 					RCC_APB2Periph_GPIOB

#endif


/**
  * @brief  W5500 SPI Interface pins
  */
#define W5500_SPI                       SPI2
#define W5500_SPI_CLK                   RCC_APB1Periph_SPI2

#define W5500_SPI_SCK_PIN               GPIO_Pin_13                  /* PB.13 */
#define W5500_SPI_SCK_GPIO_PORT         GPIOB                       /* GPIOB */
#define W5500_SPI_SCK_GPIO_CLK          RCC_APB2Periph_GPIOB

#define W5500_SPI_MISO_PIN              GPIO_Pin_14                  /* PB.14 */
#define W5500_SPI_MISO_GPIO_PORT        GPIOB                       /* GPIOB */
#define W5500_SPI_MISO_GPIO_CLK         RCC_APB2Periph_GPIOB

#define W5500_SPI_MOSI_PIN              GPIO_Pin_15                  /* PB.15 */
#define W5500_SPI_MOSI_GPIO_PORT        GPIOB                       /* GPIOB */
#define W5500_SPI_MOSI_GPIO_CLK         RCC_APB2Periph_GPIOB

#define W5500_CS_PIN                    GPIO_Pin_12                  /* PB.12 */
#define W5500_CS_GPIO_PORT              GPIOB                       /* GPIOB */
#define W5500_CS_GPIO_CLK               RCC_APB2Periph_GPIOB

#define W5500_DUMMY_BYTE		0xFF

#ifdef CHINA_BOARD
#define W5500_RESET_PIN                 GPIO_Pin_9   // PB.9 China board
#else
#define W5500_RESET_PIN 				GPIO_Pin_0	 // PB.0
#endif

//#define RESET_W5500                 GPIO_Pin_0   // PB.0

//void RCC_Configuration(void);
void NVIC_Configuration(void);
void IWDG_Configureation(void);		// Independent Watchdog
//void GPIO_Configuration(void);
void LED_Init(Led_TypeDef Led);
void LED_On(Led_TypeDef Led);
void LED_Off(Led_TypeDef Led);
void LED_Toggle(Led_TypeDef Led);
uint8 get_LED_Status(Led_TypeDef Led);
//void Timer_Configuration(void);
//void USART1_Configuration(void);
void myprintf(char *fmt, ...);

void Delay(volatile uint32_t nCount);
void delay_us (const uint32_t usec);
void delay_ms (const uint32_t msec);
#if defined(FACTORY_FW)
int check_factory_flag (void);
void check_factory_uart1 (void);
void check_RS422 (uint8_t * buf);
void save_factory_flag (void);
void release_factory_flag (void);
void factory_test_1st (void);
void factory_test_2nd (void);
void factory_run(void);

extern int g_factoryfw_flag;
#endif

#if defined(F_SPI_FLASH)
int check_spiflash_flag(void);
void save_spiflash_flag(void);
void release_spiflash_flag(void);
void sflash_run(void);
int g_spiflash_flag;
#endif
#endif
