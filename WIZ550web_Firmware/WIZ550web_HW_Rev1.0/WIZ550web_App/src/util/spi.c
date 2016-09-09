/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/
#include  <stdio.h>

#include "stm32f10x.h"
#include <stm32f10x_spi.h>
//#include "hw_config.h"
#include "spi.h"


/* ------------------------------------------------------------------------------------------------- */
/* BSP SPI */
/* ------------------------------------------------------------------------------------------------- */



/* ------------------------------------------------------------------------------------------------- */
/* extern USART */
/* ------------------------------------------------------------------------------------------------- */


// SPI speed setting function
// SpeedSet:
// SPI_SPEED_2 2 frequency (SPI 36M @ sys 72M)
// SPI_SPEED_4 4 frequency (SPI 18M @ sys 72M)
// SPI_SPEED_8 8 frequency (SPI 9M @ sys 72M)
// SPI_SPEED_16 16 frequency (SPI 4.5M @ sys 72M)
// SPI_SPEED_256 256 frequency (SPI 281.25K @ sys 72M)
void bsp_set_spi1_speed (u8 speed)
{
	SPI1->CR1 &= 0XFFC7; // Fsck = Fcpu/256
	switch (speed)
	{
		case SPI_SPEED_2: // Second division
			SPI1->CR1 |= 0<<3; // Fsck = Fpclk / 2 = 36Mhz
			break;
		case SPI_SPEED_4: // four-band
			SPI1-> CR1 |= 1<<3; // Fsck = Fpclk / 4 = 18Mhz
			break;
		case SPI_SPEED_8: // eighth of the frequency
			SPI1-> CR1 |= 2<<3; // Fsck = Fpclk / 8 = 9Mhz
			break;
		case SPI_SPEED_16: // sixteen frequency
			SPI1-> CR1 |= 3<<3; // Fsck = Fpclk/16 = 4.5Mhz
			break;
		case SPI_SPEED_256: // 256 frequency division
			SPI1-> CR1 |= 7<<3; // Fsck = Fpclk/16 = 281.25Khz
			break;
	}
	
	SPI1->CR1 |= 1<<6; // SPI devices enable
	
}

/*******************************************************************************
* Function Name: bsp_readwritebyte_spi1
* Description: SPI read and write a single byte (to return after sending the data read in this Newsletter)
* Input: u8 TxData the number to be sent
* Output: None
* Return: u8 RxData the number of received
*******************************************************************************/
u8 bsp_readwritebyte_spi1 (u8 tx_data)
{
	u8 retry=0;
	
	/* Loop while DR register in not emplty */
	while (SPI_I2S_GetFlagStatus (SPI1, SPI_I2S_FLAG_TXE) == RESET)
	{
		retry++;
		if(retry>400)
			return 0;
	}

	/* Send byte through the SPI1 peripheral */
	SPI_I2S_SendData (SPI1, tx_data);

	retry=0;

	/* Wait to receive a byte */
	while (SPI_I2S_GetFlagStatus (SPI1, SPI_I2S_FLAG_RXNE) == RESET)
	{
		retry++;
		
		if(retry>400)
			return 0;
	}		

	/* Return the byte read from the SPI bus */
	return SPI_I2S_ReceiveData (SPI1);
}


// SPI speed setting function
// SpeedSet:
// SPI_SPEED_2 2 frequency (SPI 36M @ sys 72M)
// SPI_SPEED_4 4 frequency (SPI 18M @ sys 72M)
// SPI_SPEED_8 8 frequency (SPI 9M @ sys 72M)
// SPI_SPEED_16 16 frequency (SPI 4.5M @ sys 72M)
// SPI_SPEED_256 256 frequency (SPI 281.25K @ sys 72M)
void bsp_set_spi2_speed (u8 speed)
{
	SPI2->CR1 &= 0XFFC7; // Fsck = Fcpu/256
	switch (speed)
	{
		case SPI_SPEED_2: // Second division
			SPI2->CR1 |= 0<<3; // Fsck = Fpclk / 2 = 36Mhz
			break;
		case SPI_SPEED_4: // four-band
			SPI2-> CR1 |= 1<<3; // Fsck = Fpclk / 4 = 18Mhz
			break;
		case SPI_SPEED_8: // eighth of the frequency
			SPI2-> CR1 |= 2<<3; // Fsck = Fpclk / 8 = 9Mhz
			break;
		case SPI_SPEED_16: // sixteen frequency
			SPI2-> CR1 |= 3<<3; // Fsck = Fpclk/16 = 4.5Mhz
			break;
		case SPI_SPEED_256: // 256 frequency division
			SPI2-> CR1 |= 7<<3; // Fsck = Fpclk/16 = 281.25Khz
			break;
	}
	
	SPI2->CR1 |= 1<<6; // SPI devices enable
	
}

void bsp_set_spi2_speed_mp3(u8 SpeedSet)
{
    SPI_InitTypeDef SPI_InitStructure ;
    
    SPI_InitStructure.SPI_Direction=SPI_Direction_2Lines_FullDuplex ;
    SPI_InitStructure.SPI_Mode=SPI_Mode_Master ;
    SPI_InitStructure.SPI_DataSize=SPI_DataSize_8b ;
    SPI_InitStructure.SPI_CPOL=SPI_CPOL_High ;
    SPI_InitStructure.SPI_CPHA=SPI_CPHA_2Edge ;
    SPI_InitStructure.SPI_NSS=SPI_NSS_Soft ;

    if(SpeedSet==SPI_SPEED_LOW)
    {
        SPI_InitStructure.SPI_BaudRatePrescaler=SPI_BaudRatePrescaler_16;
    }
    else 
    {
        SPI_InitStructure.SPI_BaudRatePrescaler=SPI_BaudRatePrescaler_4;
    }

	//moon.mp3: 4707774 Byte size buffer[512]     
	//speed: 392314 Byte/S£¬
	//Prescaler_128, 59592 Byte/S
	//Prescaler_64, 104617 Byte/S
	//Prescaler_32, 168134 Byte/S    162337 Byte/S
	//Prescaler_16, 261543 Byte/S    247777 Byte/S
	//Prescaler_8,  313851 Byte/S    336269 Byte/S
	//Prescaler_4,  392314 Byte/S    392314 Byte/S
	//Prescaler_2,  392314 Byte/S

    SPI_InitStructure.SPI_FirstBit=SPI_FirstBit_MSB ;
    SPI_InitStructure.SPI_CRCPolynomial=7 ;
    SPI_Init(SPI2,&SPI_InitStructure);
}

/*******************************************************************************
* Function Name: bsp_readwritebyte_spi2
* Description: SPI read and write a single byte (to return after sending the data read in this Newsletter)
* Input: u8 TxData the number to be sent
* Output: None
* Return: u8 RxData the number of received
*******************************************************************************/
u8 bsp_readwritebyte_spi2 (u8 tx_data)
{
	u16 retry=0;
	
	/* Loop while DR register in not emplty */
	while (SPI_I2S_GetFlagStatus (SPI2, SPI_I2S_FLAG_TXE) == RESET)
	{
		retry++;
		if(retry>400)
			return 0;
	}

	/* Send byte through the SPI2 peripheral */
	SPI_I2S_SendData (SPI2, tx_data);

	retry=0;

	/* Wait to receive a byte */
	while (SPI_I2S_GetFlagStatus (SPI2, SPI_I2S_FLAG_RXNE) == RESET)
	{
		retry++;
		
		if(retry>400)
			return 0;
	}		

	/* Return the byte read from the SPI bus */
	return SPI_I2S_ReceiveData (SPI2);
}


