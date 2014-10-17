
#ifndef __GPIOHANDLER_H__
#define __GPIOHANDLER_H__

#include <stdint.h>

#define LEDn                             4

#define LED1_PIN						GPIO_Pin_4
#define LED1_GPIO_PORT					GPIOC
#define LED1_GPIO_CLK					RCC_APB2Periph_GPIOC

#define LED2_PIN						GPIO_Pin_5
#define LED2_GPIO_PORT					GPIOC
#define LED2_GPIO_CLK					RCC_APB2Periph_GPIOC

#define BOOT_PIN						GPIO_Pin_12	
#define BOOT_GPIO_PORT					GPIOC
#define BOOT_GPIO_CLK					RCC_APB2Periph_GPIOC

typedef enum
{
  LED1 = 0,
  LED2 = 1,
  LED3 = 2,
  LED4 = 3
} Led_TypeDef;

void LED_Init(Led_TypeDef Led);
void LED_On(Led_TypeDef Led);
void LED_Off(Led_TypeDef Led);
void LED_Toggle(Led_TypeDef Led);
uint8_t get_LED_Status(Led_TypeDef Led);
void BOOT_Pin_Init();
uint8_t get_bootpin_Status();

#endif
