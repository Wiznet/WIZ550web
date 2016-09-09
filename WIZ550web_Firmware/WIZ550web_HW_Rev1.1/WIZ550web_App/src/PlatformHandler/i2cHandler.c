
/* Includes ------------------------------------------------------------------*/
#include "i2cHandler.h"
#include "eepromHandler.h"

void I2C1Initialize(void)
{
	/*i2c clock enable*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE);
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO,ENABLE);

	/*i2c gpio initialize*/
	GPIO_InitTypeDef	GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin =  EEP_SCL_Pin | EEP_SDA_Pin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_Init(EEP_PORT, &GPIO_InitStructure);

	/*i2c initialize*/
	I2C_InitTypeDef  I2C_InitStructure;

	/* I2C configuration */
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_OwnAddress1 = EEP_Address;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_ClockSpeed = 100000;
	I2C_Init(EPP_I2C, &I2C_InitStructure);

	I2C_Cmd(EPP_I2C, ENABLE);
}

void EEP_Read(uint8_t* pBuffer, uint16_t ReadAddr, uint16_t NumByteToRead)
{
	if(EE_TYPE>EE24AA16)
	/* While the bus is busy */
	while(I2C_GetFlagStatus(EPP_I2C, I2C_FLAG_BUSY));

    /* Send START condition */
    I2C_GenerateSTART(EPP_I2C, ENABLE);

    /* Test on EV5 and clear it */
    while(!I2C_CheckEvent(EPP_I2C, I2C_EVENT_MASTER_MODE_SELECT));

	if(EE_TYPE>EE24AA16)
	{
		/* Send EEPROM address for write */
		I2C_Send7bitAddress(EPP_I2C, EEP_Address, I2C_Direction_Transmitter);
	}
	else
	{
		/* Send EEPROM address for write */
		I2C_Send7bitAddress(EPP_I2C, (uint8_t)(EEP_Address+((ReadAddr/EEPROM_BLOCK_SIZE)<<1)), I2C_Direction_Transmitter);
	}

    /* Test on EV6 and clear it */
    while(!I2C_CheckEvent(EPP_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

	if(EE_TYPE>EE24AA16)
	{
		/* Send the EEPROM's internal address to read from: MSB of the address first*/
		I2C_SendData(EPP_I2C, (uint8_t)((ReadAddr & 0xFF00)>>8));

		/* Test on EV8 and clear it */
		while(!I2C_CheckEvent(EPP_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	}

    /* Send the EEPROM's internal address to read from: LSB of the address */
    I2C_SendData(EPP_I2C, (uint8_t)(ReadAddr & 0x00FF));

    /* Test on EV8 and clear it */
    while(!I2C_CheckEvent(EPP_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    /* Send STRAT condition a second time */
    I2C_GenerateSTART(EPP_I2C, ENABLE);

    /* Test on EV5 and clear it */
    while(!I2C_CheckEvent(EPP_I2C, I2C_EVENT_MASTER_MODE_SELECT));

	if(EE_TYPE>EE24AA16)
	{
		/* Send EEPROM address for read */
		I2C_Send7bitAddress(EPP_I2C, EEP_Address, I2C_Direction_Receiver);
	}
	else
	{
		/* Send EEPROM address for read */
		I2C_Send7bitAddress(EPP_I2C, (uint8_t)(EEP_Address+((ReadAddr/EEPROM_BLOCK_SIZE)<<1)), I2C_Direction_Receiver);
	}

    /* Test on EV6 and clear it */
    while(!I2C_CheckEvent(EPP_I2C, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));

    /* While there is data to be read */
    while(NumByteToRead)
    {
        if(NumByteToRead == 1)
        {
            /* Disable Acknowledgement */
            I2C_AcknowledgeConfig(EPP_I2C, DISABLE);

            /* Send STOP Condition */
            I2C_GenerateSTOP(EPP_I2C, ENABLE);
        }

        /* Test on EV7 and clear it */
        if(I2C_CheckEvent(EPP_I2C, I2C_EVENT_MASTER_BYTE_RECEIVED))
        {
            /* Read a byte from the EEPROM */
            *pBuffer = I2C_ReceiveData(EPP_I2C);

            /* Point to the next location where the byte read will be saved */
            pBuffer++;

            /* Decrement the read bytes counter */
            NumByteToRead--;
        }
    }

    /* Enable Acknowledgement to be ready for another reception */
    I2C_AcknowledgeConfig(EPP_I2C, ENABLE);
}

void EEP_Write(uint8_t* pBuffer, uint16_t WriteAddr, uint16_t NumByteToWrite)
{
	/* While the bus is busy */
	while(I2C_GetFlagStatus(EPP_I2C, I2C_FLAG_BUSY));

	/* Send START condition */
	I2C_GenerateSTART(EPP_I2C, ENABLE);

	/* Test on EV5 and clear it */
	while(!I2C_CheckEvent(EPP_I2C, I2C_EVENT_MASTER_MODE_SELECT));

	// Control Byte (Slave Select bit)
	if(EE_TYPE>EE24AA16)
	{
		/* Send EEPROM address for write */
		I2C_Send7bitAddress(EPP_I2C, EEP_Address, I2C_Direction_Transmitter);
	}
	else
	{
		/* Send EEPROM address for write */
		I2C_Send7bitAddress(EPP_I2C, (uint8_t)(EEP_Address+((WriteAddr/EEPROM_BLOCK_SIZE)<<1)), I2C_Direction_Transmitter);
	}

	/* Test on EV6 and clear it */
	while(!I2C_CheckEvent(EPP_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

	// Address High byte
	if(EE_TYPE>EE24AA16)
	{
		/* Send the EEPROM's internal address to write from: MSB of the address first */
		I2C_SendData(EPP_I2C, (uint8_t)((WriteAddr & 0xFF00) >> 8));

		/* Test on EV8 and clear it */
		while(!I2C_CheckEvent(EPP_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	}

	// Address Low Byte
	/* Send the EEPROM's internal address to write from: LSB of the address */
	I2C_SendData(EPP_I2C, (uint8_t)(WriteAddr & 0x00FF));

	/* Test on EV8 and clear it */
	while(!I2C_CheckEvent(EPP_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	//Data Trans !
	while(NumByteToWrite--)
	{
		/* Send the current byte */
		I2C_SendData(EPP_I2C, *pBuffer);

		/* Point to the next byte to be written */
		pBuffer++;

		/* Test on EV8 and clear it */
		while (!I2C_CheckEvent(EPP_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	}

	/* Send STOP condition */
	I2C_GenerateSTOP(EPP_I2C, ENABLE);
}



void IIC_delay_us(u16 num)
{
	u8 i,j;

	for(i=0;i<num;i++)
		for(j=0;j<20;j++);
}

void IIC1_Start(void)
{
	IIC1_SDA_HIGH();
	IIC1_SCL_HIGH();
	IIC_delay_us(4);
	IIC1_SDA_LOW();
	IIC_delay_us(4);
	IIC1_SCL_LOW();
}

void IIC1_Stop(void)
{
	IIC1_SCL_LOW();
	IIC1_SDA_LOW();
	IIC_delay_us(4);
	IIC1_SCL_HIGH();
	IIC1_SDA_HIGH();
	IIC_delay_us(4);
}

u8 IIC1_Wait_Ack(void)
{
	u16 ucErrTime=0;

	IIC1_SDA_HIGH();
	IIC_delay_us(1);
	IIC1_SCL_HIGH();
	IIC_delay_us(1);

	while(IIC1_READ_DATA)
	{
		ucErrTime++;
		if(ucErrTime>2500)
		{
			IIC1_Stop();
			return 1;
		}
	}

	IIC1_SCL_LOW();

	return 0;
}

void IIC1_Ack(void)
{
	IIC1_SCL_LOW();
	IIC1_SDA_LOW();
	IIC_delay_us(2);
	IIC1_SCL_HIGH();
	IIC_delay_us(2);
	IIC1_SCL_LOW();
}

void IIC1_NAck(void)
{
	IIC1_SCL_LOW();
	IIC1_SDA_HIGH();
	IIC_delay_us(2);
	IIC1_SCL_HIGH();
	IIC_delay_us(2);
	IIC1_SCL_LOW();
}

void IIC1_Send_Byte(u8 txd)
{
	u8 t;

	IIC1_SCL_LOW();

	for(t=0;t<8;t++)
	{
		if(txd&0x80)
			IIC1_SDA_HIGH();
		else
			IIC1_SDA_LOW();

		txd<<=1;
		IIC_delay_us(2);
		IIC1_SCL_HIGH();
		IIC_delay_us(2);
		IIC1_SCL_LOW();
		IIC_delay_us(2);
	}
}

u8 IIC1_Read_Byte(unsigned char ack)
{
	unsigned char i,receive = 0;

	for(i=0;i<8;i++)
	{
		IIC1_SCL_LOW();
		IIC_delay_us(2);
		IIC1_SCL_HIGH();
		receive<<=1;

		if(IIC1_READ_DATA)receive++;

		IIC_delay_us(1);
	}

	if(!ack)
		IIC1_NAck();
	else
		IIC1_Ack();

	return receive;
}
