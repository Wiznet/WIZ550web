
#ifndef __EEPROMHANDLER_H__
#define __EEPROMHANDLER_H__

#define EEPROM_ENABLE
#if defined(EEPROM_ENABLE)
	#define EEPROM_ENABLE_BYI2CPERI
	#if !defined(EEPROM_ENABLE_BYI2CPERI)
	#define EEPROM_ENABLE_BYGPIO
	#endif
#endif

#define EE24AA01  128
#define EE24AA02  256
#define EE24AA04  512
#define EE24AA08  1024
#define EE24AA16  2048
#define EE24AA32  4096
#define EE24AA64  8192
#define EE24AA128 16384
#define EE24AA256 32768

#define EE_TYPE  EE24AA64

#if (EE_TYPE > EE24AA16)
#define EEPROM_BLOCK_SIZE       	512
#define EEPROM_PAGE_SIZE       		32
#else
#define EEPROM_BLOCK_SIZE       	256
#define EEPROM_PAGE_SIZE       		16
#define EEPROM_BLOCK_COUNT       	2 // 2(24AA04), 4(24AA08), 8(24AA16)
#endif

void bsp_24aaxx_gpio_init(void);

u8 EE24AAXX_ReadOneByte(u16 ReadAdder);
void EE24AAXX_WriteOneByte(u16 WriteAddr,u8 DataToWrite);
void EE24AAXX_WriteLenByte(u16 WriteAddr,u32 DataToWrite,u8 Len);
u32 EE24AAXX_ReadLenByte(u16 ReadAddr,u8 Len);
void EE24AAXX_Read(u16 ReadAddr,u8 *pBuffer,u16 NumToRead);
void EE24AAXX_Write(u16 WriteAddr,u8 *pBuffer,u16 NumToWrite);
void EE24AAXX_Init(void);
void EE24AAXX_WritePage(u16 WriteAddr,u8 *pBuffer,u8 len);

#endif
