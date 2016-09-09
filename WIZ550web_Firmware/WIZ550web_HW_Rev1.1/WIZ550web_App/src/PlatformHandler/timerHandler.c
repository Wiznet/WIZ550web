/**
 * @file	timerHandler.c
 * @brief	TIMER Handler Example
 * @version 1.0
 * @date	2014/07/15
 * @par Revision
 *			2014/07/15 - 1.0 Release
 * @author	
 * \n\n @par Copyright (C) 1998 - 2014 WIZnet. All rights reserved.
 */

#include "timerHandler.h"
#include "stm32f10x.h"
#include "boardutil.h"

#include "dhcp.h"
#include "httpServer.h" // for function httpServer_time_handler()

volatile uint16_t msec_cnt = 0;
volatile uint8_t sec_cnt = 0;

void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET )
	{
		// Also cleared the wrong interrupt flag in the ISR
		TIM_ClearFlag(TIM2, TIM_FLAG_Update);
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update); // Clear the interrupt flag

		//Todo
		msec_cnt++; // milli second
		if(msec_cnt >= 1000 - 1) // second
		{
			sec_cnt++;

			httpServer_time_handler();
			DHCP_time_handler();
			msec_cnt = 0;
		}

		if(sec_cnt >= 60 - 1)
		{
			sec_cnt = 0;
		}
	}
}

/**
  * @brief  Configures the Timer
  * @param  None
  * @return None
  */
void Timer_Configuration(void)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 1000;
  TIM_TimeBaseStructure.TIM_Prescaler = 0;
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

  /* Prescaler configuration */
  TIM_PrescalerConfig(TIM2, 71, TIM_PSCReloadMode_Immediate);

  /* TIM enable counter */
  TIM_Cmd(TIM2, ENABLE);

  /* TIM IT enable */
  TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

#ifdef ENABLE_TIMER_INT
  // Timer for delay

  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 1;
  TIM_TimeBaseStructure.TIM_Prescaler = 0;
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

  /* Prescaler configuration */
  TIM_PrescalerConfig(TIM3, 71, TIM_PSCReloadMode_Immediate);

  /* TIM enable counter */
  TIM_Cmd(TIM3, DISABLE);

  /* TIM IT enable */
  TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
#endif

}
