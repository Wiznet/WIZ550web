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

#if !defined(MULTIFLASH_ENABLE)
#define BLOCK_SIZE	FLASH_PAGE_SIZE
//#define CONFIG_PAGE_ADDR	0x801F800	// Page126,127	(STM32103RBT6, the last page of 128kB on-chip flash, 2kB size)
#endif

int read_storage(uint8_t isConfig, void *data, uint16_t size)
{
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
}

int write_storage(uint8_t isConfig, void *data, uint16_t size)
{
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
}
