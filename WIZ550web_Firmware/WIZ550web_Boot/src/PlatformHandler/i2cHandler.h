
#ifndef __I2CHANDLER_H__
#define __I2CHANDLER_H__

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

// I2C EERPOM Peri function Section
#define EPP_I2C				I2C1
#define EEP_Address			0xA0
#define EEP_MacAddresss		0xFA
#define EEP_I2C_RCC			RCC_APB1Periph_I2C1
#define EEP_GPIO_RCC		RCC_APB2Periph_GPIOB
#define EEP_PORT			GPIOB
#define EEP_SCL_Pin			GPIO_Pin_6
#define EEP_SDA_Pin			GPIO_Pin_7

extern u8 EEPROM_ADDRESS;
void I2C1Initialize(void);
void EEP_Read(uint8_t* pBuffer, uint16_t ReadAddr, uint16_t NumByteToRead);
void EEP_Write(uint8_t* pBuffer, uint16_t WriteAddr, uint16_t NumByteToWrite);
void I2C_EE_WaitEepromStandbyState(void);


// I2C EERPOM GPIO function Section
#define IIC1_SCL_PORT  	GPIOC
#define IIC1_SCL_PIN  	GPIO_Pin_6

#define IIC1_SDA_PORT  	GPIOC
#define IIC1_SDA_PIN  	GPIO_Pin_7

#define IIC1_SCL_HIGH() GPIO_WriteBit(IIC1_SCL_PORT,IIC1_SCL_PIN,Bit_SET)
#define IIC1_SCL_LOW() 	GPIO_WriteBit(IIC1_SCL_PORT,IIC1_SCL_PIN,Bit_RESET)

#define IIC1_SDA_HIGH() GPIO_WriteBit(IIC1_SDA_PORT,IIC1_SDA_PIN,Bit_SET)
#define IIC1_SDA_LOW() 	GPIO_WriteBit(IIC1_SDA_PORT,IIC1_SDA_PIN,Bit_RESET)

#define IIC1_READ_DATA 	GPIO_ReadInputDataBit(IIC1_SDA_PORT,IIC1_SDA_PIN)

void IIC1_Init(void);
void IIC1_Start(void);
void IIC1_Stop(void);
u8 IIC1_Wait_Ack(void);
void IIC1_Ack(void);
void IIC1_NAck(void);
void IIC1_Send_Byte(u8 txd);
u8 IIC1_Read_Byte(unsigned char ack);

#endif
