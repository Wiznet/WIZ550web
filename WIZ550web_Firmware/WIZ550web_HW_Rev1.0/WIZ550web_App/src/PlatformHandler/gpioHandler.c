/**
 * @file	gpioHandler.c
 * @brief	GPIO Handler Example
 * @version 1.0
 * @date	2014/07/15
 * @par Revision
 *			2014/07/15 - 1.0 Release
 * @author	
 * \n\n @par Copyright (C) 1998 - 2014 WIZnet. All rights reserved.
 */

#include "gpioHandler.h"
#include "stm32f10x.h"
#include "boardutil.h"
#include "userHandler.h"

extern IOStorage IOdata;

GPIO_TypeDef* IO_PORT[IOn] = {D0_GPIO_PORT, D1_GPIO_PORT, D2_GPIO_PORT, D3_GPIO_PORT,
							  D4_GPIO_PORT, D5_GPIO_PORT, D6_GPIO_PORT, D7_GPIO_PORT,
							  D8_GPIO_PORT, D9_GPIO_PORT, D10_GPIO_PORT, D11_GPIO_PORT,
							  D12_GPIO_PORT, D13_GPIO_PORT, D14_GPIO_PORT, D15_GPIO_PORT
							  };
const uint16_t IO_PIN[IOn] = {D0_PIN, D1_PIN, D2_PIN, D3_PIN,
							  D4_PIN, D5_PIN, D6_PIN, D7_PIN,
							  D8_PIN, D9_PIN, D10_PIN, D11_PIN,
							  D12_PIN, D13_PIN, D14_PIN, D15_PIN
							  };
const uint32_t IO_CLK[IOn] = {D0_GPIO_CLK, D1_GPIO_CLK, D2_GPIO_CLK, D3_GPIO_CLK,
							  D4_GPIO_CLK, D5_GPIO_CLK, D6_GPIO_CLK, D7_GPIO_CLK,
							  D8_GPIO_CLK, D9_GPIO_CLK, D10_GPIO_CLK, D11_GPIO_CLK,
							  D12_GPIO_CLK, D13_GPIO_CLK, D14_GPIO_CLK, D15_GPIO_CLK,
							  };

/**
  * @brief  Configures the GPIO
  * @param  None
  * @return None
  */
void GPIO_Configuration(void)
{

  GPIO_InitTypeDef  GPIO_InitStructure;

    // Only ADC, CS signals (W5500, Flash), W5500 Reset and Int of W5500 are configured.
    // Port A : ADC
    GPIO_InitStructure.GPIO_Pin = ADC_0 | ADC_1 | ADC_2 | ADC_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

	// Port B Output
    //GPIO_InitStructure.GPIO_Pin = W5500_RESET_PIN | CS_EX_Flash | W5500_CS_PIN ;
	GPIO_InitStructure.GPIO_Pin = W5500_RESET_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_SetBits(GPIOB, W5500_RESET_PIN);
    //GPIO_SetBits(GPIOB, CS_EX_Flash);
    //GPIO_SetBits(GPIOB, W5500_CS_PIN);

	// Port B input
    GPIO_InitStructure.GPIO_Pin = INT_W5500;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/**
  * @brief	Configures GPIO for D0~D15
  * @param	io Specifies the GPIO to be configured.
  *   This parameter can be one of following parameters:
  * 	@arg D0 @arg D1 @arg D2 @arg D3 @arg D4
  * 	@arg D5 @arg D6 @arg D7 @arg D8 @arg D9
  * 	@arg D10 @arg D11 @arg D12 @arg D13 @arg D14 @arg D15
  * @return None
  */
void IO_Init(IO_Type io)
{
	GPIO_InitTypeDef	GPIO_InitStructure;

  	/* Enable the GPIO_LED Clock */
  	RCC_APB2PeriphClockCmd(IO_CLK[io], ENABLE);

  	/* Configure the GPIO_LED pin */
  	GPIO_InitStructure.GPIO_Pin = IO_PIN[io];
  	if(IOdata.io[io] == Input)
  		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;//GPIO_Mode_IN_FLOATING;
  	else
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

  	GPIO_Init(IO_PORT[io], &GPIO_InitStructure);

}

/**
  * @brief	Turns selected GPIO On.
  * @param	io Specifies the GPIO to be set on.
  *   This parameter can be one of following parameters:
  * 	@arg D0 @arg D1 @arg D2 @arg D3 @arg D4
  * 	@arg D5 @arg D6 @arg D7 @arg D8 @arg D9
  * 	@arg D10 @arg D11 @arg D12 @arg D13 @arg D14 @arg D15
  * @return None
  */
void IO_On(IO_Type io)
{
	if(IOdata.io[io] == Output)
  		GPIO_SetBits(IO_PORT[io], IO_PIN[io]);
}

/**
  * @brief	Turns selected GPIO Off.
  * @param	io Specifies the GPIO to be set off.
  *   This parameter can be one of following parameters:
  * 	@arg D0 @arg D1 @arg D2 @arg D3 @arg D4
  * 	@arg D5 @arg D6 @arg D7 @arg D8 @arg D9
  * 	@arg D10 @arg D11 @arg D12 @arg D13 @arg D14 @arg D15
  * @return None
  */
void IO_Off(IO_Type io)
{
	if(IOdata.io[io] == Output)
		GPIO_ResetBits(IO_PORT[io], IO_PIN[io]);
}

/**
  * @brief	Toggles the selected GPIO.
  * @param	io Specifies the GPIO to be toggled.
  *   This parameter can be one of following parameters:
  * 	@arg D0 @arg D1 @arg D2 @arg D3 @arg D4
  * 	@arg D5 @arg D6 @arg D7 @arg D8 @arg D9
  * 	@arg D10 @arg D11 @arg D12 @arg D13 @arg D14 @arg D15
  * @return None
  */
void IO_Toggle(IO_Type io)
{
	if(IOdata.io[io] == Output)
  		IO_PORT[io]->ODR ^= IO_PIN[io];
}

/**
  * @brief	To get the status of GPIO.
  * @param	io Specifies the GPIO to get a status
  *   This parameter can be one of following parameters:
  * 	@arg D0 @arg D1 @arg D2 @arg D3 @arg D4
  * 	@arg D5 @arg D6 @arg D7 @arg D8 @arg D9
  * 	@arg D10 @arg D11 @arg D12 @arg D13 @arg D14 @arg D15
  * @return The status of GPIO
  */
uint8_t get_IO_Status(IO_Type io)
{
	if(IOdata.io[io] == Input)
		return GPIO_ReadInputDataBit(IO_PORT[io], IO_PIN[io]);
	else if (IOdata.io[io] == Output)
		return GPIO_ReadOutputDataBit(IO_PORT[io], IO_PIN[io]);
	else
		return RET_OK;
}

