/**
 * @file	extiHandler.c
 * @brief	EXTI Handler Example
 * @version 1.0
 * @date	2014/07/15
 * @par Revision
 *			2014/07/15 - 1.0 Release
 * @author	
 * \n\n @par Copyright (C) 1998 - 2014 WIZnet. All rights reserved.
 */

#include "stm32f10x.h"
#include "boardutil.h"
#include "extiHandler.h"

#if defined(FACTORY_FW)
void EXTI15_10_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line12) != RESET)
    {
        EXTI_ClearITPendingBit(EXTI_Line12);
#if defined(FACTORY_FW)
#if defined(FACTORY_FW_FLASH)
		if(check_factory_flag() == 1)
#else
		if(g_factoryfw_flag == 1)
#endif
		{
			LED_Toggle(LED1);
			if(get_IO_Status(D8) == On)
			{
				printf("########## SW1 Pressed OK.\r\n");
			}
			else
			{
				printf("########## SW1 Released OK.\r\n");
			}
		}
#endif
    }
}

void EXTI2_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line2) != RESET)
    {
        EXTI_ClearITPendingBit(EXTI_Line2);
#if defined(FACTORY_FW)
#if defined(FACTORY_FW_FLASH)
		if(check_factory_flag() == 1)
#else
		if(g_factoryfw_flag == 1)
#endif
		{
			LED_Toggle(LED1);
			if(get_IO_Status(D9) == On)
			{
				printf("########## SW2 Pressed OK.\r\n");
			}
			else
			{
				printf("########## SW2 Released OK.\r\n");
			}
		}
#endif
    }
}

void EXTI3_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line3) != RESET)
    {
        EXTI_ClearITPendingBit(EXTI_Line3);
#if defined(FACTORY_FW)
#if defined(FACTORY_FW_FLASH)
		if(check_factory_flag() == 1)
#else
		if(g_factoryfw_flag == 1)
#endif
		{
			LED_Toggle(LED2);
			if(get_IO_Status(D10) == On)
			{
				printf("########## SW3 Pressed OK.\r\n");
			}
			else
			{
				printf("########## SW3 Released OK.\r\n");
			}
		}
#endif
    }
}

void EXTI4_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line4) != RESET)
    {
        EXTI_ClearITPendingBit(EXTI_Line4);
#if defined(FACTORY_FW)
#if defined(FACTORY_FW_FLASH)
		if(check_factory_flag() == 1)
#else
		if(g_factoryfw_flag == 1)
#endif
		{
			LED_Toggle(LED2);
			if(get_IO_Status(D11) == On)
			{
				printf("########## SW4 Pressed OK.\r\n");
			}
			else
			{
				printf("########## SW4 Released OK.\r\n");
				//printf("########## Please confirm the test result.\r\n");
			}
		}
#endif
    }
}

/**
  * @brief  Configures EXTI
  * @param  None
  * @return None
  */
void EXTI_Configuration(void)
{
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    // Define GPIO Init structure
    GPIO_InitTypeDef GPIO_InitStructure;

#if 0
    /* Configure the GPIOC ports for input */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    /* Configure the GPIOC ports for input */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    /* Configure the GPIOC ports for input */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

#endif

    /* Connect EXTI */
    // External Interrupt configuration register1 (AFIO_EXTICR2)
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource12);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource2);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource3);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource4);

    /* Configure EXTI12  to generate an interrupt on falling edge */
    EXTI_InitStructure.EXTI_Line = EXTI_Line12 | EXTI_Line2 | EXTI_Line3 | EXTI_Line4;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;//EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    // 2 bit for preemption priority, 2 bits for sub priority
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* Clear EXTI Line Pending Bit */
    EXTI_ClearITPendingBit(EXTI_Line12);
    /* Enable the Key EXTI line Interrupt */
    NVIC_ClearPendingIRQ(EXTI15_10_IRQn);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* Clear EXTI Line Pending Bit */
    EXTI_ClearITPendingBit(EXTI_Line2);
    /* Enable the Key EXTI line Interrupt */
    NVIC_ClearPendingIRQ(EXTI2_IRQn);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* Clear EXTI Line Pending Bit */
    EXTI_ClearITPendingBit(EXTI_Line3);
    /* Enable the Key EXTI line Interrupt */
    NVIC_ClearPendingIRQ(EXTI3_IRQn);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* Clear EXTI Line Pending Bit */
    EXTI_ClearITPendingBit(EXTI_Line4);
    /* Enable the Key EXTI line Interrupt */
    NVIC_ClearPendingIRQ(EXTI4_IRQn);

}
#endif
