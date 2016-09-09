#include <string.h>
#include "stm32f10x.h"
#include "common.h"
#include "flashHandler.h"
#include "uartHandler.h"
#include "tftp.h"

#if defined(MULTIFLASH_ENABLE)
Probe_Flash flash;
#endif

int addr_to_page(uint32_t addr)
{
	uint32_t mask = 0;

#if !defined(MULTIFLASH_ENABLE)
	mask = APP_BASE - FLASH_PAGE0_BASE - 1;
#else
	mask = flash.flash_app_base - flash.flash_page0_base - 1;
#endif
	addr &= mask;

#if !defined(MULTIFLASH_ENABLE)
	return (addr / FLASH_PAGE_SIZE);
#else
	return (addr / flash.flash_page_size);
#endif
}

void erase_flash_page(uint32_t page_addr)
{
	FLASH_Status FLASHStatus = FLASH_COMPLETE;

	__disable_irq();
	FLASH_Unlock();

	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);	

	FLASHStatus = FLASH_ErasePage(page_addr);
	if(FLASHStatus != FLASH_COMPLETE)
		myprintf("FLASH_ErasePage Error\r\n");

	FLASH_Lock();
	__enable_irq();
}

int write_flash(uint32_t addr, uint8_t *data, uint32_t data_len)
{
	FLASH_Status FLASHStatus = FLASH_COMPLETE;
	int rep = 0, i;
	uint32_t tmp;

	__disable_irq();

	rep = (data_len / 4);
	if(data_len % 4)
		rep++;

	FLASH_Unlock();
	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);	

	for(i = 0 ; i < rep ; i++) {
		tmp = *((uint32_t *)data + i);
		FLASHStatus = FLASH_ProgramWord(addr, tmp);
		if(FLASHStatus != FLASH_COMPLETE) {
			myprintf("FLASH_ProgramWord Error\r\n");
			return -1;
		}

		addr += 4;
	}

	FLASH_Lock();

	__enable_irq();

	return data_len;
}

int read_flash(uint32_t addr, uint8_t *data, uint32_t data_len)
{
	uint32_t i;

	for(i = 0; i < data_len; i++)
	{
		data[i] = *(uint8_t *) (addr + i);
	}

	return i;
}

/*
 * TFTP Dependency
 */
static uint8_t *g_write_point = (uint8_t *)APP_BASE;
void save_data(uint8_t *data, uint32_t data_len, uint16_t block_number)
{
	static uint32_t cnt = 0;
	int i;

	if(block_number == 1) {
#if !defined(MULTIFLASH_ENABLE)
		for(i = 0 ; i < FLASH_APP_PAGE; i++) {
			erase_flash_page(APP_BASE + (FLASH_PAGE_SIZE * i));
		}
#else
		for(i = 0 ; i < flash.flash_app_page; i++) {
			erase_flash_page(flash.flash_app_base + (flash.flash_page_size * i));
		}
#endif
	}

	//DBG_PRINT(INFO_DBG, "#");
	cnt += data_len;

	if(data_len < TFTP_BLK_SIZE) {
		memset(data + data_len, 0xff, TFTP_BLK_SIZE - data_len);
		write_flash((uint32_t)g_write_point, data, TFTP_BLK_SIZE);
		//DBG_PRINT(INFO_DBG, "\r\nwrite flash %d bytes\r\n", cnt);
#if !defined(MULTIFLASH_ENABLE)
		g_write_point = (uint8_t *)APP_BASE;
#else
		g_write_point = (uint8_t *)flash.flash_app_base;
#endif
		cnt = 0;
	}
	else {
		write_flash((uint32_t)g_write_point, data, data_len);
		g_write_point += data_len;
	}
}

#if defined(MULTIFLASH_ENABLE)
void probe_flash(void)
{
	uint32_t flash_kb = *((unsigned short *)0x1FFFF7E0);

    if(flash_kb >= 256)
    {
    	myprintf("HIGH Density Flash memory\r\n");

    	flash.flash_page0_base = FLASH_PAGE0_BASE;
    	flash.flash_app_base = APP_BASE;
    	flash.flash_page_size = 0x800;
    	flash.flash_page = 128;//64;
    	flash.flash_boot_page = 14;//12;
    	flash.flash_config_page = FLASH_CONFIG_PAGE;
    	flash.flash_app_page = flash.flash_page - flash.flash_boot_page - flash.flash_config_page;
    	//flash.config_page_addr = flash.flash_page0_base + (flash.flash_page_size*62);
    	flash.config_page_addr = flash.flash_page0_base + (flash.flash_page_size*126);
    }
    else
    {
    	myprintf("Medium Density Flash memory\r\n");

    	flash.flash_page0_base = FLASH_PAGE0_BASE;
    	flash.flash_app_base = APP_BASE;
    	flash.flash_page_size = 0x400;
    	flash.flash_page = 128;
    	flash.flash_boot_page = 28;//24;
    	flash.flash_config_page = FLASH_CONFIG_PAGE;
    	flash.flash_app_page = flash.flash_page - flash.flash_boot_page - flash.flash_config_page;
    	flash.config_page_addr = flash.flash_page0_base + (flash.flash_page_size*126);
    }
}
#endif
