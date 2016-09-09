
#include "stm32f10x.h"
#include "eepromHandler.h"
#include "i2cHandler.h"
#include "timerHandler.h"

void bsp_24aaxx_gpio_init()
{
	GPIO_InitTypeDef GPIO_InitStructure;	
	
	//SCL
	GPIO_InitStructure.GPIO_Pin = IIC1_SCL_PIN;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
	GPIO_Init(IIC1_SCL_PORT, &GPIO_InitStructure);                                 

	//SDA
	GPIO_InitStructure.GPIO_Pin = IIC1_SDA_PIN;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_Init(IIC1_SDA_PORT, &GPIO_InitStructure); 
}

void EE24AAXX_Write_Delay(void)
{
	u16 i;
	for(i=0;i<45000;i++);
}

void EE24AAXX_Init(void)
{
	bsp_24aaxx_gpio_init();
}

u8 EE24AAXX_ReadOneByte(u16 ReadAddr)
{
	u8 temp = 0;

	IIC1_Start();
	if(EE_TYPE>EE24AA16)
	{
		IIC1_Send_Byte(0xA0);
		IIC1_Wait_Ack();
		IIC1_Send_Byte((uint8_t)((ReadAddr & 0xFF00) >> 8));
		IIC1_Wait_Ack();
		IIC1_Send_Byte((uint8_t)(ReadAddr & 0x00FF));
		IIC1_Wait_Ack();
	}
	else
	{
		IIC1_Send_Byte((uint8_t)(0xA0+((ReadAddr/EEPROM_BLOCK_SIZE)<<1)));
		IIC1_Wait_Ack();
		IIC1_Send_Byte((uint8_t)(ReadAddr%EEPROM_BLOCK_SIZE));
		IIC1_Wait_Ack();
	}
	IIC1_Start();
	if(EE_TYPE>EE24AA16)
	{
		IIC1_Send_Byte(0xA1);
	}
	else
	{
		IIC1_Send_Byte((uint8_t)(0xA1+((ReadAddr/EEPROM_BLOCK_SIZE)<<1)));
	}
	IIC1_Wait_Ack();
	temp = IIC1_Read_Byte(0);
	IIC1_Stop();

	return temp;
}

void EE24AAXX_WriteOneByte(u16 WriteAddr,u8 DataToWrite)
{
	IIC1_Start();
	if(EE_TYPE>EE24AA16)
	{
		IIC1_Send_Byte(0xA0);
		IIC1_Wait_Ack();
		IIC1_Send_Byte((uint8_t)((WriteAddr & 0xFF00) >> 8));
		IIC1_Wait_Ack(); 
		IIC1_Send_Byte((uint8_t)(WriteAddr & 0x00FF));
		IIC1_Wait_Ack();
	}
	else
	{
		IIC1_Send_Byte((uint8_t)(0xa0 + ((WriteAddr/EEPROM_BLOCK_SIZE<<1))));
		IIC1_Wait_Ack();
		IIC1_Send_Byte((uint8_t)(WriteAddr%EEPROM_BLOCK_SIZE));
		IIC1_Wait_Ack();
	}
	IIC1_Send_Byte((uint8_t)DataToWrite);
	IIC1_Wait_Ack();
	IIC1_Stop();
	EE24AAXX_Write_Delay();
}

void EE24AAXX_WriteLenByte(u16 WriteAddr,u32 DataToWrite,u8 Len)
{
	u8 t;
	for(t=0;t<Len;t++)
	{
		EE24AAXX_WriteOneByte(WriteAddr+t,(DataToWrite>>(8*t))&0xff);
	}
}

u32 EE24AAXX_ReadLenByte(u16 ReadAddr,u8 Len)
{
	u8 t;
	u32 temp = 0;

	for(t=0;t<Len;t++)
	{
		temp<<=8;
		temp+=EE24AAXX_ReadOneByte(ReadAddr+Len-t-1);
	}

	return temp;
}

void EE24AAXX_Read(u16 ReadAddr,u8 *pBuffer,u16 NumToRead)
{
	while(NumToRead)
	{
		*pBuffer++=EE24AAXX_ReadOneByte(ReadAddr++);
		NumToRead--;
	}
}

void EE24AAXX_Write(u16 WriteAddr,u8 *pBuffer,u16 NumToWrite)
{
	while(NumToWrite--)
	{
		EE24AAXX_WriteOneByte(WriteAddr,*pBuffer);
		WriteAddr++;
		pBuffer++;
	}
}

void EE24AAXX_WritePage(u16 WriteAddr,u8 *pBuffer,u8 len)
{
	int i;

	if(len>EEPROM_PAGE_SIZE)
		len = EEPROM_PAGE_SIZE;

	IIC1_Start();
	if(EE_TYPE>EE24AA16)
	{
		IIC1_Send_Byte(0xA0);
		IIC1_Wait_Ack();
		IIC1_Send_Byte((uint8_t)((WriteAddr & 0xFF00) >> 8));
		IIC1_Wait_Ack();
		IIC1_Send_Byte((uint8_t)(WriteAddr & 0x00FF));
		IIC1_Wait_Ack();
	}
	else
	{
		IIC1_Send_Byte((uint8_t)(0xa0 + ((WriteAddr/EEPROM_BLOCK_SIZE<<1))));
		IIC1_Wait_Ack();
		IIC1_Send_Byte((uint8_t)(WriteAddr%EEPROM_BLOCK_SIZE));
		IIC1_Wait_Ack();
	}

	for(i=0; i<len; i++)
	{
		IIC1_Send_Byte((uint8_t)pBuffer[i]);
		IIC1_Wait_Ack();
	}
	IIC1_Stop();
}
