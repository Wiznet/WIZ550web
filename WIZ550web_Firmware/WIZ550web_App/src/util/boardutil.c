
#include "stm32f10x_conf.h"
#include "stm32f10x.h"

#include <stdio.h>
#include <stdarg.h>
#include "boardutil.h"
#include "types.h"
#include "userHandler.h"

#if defined(FACTORY_FW)
#include "adcHandler.h"
#include "extiHandler.h"
#include "userHandler.h"
#include "gpioHandler.h"
#include "uartHandler.h"
#endif

//#define _WEB_DEBUG_

#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

#ifdef CHINA_BOARD
GPIO_TypeDef* GPIO_PORT[LEDn] = {LED1_GPIO_PORT, LED2_GPIO_PORT, LED3_GPIO_PORT, LED4_GPIO_PORT};
const uint16_t GPIO_PIN[LEDn] = {LED1_PIN, LED2_PIN, LED3_PIN, LED4_PIN};
const uint32_t GPIO_CLK[LEDn] = {LED1_GPIO_CLK, LED2_GPIO_CLK, LED3_GPIO_CLK, LED4_GPIO_CLK};
#else
GPIO_TypeDef* GPIO_PORT[LEDn] = {LED1_GPIO_PORT, LED2_GPIO_PORT};
const uint16_t GPIO_PIN[LEDn] = {LED1_PIN, LED2_PIN};
const uint32_t GPIO_CLK[LEDn] = {LED1_GPIO_CLK, LED2_GPIO_CLK};
#endif

#if defined(FACTORY_FW)
int teststep = 0;
int g_factoryfw_flag = 0;
#endif

#if defined(F_SPI_FLASH)
int g_spiflash_flag = 0;
#endif

/**
  * @brief  Configures the NVIC interrupts.
  * @param  None
  * @retval None
  */
void NVIC_Configuration(void)
{
  //NVIC_InitTypeDef NVIC_InitStructure;

#ifdef  VECT_TAB_RAM
  /* Set the Vector Table base location at 0x20000000 */
  NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);

#else  /* VECT_TAB_FLASH  */

  /* Configure the preemption priority and subpriority:
     - 1 bits for pre-emption priority: possible value are 0 or 1
     - 3 bits for subpriority: possible value are 0..7
     - Lower values gives higher priority
  */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);


#if 0
 /* Enable the WAKEUP_BUTTON_EXTI_IRQn Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = WAKEUP_BUTTON_EXTI_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = PreemptionPriorityValue;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Enable the KEY_BUTTON_EXTI_IRQn Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = KEY_BUTTON_EXTI_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
#endif


#if 0
  /* Enable the USART1 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Enable the USART2 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
#endif


#if  0
  /* Enable the TIM2 global Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);


  /* Enable the TIM3 global Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
#endif

  /* Set the Vector Table base location at 0x08000000 */
  //NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);

  /* Set the Vector Table base location at 0x08002000 -> USE AIP*/
//  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x2000);
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x6000);
	__enable_irq();
#endif
}

// IWDG: STM32 Independent Watchdog Initialization
void IWDG_Configureation(void)
{
	//RCC_LSICmd(ENABLE); //open LSI
	//while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET);
	if(RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != RESET)
	{
		RCC_ClearFlag();
	}

	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	IWDG_SetPrescaler(IWDG_Prescaler_128); // 40Khz / 128 = 0.31KHz; 1 / 0.31KHz = 3.22ms
	//IWDG_SetReload(1250); // 1s, max 0xfff
	IWDG_SetReload(0xfff); // 4095 * 3.22ms = 13185.9ms = 13 seconds; it means if IWDG was not reloaded, MCU will reset!

	//IWDG_ReloadCounter();
	IWDG_Enable();
}

/**
  * @brief  Configures LED GPIO.
  * @param  Led: Specifies the Led to be configured.
  *   This parameter can be one of following parameters:
  *     @arg LED1
  *     @arg LED2
  * @retval None
  */
void LED_Init(Led_TypeDef Led)
{
  GPIO_InitTypeDef  GPIO_InitStructure;

  /* Enable the GPIO_LED Clock */
  RCC_APB2PeriphClockCmd(GPIO_CLK[Led], ENABLE);

  /* Configure the GPIO_LED pin */
  GPIO_InitStructure.GPIO_Pin = GPIO_PIN[Led];
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

  GPIO_Init(GPIO_PORT[Led], &GPIO_InitStructure);

}

/**
  * @brief  Turns selected LED On.
  * @param  Led: Specifies the Led to be set on.
  *   This parameter can be one of following parameters:
  *     @arg LED1
  *     @arg LED2
  * @retval None
  */
void LED_On(Led_TypeDef Led)
{
  //GPIO_PORT[Led]->BSRR = GPIO_PIN[Led];
  GPIO_ResetBits(GPIO_PORT[Led], GPIO_PIN[Led]);
}

/**
  * @brief  Turns selected LED Off.
  * @param  Led: Specifies the Led to be set off.
  *   This parameter can be one of following parameters:
  *     @arg LED1
  *     @arg LED2
  * @retval None
  */
void LED_Off(Led_TypeDef Led)
{
  //GPIO_PORT[Led]->BRR = GPIO_PIN[Led];
  GPIO_SetBits(GPIO_PORT[Led], GPIO_PIN[Led]);
}

/**
  * @brief  Toggles the selected LED.
  * @param  Led: Specifies the Led to be toggled.
  *   This parameter can be one of following parameters:
  *     @arg LED1
  *     @arg LED2
  * @retval None
  */
void LED_Toggle(Led_TypeDef Led)
{
  GPIO_PORT[Led]->ODR ^= GPIO_PIN[Led];
}

uint8_t get_LED_Status(Led_TypeDef Led)
{
	return GPIO_ReadOutputDataBit(GPIO_PORT[Led], GPIO_PIN[Led]);
}

/**
  * @brief  custom printf function
  * @param
  * @return None
  */

void myprintf(char *fmt, ...)
{
	va_list arg_ptr;
	char etxt[128]; // buffer size
	uint8_t i;

	va_start(arg_ptr, fmt);
	vsprintf(etxt, fmt, arg_ptr);
	va_end(arg_ptr);

	for (i = 0; etxt[i] != '\0'; i++) {
		USART_SendData(USART2, (u8) etxt[i]);
		while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET); // Tx buffer empty
	}
}


/**
  * @brief  Inserts a delay time.
  * @param  nCount: specifies the delay time length.
  * @return None
  */
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

#if defined(FACTORY_FW)
#define		FACTORY_TEST_STR		"TEST"
#define		FACTORY_CHECK_STR		"FTContinue"

extern IOStorage IOdata;
extern int teststep;

int check_factory_flag (void)
{
	int ret = 0;

	if(IOdata.factory_flag[0] == 0xFF && IOdata.factory_flag[1] == 0xFF)
	{
		ret = 0;
	}
	else if(IOdata.factory_flag[0] == 0xA5 && IOdata.factory_flag[1] == 0xA5)
	{
		ret = 1;
	}
	else
	{
		ret = 2;
	}

	return ret;
}

void check_factory_uart1 (void)
{
	uint8_t * buf_ptr;

	buf_ptr = getUSART1buf();

	if(strncmp(buf_ptr, FACTORY_CHECK_STR, sizeof(FACTORY_CHECK_STR)) == 0)
	{
		// Serial data buffer clear
		USART1_flush();

		factory_test_2nd();

#if defined(FACTORY_FW_FLASH)
		release_factory_flag();
#else
		//g_factoryfw_flag = 0;
#endif

		//delay_ms(200);
		//NVIC_SystemReset();
	}
}

void check_RS422 (uint8_t * buf)
{
#if defined(FACTORY_FW_FLASH)
	if(check_factory_flag() == 1)
#else
	if(g_factoryfw_flag == 1)
#endif
	{
		if(strncmp(buf, FACTORY_TEST_STR, sizeof(FACTORY_TEST_STR)) == 0)
		{
			printf("########## RS422 RX:%s OK.\r\n", buf);
		}
		else
		{
			printf("########## RS422 RX:%s Fail.\r\n", buf);
		}
	}
}

void save_factory_flag (void)
{
	IOdata.factory_flag[0] = 0xA5;
	IOdata.factory_flag[1] = 0xA5;

	write_IOstorage(&IOdata, sizeof(IOdata));
}

void release_factory_flag (void)
{
#if defined(FACTORY_FW_ONCE)
	IOdata.factory_flag[0] = 0xFF;
	IOdata.factory_flag[1] = 0xFF;
#endif
	IOdata.io_statuscode[0] = 0xFF;
	IOdata.io_statuscode[1] = 0xFF;

	write_IOstorage(&IOdata, sizeof(IOdata));
	//IO_status_init();
}

void factory_test_1st (void)
{
	int i;
	//uint16 adc_val = 0;
	int fail_count;

	// check A0~A3
	if (teststep == 0)
	{
		fail_count = 0;
#if 0
		for(i = 0; i < 4; i++)
		{
			adc_val = ADC_DualConvertedValueTab[i];
			if (adc_val > 100) //TODO
			{
				delay_ms(1);
		    	//printf("########## A%d[%d] OK.\r\n", i, adc_val);
			}
			else
			{
				fail_count += 1;
		    	printf("########## A%d[%d] Fail.\r\n", i, adc_val);
			}
		}
#endif
		if(fail_count == 0)
		{
			printf("########## A0 = %d || A1 = %d || A2 = %d || A3 = %d\r\n", ADC_DualConvertedValueTab[0]
			                                                                , ADC_DualConvertedValueTab[1]
			                                                                , ADC_DualConvertedValueTab[2]
			                                                                , ADC_DualConvertedValueTab[3]);
		}

		teststep = 1;
	}

	// check RS422
	if (teststep == 1)
	{
		printf("########## RS422 TX:TEST\r\n");
		UART2_flush();
		UART_write(FACTORY_TEST_STR, sizeof(FACTORY_TEST_STR));
		UART_write("\r", 1);
		teststep = 2;
	}

	// check SW1~SW3
	if (teststep == 2)
	{
		Delay(10000000);
		EXTI_Configuration();
		USART1_flush();
		teststep = 3;
	}

#if 0
		// check VCC, Temperature
		if (teststep == 5)
		{
			adc_val = ADC_DualConvertedValueTab[0];

			if (adc_val > 4000)
			{
				printf("########## VCC[%d] OK.\r\n", adc_val);

				adc_val = ADC_DualConvertedValueTab[1];
				if (adc_val > 100) //TODO
					printf("########## Temperature[%d] OK.\r\n", adc_val);
				else
					printf("########## Temperature[%d] Fail.\r\n", adc_val);
	    		teststep = 6;
			}
		}
#endif

	//check_factory_uart1();
}

void factory_test_2nd (void)
{
	int i;
	//uint16 adc_val = 0;
	int fail_count;
    EXTI_InitTypeDef EXTI_InitStructure;

    EXTI_InitStructure.EXTI_Line = EXTI_Line12 | EXTI_Line2 | EXTI_Line3 | EXTI_Line4;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;//EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = DISABLE;
    EXTI_Init(&EXTI_InitStructure);

	// check D0~D7(output/off) D8~15 input/off
	if (teststep == 3)
	{
		//delay_ms(100);
		fail_count = 0;
		for(i = 0; i < IOn; i++)
		{
			if (get_IO_Status(i) == Off)
			{
				if(i < 8)
				{
					delay_ms(1);
					//printf("########## D%d output/off OK.\r\n", i);
				}
				else
				{
					delay_ms(1);
					//printf("########## D%d input/off OK.\r\n", i);
				}
			}
			else
			{
				if(i < 8)
				{
					fail_count += 1;
					printf("########## D%d output/off Fail.\r\n", i);
				}
				else
				{
					printf("########## D%d input/off Fail.\r\n", i);
					fail_count += 1;
				}
			}
		}

		if(fail_count == 0)
		{
			printf("########## D0~D7[output/off] D8~D15[input/off] OK.\r\n");
		}

		teststep = 4;
	}

	// check D0~D7(output/on) D8~15 input/on
	if (teststep == 4)
	{
		fail_count = 0;
		for(i = 0; i < 8/*IOn*/; i++)
		{
			IOdata.ios[i] = On;
		}

	   	for(i = 0; i < 8/*IOn*/; i++)
	   	{
	   		if(IOdata.io[i] == Output)
	   		{
	   			if(IOdata.ios[i] == Off)	IO_Off(i);
	   			else						IO_On(i);
	   		}
	   	}

		for(i = 0; i < IOn; i++)
		{
			if (get_IO_Status(i) == On)
			{
				if(i < 8)
				{
					delay_ms(1);
					//printf("########## D%d output/on OK.\r\n", i);
				}
				else
				{
					delay_ms(1);
					//printf("########## D%d input/on OK.\r\n", i);
				}
			}
			else
			{
				if(i < 8)
				{
					fail_count += 1;
					printf("########## D%d output/on Fail.\r\n", i);
				}
				else
				{
					fail_count += 1;
					printf("########## D%d input/on Fail.\r\n", i);
				}
			}
		}

		if(fail_count == 0)
		{
			printf("########## D0~D7[output/on] D8~D15[input/on] OK.\r\n");
		}

		teststep = 5;
	}

	// check A0~A3
	if (teststep == 5)
	{
		fail_count = 0;
#if 0
		for(i = 0; i < 4; i++)
		{
			adc_val = ADC_DualConvertedValueTab[i];
			if (adc_val > 100) //TODO
			{
				delay_ms(1);
		    	//printf("########## A%d[%d] OK.\r\n", i, adc_val);
			}
			else
			{
				fail_count += 1;
		    	printf("########## A%d[%d] Fail.\r\n", i, adc_val);
			}
		}
#endif
		if(fail_count == 0)
		{
			printf("########## A0 = %d || A1 = %d || A2 = %d || A3 = %d\r\n", ADC_DualConvertedValueTab[0]
			                                                                , ADC_DualConvertedValueTab[1]
			                                                                , ADC_DualConvertedValueTab[2]
			                                                                , ADC_DualConvertedValueTab[3]);
		}

		teststep = 6;
	}

	printf("########## Test Completed.\r\n");
	//check_factory_uart1();
}

void factory_run(void)
{
	if ((get_IO_Status(D10) == On) && (get_IO_Status(D11) == On) && (g_factoryfw_flag == 0))
	{
		printf("\r\n########## Factory Test is started.\r\n");
		g_factoryfw_flag = 1;

#if defined(FACTORY_FW_FLASH)
		save_factory_flag();

		if (check_factory_flag() == 1)
		{
			factory_test_1st();
		}
#else
		factory_test_1st();
#endif
	}

}

#endif

#if defined(F_SPI_FLASH)
extern IOStorage IOdata;
int check_spiflash_flag(void)
{
	int ret = 0;

	read_IOstorage(&IOdata, sizeof(IOdata));

	if(IOdata.spiflash_flag[0] == 0xAE && IOdata.spiflash_flag[1] == 0xAE)
	{
		ret = 0;
	}
	else
	{
		ret = 1;
	}

	return ret;
}

void save_spiflash_flag(void)
{
	IOdata.spiflash_flag[0] = 0xAE;
	IOdata.spiflash_flag[1] = 0xAE;

	write_IOstorage(&IOdata, sizeof(IOdata));
}

void release_spiflash_flag(void)
{
	IOdata.spiflash_flag[0] = 0xFF;
	IOdata.spiflash_flag[1] = 0xFF;

	write_IOstorage(&IOdata, sizeof(IOdata));
}

void sflash_run(void)
{
	if ((get_IO_Status(D8) == On) && (get_IO_Status(D9) == On) && (g_spiflash_flag == 0))
	{
		printf("\r\n########## SW1 and SW2 were pressed.\r\n");
		printf("########## Data flash flag was cleared.\r\n");
		printf("########## Please reset a target.\r\n");
		g_spiflash_flag = 1;
		release_spiflash_flag();
	}
}

#endif

void kick_watchdog (void)
{
	// Depend on system.
	IWDG_ReloadCounter(); // Feed IWDG
}
