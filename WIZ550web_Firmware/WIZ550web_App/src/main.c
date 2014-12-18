/*
===============================================================================
 Name        : main.c
 Author      : WIZnet
 Version     :
 Copyright   : WIZnet Co., Ltd. 2014
 Description : main definition
===============================================================================
*/

/*
 * @brief WIZ550WEB Firmware
 *
 * @note
 * Copyright(C) WIZnet Co., Ltd. 2014
 * All rights reserved.
 *
 */

/*****************************************************************************
 * Includes
 ****************************************************************************/

#include "stm32f10x.h"
#include "common.h"
#include "dataflash.h"
#include "httpServer.h"
#include "userHandler.h"
#include "uartHandler.h"
#include "rccHandler.h"
#include "gpioHandler.h"
#include "adcHandler.h"
#include "spiHandler.h"
#include "ConfigData.h"
#include "ConfigMessage.h"
#include "timerHandler.h"
#include "flashHandler.h"
#include "atcmd.h"
#include "dhcp.h"
#include "dhcp_cb.h"
#include "ftpd.h"

#ifdef _USE_SDCARD_
#include "ff.h"
#include "mmcHandler.h"
#include "ffconf.h"
#else
#include "dataflash.h"
#endif

///////////////////////////////////////
// Debugging Message Printout enable //
///////////////////////////////////////
#define _MAIN_DEBUG_
//#define _WEB_DEBUG_
#if defined (_MAIN_DEBUG_) || defined (_WEB_DEBUG_)
#include <stdio.h>
#endif
//////////////////////////////
// Shared Buffer Definition //
//////////////////////////////
#define DATA_BUF_SIZE   2048

uint8_t RX_BUF[DATA_BUF_SIZE];
uint8_t TX_BUF[DATA_BUF_SIZE];
#if defined(F_APP_FTP)
uint8_t FTP_DBUF[_MAX_SS];
#endif

////////////////////////////////
// W5500 HW Socket Definition //
////////////////////////////////
#if defined(F_APP_FTP)
#define MAX_HTTPSOCK	4
#else
#define MAX_HTTPSOCK	5
#endif

#define SOCK_CONFIG		0
#define SOCK_DHCP		1
#if defined(F_APP_FTP)
uint8_t socknumlist[] = {4, 5, 6, 7};
#else
uint8_t socknumlist[] = {3, 4, 5, 6, 7};
#endif
//////////////////////////////////////////


int g_mkfs_done = 0;

/*****************************************************************************
 * Private functions
 ****************************************************************************/
static void display_SDcard_Info(uint8_t mount_ret);

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/**
 * @brief    Main routine for WIZ550WEB firmware
 * @return   Function should not exit.
 */
int main(void)
{
	uint8_t i;
#if defined (_MAIN_DEBUG_) && defined (_USE_SDCARD_)
	uint8_t ret;
#endif
#if defined(F_APP_FTP)
	wiz_NetInfo gWIZNETINFO;
#endif

	S2E_Packet *value = get_S2E_Packet_pointer();

	// MCU Initialization
	RCC_Configuration();
	NVIC_Configuration();
	Timer_Configuration();

	// LED Initialization
	LED_Init(LED1);
	LED_Init(LED2);
	LED_Off(LED1);
	LED_Off(LED2);

#if defined(MULTIFLASH_ENABLE)
	probe_flash();
#endif

	// Load the Configuration data
	load_S2E_Packet_from_storage();

	// UART Initialization
	USART1_Configuration();
	USART2_Configuration();

	// W5500 Initialization
	W5500_SPI_LowLevel_Init();
	W5500_Init();

	// ADC Initialization
	adc_dmamulti_init();

#ifdef _MAIN_DEBUG_
    printf("\r\n=======================================\r\n");
	printf(" WIZnet WIZ550WEB Revision 1.0\r\n");
	printf(" Embedded Web Server Module\r\n");
	printf(" Firmware Version %d.%d.%d\r\n", MAJOR_VER, MINOR_VER, MAINTENANCE_VER);
	printf("=======================================\r\n");

	printf(" # Device Name : %s\r\n\r\n", value->module_name);
#endif

	// Initialize Network Information
	if(value->options.dhcp_use)		// DHCP
	{
		uint32_t ret;
		uint8_t dhcp_retry = 0;

#ifdef _MAIN_DEBUG_
		printf(" - DHCP Client running\r\n");
#endif

		DHCP_init(SOCK_DHCP, TX_BUF);
		reg_dhcp_cbfunc(w5500_dhcp_assign, w5500_dhcp_assign, w5500_dhcp_conflict);

		while(1)
		{
			ret = DHCP_run();

			if(ret == DHCP_IP_LEASED)
			{
#ifdef _MAIN_DEBUG_
				printf(" - DHCP Success: DHCP Leased time : %ld Sec.\r\n\r\n", getDHCPLeasetime());
#endif
				break;
			}
			else if(ret == DHCP_FAILED)
			{
				dhcp_retry++;
			}

			if(dhcp_retry > 3)
			{
#ifdef _MAIN_DEBUG_
				printf(" - DHCP Failed\r\n\r\n");
#endif
				Net_Conf();
				break;
			}

			do_udp_config(SOCK_CONFIG);
		}
	}
	else 								// Static
	{
		Net_Conf();
	}

#ifdef _MAIN_DEBUG_
	display_Net_Info();
#endif

#ifdef _USE_SDCARD_
	// SD card Initialization
	ret = mmc_mount();
	if(ret <= 0)
	{
#ifdef _MAIN_DEBUG_
		printf("\r\n - Can't mount SD card: Please Reboot WIZ550WEB Board or Insert SD card\r\n");
#endif
		//while(!(ret = mmc_mount()));
	}

	if(ret > 0)
	{
		LED_On(LED1);
#ifdef _MAIN_DEBUG_
		display_SDcard_Info(ret);
#endif
	}

#else
	// External DataFlash Initialization
	DataFlash_Init(); // DF CS Init
#endif

	atc_init();

	httpServer_init(TX_BUF, RX_BUF, MAX_HTTPSOCK, socknumlist);
#ifdef _USE_WATCHDOG_
	reg_httpServer_cbfunc(NVIC_SystemReset, IWDG_ReloadCounter); // Callback: STM32 MCU Reset / WDT Reset (IWDG)
#else
	reg_httpServer_cbfunc(NVIC_SystemReset, NULL); // Callback: STM32 MCU Reset
#endif
	IO_status_init();

#if defined(F_APP_FTP)
	ctlnetwork(CN_GET_NETINFO, (void*) &gWIZNETINFO);
	ftpd_init(1, 2, gWIZNETINFO.ip);	// Added by James for FTP
#endif

#ifdef _USE_WATCHDOG_
	// IWDG Initialization: STM32 Independent WatchDog
	IWDG_Configureation();
#endif

	// Main routine
	while(1)
	{
#ifdef _USE_WATCHDOG_
		IWDG_ReloadCounter(); // Feed IWDG
#endif

		do_udp_config(SOCK_CONFIG);				// Configuration tool handler
		atc_run();								// AT Command handler

		if(value->options.dhcp_use)	DHCP_run();	// DHCP client handler

#if defined(FACTORY_FW)
		if ((get_IO_Status(D10) == On) && (get_IO_Status(D11) == On) && (g_factoryfw_flag == 0))
		{
			printf("\r\n########## Factory Test is started.\r\n");
			g_factoryfw_flag = 1;

#if defined(FACTORY_FW_FLASH)
			save_factory_flag();

			if (check_factory_flag() == 1)
			{
				factory_test_1st();
			}
#else
			factory_test_1st();
#endif
		}

		if (g_factoryfw_flag == 1)
		{
			check_factory_uart1();
		}
#endif
#if defined(SPI_FLASH)
		if ((get_IO_Status(D10) == On) && (get_IO_Status(D11) == On) && (g_spiflash_flag == 0))
		{
			printf("\r\n########## spiflash flag is reset.\r\n");
			g_spiflash_flag = 1;
			release_factory_flag();
		}
#endif

		for(i = 0; i < MAX_HTTPSOCK; i++)	httpServer_run(i);

#if defined(F_APP_FTP)
		ftpd_run(FTP_DBUF);
#endif
#ifdef _USE_WATCHDOG_
		IWDG_ReloadCounter(); // Feed IWDG
#endif
	} // End of main routine
}

#ifdef _USE_SDCARD_
static void display_SDcard_Info(uint8_t mount_ret)
{
	uint32_t totalSize = 0, availableSize = 0;

#if !defined(SPI_FLASH)
	printf("\r\n - SD card mount succeed\r\n");
#else
	printf("\r\n - sFlash mount succeed\r\n");
#endif
	printf(" - Type : ");

	switch(mount_ret)
	{
		case CARD_MMC: printf("MMC\r\n"); 	break;
		case CARD_SD: printf("SD\r\n"); 	break;
		case CARD_SD2: printf("SD2\r\n"); 	break;
		case CARD_SDHC: printf("SDHC\r\n"); break;
		case SPI_FLASHM: printf("sFlash\r\n"); break;
		default: printf("\r\n"); 	break;
	}

	if(_MAX_SS == 512)
	{
		getMountedMemorySize(&totalSize, &availableSize);
		printf(" - Available Memory Size : %ld kB / %ld kB ( %ld kB is used )\r\n", availableSize, totalSize, (totalSize - availableSize));
	}
	printf("\r\n");
}
#endif
