/*
 * storageHandler.c
 *
 *  Created on: 2014. 9. 16.
 *      Author: Raphael Kwon
 */


#include <string.h>
#include "stm32f10x.h"
#include "storageHandler.h"
#include "flashHandler.h"
#include "i2cHandler.h"
#include "eepromHandler.h"

#if !defined(MULTIFLASH_ENABLE)
#define BLOCK_SIZE	FLASH_PAGE_SIZE
//#define CONFIG_PAGE_ADDR	0x801F800	// Page126,127	(STM32103RBT6, the last page of 128kB on-chip flash, 2kB size)
#endif

int read_storage(uint8_t isConfig, void *data, uint16_t size)
{
#if !defined(EEPROM_ENABLE)
	uint32_t address;

	if(isConfig == 1) {
#if !defined(MULTIFLASH_ENABLE)
		address = BLOCK_SIZE + CONFIG_PAGE_ADDR;
#else
		address = flash.flash_page_size + flash.config_page_addr;
#endif
	}
	else {
#if !defined(MULTIFLASH_ENABLE)
		address = 0x00000000 + CONFIG_PAGE_ADDR;
#else
		address = 0x00000000 + flash.config_page_addr;
#endif
	}

	return read_flash(address, data, size);
#else
	uint8_t Receive_Data[EEPROM_BLOCK_SIZE];
	uint16_t addr;
	uint16_t offset;

	if(size > EEPROM_BLOCK_SIZE)
		size = EEPROM_BLOCK_SIZE;

	addr = 0;

	memset(&Receive_Data[0], 0x00, EEPROM_BLOCK_SIZE);

	if(isConfig == 0)
		offset = 0x00;
	else
		offset = 0x00+EEPROM_BLOCK_SIZE;

#if defined(EEPROM_ENABLE_BYI2CPERI)
	EEP_Read(&Receive_Data[0], addr+offset, size);
#elif defined(EEPROM_ENABLE_BYGPIO)
	EE24AAXX_Read(addr+offset, &Receive_Data[0], size);
#endif

	memcpy(data, &Receive_Data[0], size);

	delay_ms(50);

	return 0;
#endif
}

int write_storage(uint8_t isConfig, void *data, uint16_t size)
{
#if !defined(EEPROM_ENABLE)
	uint32_t address;

	if(isConfig == 1) {
#if !defined(MULTIFLASH_ENABLE)
		address = BLOCK_SIZE + CONFIG_PAGE_ADDR;
#else
		address = flash.flash_page_size + flash.config_page_addr;
#endif
	}
	else {
#if !defined(MULTIFLASH_ENABLE)
		address = 0x00000000 + CONFIG_PAGE_ADDR;
#else
		address = 0x00000000 + flash.config_page_addr;
#endif
	}

	erase_flash_page(address);
	return write_flash(address, data, size);
#else
	uint8_t Transmit_Data[EEPROM_BLOCK_SIZE];
	uint16_t addr;
	uint16_t offset;
	uint8_t page, rest, i;

	memset(&Transmit_Data[0], 0x00, EEPROM_BLOCK_SIZE);
	memcpy(&Transmit_Data[0], data, size);

	if(size > EEPROM_BLOCK_SIZE)
		size = EEPROM_BLOCK_SIZE;

	page = size/EEPROM_PAGE_SIZE;
	rest = size%EEPROM_PAGE_SIZE;

	addr = 0;

	if(isConfig == 0)
		offset = 0x00;
	else
		offset = 0x00+EEPROM_BLOCK_SIZE;

	if(size < EEPROM_PAGE_SIZE)
	{
#if defined(EEPROM_ENABLE_BYI2CPERI)
		EEP_Write(&Transmit_Data[0], addr+offset, size);
#elif defined(EEPROM_ENABLE_BYGPIO)
		EE24AAXX_WritePage(addr+offset, &Transmit_Data[0], size);
#endif
		delay_ms(10);
	}
	else
	{
		for(i=0; i<page; i++)
		{
			addr = i*EEPROM_PAGE_SIZE;
#if defined(EEPROM_ENABLE_BYI2CPERI)
			EEP_Write(&Transmit_Data[addr], addr+offset, EEPROM_PAGE_SIZE);
#elif defined(EEPROM_ENABLE_BYGPIO)
			EE24AAXX_WritePage(addr+offset, &Transmit_Data[addr], EEPROM_PAGE_SIZE);
#endif
			delay_ms(10);
		}

		if(rest != 0)
		{
			addr = page*EEPROM_PAGE_SIZE;
#if defined(EEPROM_ENABLE_BYI2CPERI)
			EEP_Write(&Transmit_Data[addr], addr+offset, rest);
#elif defined(EEPROM_ENABLE_BYGPIO)
			EE24AAXX_WritePage(addr+offset, &Transmit_Data[addr], rest);
#endif
			delay_ms(10);
		}
	}

	delay_ms(50);

	return 0;
#endif
}
