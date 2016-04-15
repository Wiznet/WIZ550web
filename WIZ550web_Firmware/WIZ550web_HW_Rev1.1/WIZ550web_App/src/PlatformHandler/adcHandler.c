/**
 * @file	adcHandler.c
 * @brief	ADC Handler Example
 * @version 1.0
 * @date	2014/07/15
 * @par Revision
 *			2014/07/15 - 1.0 Release
 * @author	
 * \n\n @par Copyright (C) 1998 - 2014 WIZnet. All rights reserved.
 */

#include "adcHandler.h"
#include "stm32f10x.h"
#include "boardutil.h"

#define ADC1_DR_Address    (uint32_t)(ADC1_BASE + 0x4C)
__IO uint16_t ADC1ConvertedValue = 0;
vu16 ADC_DualConvertedValueTab[4];

/**
 * @brief To initialize GPIO for using ADC.
 * @param ADC_Channel Use A0~A3 on a WIZnet module
 * @return None
 */
void adc_gpio_init(uint8_t ADC_Channel) 
{
	GPIO_InitTypeDef GPIO_InitStructure;										
	
	if( ADC_Channel == ADC_Channel_4 ) // A3 PA.4
	{
		GPIO_InitStructure.GPIO_Pin = ADC12_IN4_PIN;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
		GPIO_Init(ADC12_IN4_PORT, &GPIO_InitStructure); 
	}
	
	if( ADC_Channel == ADC_Channel_5 ) // A2 PA.5
	{
		GPIO_InitStructure.GPIO_Pin = ADC12_IN5_PIN;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
		GPIO_Init(ADC12_IN5_PORT, &GPIO_InitStructure); 
	}

	if( ADC_Channel == ADC_Channel_6 ) // A1 PA.6
	{
		GPIO_InitStructure.GPIO_Pin = ADC12_IN6_PIN;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
		GPIO_Init(ADC12_IN6_PORT, &GPIO_InitStructure); 
	}
	
	if( ADC_Channel == ADC_Channel_7 ) // A0 PA.7
	{
		GPIO_InitStructure.GPIO_Pin = ADC12_IN7_PIN;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
		GPIO_Init(ADC12_IN7_PORT, &GPIO_InitStructure); 
	}

	if( ADC_Channel == ADC_Channel_13 ) // for test
	{
		GPIO_InitStructure.GPIO_Pin = ADC12_IN13_PIN;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
		GPIO_Init(ADC12_IN13_PORT, &GPIO_InitStructure);
	}

}

/**
 * @brief To initialize ADC.
 * @param ADC_Type Use A0~A3 on a WIZnet module
 * @return None
 */
void adc_init(uint8_t ADC_Type) 
{
	ADC_InitTypeDef ADC_InitStructure;
	uint8_t ADC_Channel;

	/* System clocks configuration */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_ADC2, ENABLE);

	if(  ADC_Type == A0 )
	{
		ADC_Channel = ADC_Channel_7;
	}

	if(  ADC_Type == A1 )
	{
		ADC_Channel = ADC_Channel_6;
	}

	if(  ADC_Type == A2 )
	{
		ADC_Channel = ADC_Channel_5;
	}

	if(  ADC_Type == A3 )
	{
		ADC_Channel = ADC_Channel_4;
	}
	
	/* GPIO configuration */
	adc_gpio_init(ADC_Channel);
	
	/* ADC1 configuration */
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	ADC_Init(ADC2, &ADC_InitStructure);

	/* ADC1 regular channel configuration */ 
	if( ADC_Channel == ADC_Channel_4 )
	{
		ADC_RegularChannelConfig(ADC2, ADC_Channel_4, 1, ADC_SampleTime_55Cycles5);
	}

	if( ADC_Channel == ADC_Channel_5 )
	{
		ADC_RegularChannelConfig(ADC2, ADC_Channel_5, 1, ADC_SampleTime_55Cycles5);
	}

	if( ADC_Channel == ADC_Channel_6 )
	{
		ADC_RegularChannelConfig(ADC2, ADC_Channel_6, 1, ADC_SampleTime_55Cycles5);
	}

	if( ADC_Channel == ADC_Channel_7 )
	{
		ADC_RegularChannelConfig(ADC2, ADC_Channel_7, 1, ADC_SampleTime_55Cycles5);
	}
		
	if( ADC_Channel == ADC_Channel_13 ) // for test
	{
		ADC_RegularChannelConfig(ADC2, ADC_Channel_13, 1, ADC_SampleTime_55Cycles5); 
	}

	/* Enable ADC1 */
	ADC_Cmd(ADC2, ENABLE);

	/* Start ADC1 Software Conversion */ 
	ADC_SoftwareStartConvCmd(ADC2,ENABLE);
}

/**
 * @brief To initialize ADC by DMA method.
 * @param None
 * @return None
 */
void adc_dma_init(void) 
{
	GPIO_InitTypeDef GPIO_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	
	/* System clocks configuration */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_ADC2, ENABLE);

	/* GPIO configuration */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6; // for test
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* DMA channel1 configuration */
	DMA_DeInit(DMA1_Channel1);
	DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&ADC1ConvertedValue;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = 1;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);  

	/* Enable DMA channel1 */
	DMA_Cmd(DMA1_Channel1, ENABLE);

	/* ADC1 configuration */
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	ADC_Init(ADC1, &ADC_InitStructure);

	/* ADC1 regular channels configuration */ 
	ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 1, ADC_SampleTime_28Cycles5);

	/* Enable ADC1 DMA */
	ADC_DMACmd(ADC1, ENABLE);

	/* Enable ADC1 */
	ADC_Cmd(ADC1, ENABLE);

	/* Enable ADC1 reset calibration register */
	ADC_ResetCalibration(ADC1);
	/* Check the end of ADC1 reset calibration register */
	while(ADC_GetResetCalibrationStatus(ADC1));

	/* Start ADC1 calibration */
	ADC_StartCalibration(ADC1);
	/* Check the end of ADC1 calibration */
	while(ADC_GetCalibrationStatus(ADC1));

	/* Start ADC1 Software Conversion */ 
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);

}

void adc_dmamulti_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_ADC2, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_6 | GPIO_Pin_5 | GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	DMA_DeInit(DMA1_Channel1);
	DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)ADC_DualConvertedValueTab;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = 4;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);
	DMA_Cmd(DMA1_Channel1, ENABLE);

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = 4;
	ADC_Init(ADC1, &ADC_InitStructure);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_7, 1, ADC_SampleTime_28Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 2, ADC_SampleTime_28Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 3, ADC_SampleTime_28Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 4, ADC_SampleTime_28Cycles5);
	ADC_DMACmd(ADC1, ENABLE);

	ADC_Cmd(ADC1, ENABLE);

	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1));
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1));

	ADC_SoftwareStartConvCmd(ADC1, ENABLE);

}


