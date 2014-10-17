
#ifndef __FLASHHANDLER_H__
#define __FLASHHANDLER_H__

#define MULTIFLASH_ENABLE

#define FLASH_PAGE0_BASE	0x08000000
#define FLASH_CONFIG_PAGE	3
#if !defined(MULTIFLASH_ENABLE)
#define FLASH_PAGE_SIZE		0x400
#define FLASH_PAGE			128
#define FLASH_BOOT_PAGE		24					// 1 page 1K
#define CONFIG_PAGE_ADDR	(0x8000000+(FLASH_PAGE_SIZE*126))	// Page126,127	(STM32103RBT6, the last page of 128kB on-chip flash, 2kB size)
//#define FLASH_PAGE_SIZE		0x800
//#define FLASH_PAGE			64
//#define FLASH_BOOT_PAGE		12					// 1 page 2K
//#define CONFIG_PAGE_ADDR	(0x8000000+(FLASH_PAGE_SIZE*62))	// Page62,63	(STM32103RCT6, the last page of 256kB on-chip flash, 4kB size)
#define FLASH_APP_PAGE		(FLASH_PAGE - FLASH_BOOT_PAGE - FLASH_CONFIG_PAGE)
#endif

#if defined(MULTIFLASH_ENABLE)
typedef struct __Probe_Flash {
	uint32_t flash_page0_base;
	uint32_t flash_app_base;
	uint16_t flash_page_size;
	uint16_t flash_page;
	uint8_t	 flash_boot_page;
	uint8_t	 flash_config_page;
	uint16_t flash_app_page ;
	uint32_t config_page_addr ;
} __attribute__((packed)) Probe_Flash;

extern Probe_Flash flash;

void probe_flash(void);
#endif

int addr_to_page(uint32_t addr);
void erase_flash_page(uint32_t page_addr);
int write_flash(uint32_t addr, uint8_t *data, uint32_t data_len);
int read_flash(uint32_t addr, uint8_t *data, uint32_t data_len);
void save_data(uint8_t *data, uint32_t data_len, uint16_t block_number);

#endif
