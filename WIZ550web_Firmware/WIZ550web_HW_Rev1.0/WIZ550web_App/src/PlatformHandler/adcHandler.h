/**
 * @file	adcHandler.h
 * @brief	Header File for ADC Handler Example
 * @version 1.0
 * @date	2014/07/15
 * @par Revision
 *			2014/07/15 - 1.0 Release
 * @author	
 * \n\n @par Copyright (C) 1998 - 2014 WIZnet. All rights reserved.
 */

#ifndef __ADCHANDLER_H
#define __ADCHANDLER_H

#include <stdint.h>
#include "stm32f10x.h"

#define ADC12_IN4_PORT		GPIOA
#define ADC12_IN4_PIN		GPIO_Pin_4

#define ADC12_IN5_PORT		GPIOA
#define ADC12_IN5_PIN		GPIO_Pin_5

#define ADC12_IN6_PORT		GPIOA
#define ADC12_IN6_PIN		GPIO_Pin_6

#define ADC12_IN7_PORT		GPIOA
#define ADC12_IN7_PIN		GPIO_Pin_7

#define ADC12_IN13_PORT		GPIOC
#define ADC12_IN13_PIN		GPIO_Pin_3

void adc_gpio_init(uint8_t ADC_Channel);
void adc_init(uint8_t ADC_Type);
void adc_dma_init(void);
void adc_dmamulti_init(void);

extern __IO uint16_t ADC1ConvertedValue;
extern vu16 ADC_DualConvertedValueTab[4];

#endif

