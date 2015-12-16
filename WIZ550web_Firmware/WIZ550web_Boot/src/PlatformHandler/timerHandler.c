
#include "stm32f10x.h"
#include "gpioHandler.h"
#include "uartHandler.h"
#include "timerHandler.h"
#include "tftp.h"

static uint32_t mill_sec = 0;

void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	
		mill_sec++;

		if((mill_sec % 100) == 0) {
			LED_Toggle(LED1);
			LED_Toggle(LED2);
		}

		if((mill_sec % 1000) == 0) {
			mill_sec = 0;
			tftp_timeout_handler();
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

	/* Enable the TIM2 global Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
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
}

void Delay(volatile uint32_t nCount)
{
  for(; nCount!= 0;nCount--);
}

void delay_us (const uint32_t usec)
{
	RCC_ClocksTypeDef  RCC_Clocks;

	/* Configure HCLK clock as SysTick clock source */
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);

	RCC_GetClocksFreq(&RCC_Clocks);

	// Set SysTick Reload(1us) register and Enable
	// usec * (RCC_Clocks.HCLK_Frequency / 1000000) < 0xFFFFFFUL  -- because of 24bit timer
	SysTick_Config(usec * (RCC_Clocks.HCLK_Frequency / 1000000));
	// 72/72000000 --> 1usec
	// 0.001msec = 1usec
	// 1Hz = 1sec, 10Hz = 100msec, 100Hz = 10msec, 1KHz = 1msec,
	// 10KHz = 0.1msec, 100Khz = 0.01msec, 1MHz = 1usec(0.001msec)
	// 1usec = 1MHz


	// SysTick Interrupt Disable
	SysTick->CTRL  &= ~SysTick_CTRL_TICKINT_Msk ;

	// Until Tick count is 0
	while (!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk));
}

void delay_ms (const uint32_t msec)
{
	delay_us(1000 * msec);
}
