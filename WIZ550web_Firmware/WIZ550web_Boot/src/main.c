//
// This file is part of the GNU ARM Eclipse distribution.
// Copyright (c) 2014 Liviu Ionescu.
//

// ----------------------------------------------------------------------------

#include <stdio.h>
#include <string.h>
#include "diag/Trace.h"

#include "stm32f10x.h"
#include "common.h"
#include "rccHandler.h"
#include "spiHandler.h"
#include "uartHandler.h"
#include "flashHandler.h"
#include "storageHandler.h"
#include "gpioHandler.h"
#include "timerHandler.h"
#include "tftp.h"
#include "ConfigData.h"
#include "ConfigMessage.h"

// ----------------------------------------------------------------------------
//
// STM32F1 empty sample (trace via NONE).
//
// Trace support is enabled by adding the TRACE macro definition.
// By default the trace messages are forwarded to the NONE output,
// but can be rerouted to any device or completely suppressed, by
// changing the definitions required in system/src/diag/trace_impl.c
// (currently OS_USE_TRACE_ITM, OS_USE_TRACE_SEMIHOSTING_DEBUG/_STDOUT).
//

// ----- main() ---------------------------------------------------------------
uint8_t socket_buf[2048];

// Sample pragmas to cope with warnings. Please note the related line at
// the end of this function, used to pop the compiler diagnostics status.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"

void application_jump(void)
{
	//DBG_PRINT(INFO_DBG, "\r\n### Application Start... ###\r\n");

	__disable_irq();

	/* Set Stack Pointer */
	asm volatile("ldr r0, =0x08006000");
	asm volatile("ldr r0, [r0]");
	asm volatile("mov sp, r0");

	/* Jump to Application ResetISR */
	asm volatile("ldr r0, =0x08006004");
	asm volatile("ldr r0, [r0]");
	asm volatile("mov pc, r0");
}

int application_update(void)
{
	Firmware_Upload_Info firmware_upload_info;
	uint8_t firmup_flag = 0;

	read_storage(0, &firmware_upload_info, sizeof(Firmware_Upload_Info));
	if(firmware_upload_info.wiznet_header.stx == STX) {
		firmup_flag = 1;
	}

	if(firmup_flag) {
		uint32_t tftp_server;
		uint8_t *filename;
		int ret;

		//DBG_PRINT(INFO_DBG, "### Application Update... ###\r\n");
		tftp_server = (firmware_upload_info.tftp_info.ip[0] << 24) | (firmware_upload_info.tftp_info.ip[1] << 16) | (firmware_upload_info.tftp_info.ip[2] << 8) | (firmware_upload_info.tftp_info.ip[3]);
		filename = firmware_upload_info.filename;

		TFTP_read_request(tftp_server, filename);

		while(1) {
			ret = TFTP_run();
			if(ret != TFTP_PROGRESS)
				break;
		}

		if(ret == TFTP_SUCCESS) {
			reply_firmware_upload_done(SOCK_CONFIG);

			memset(&firmware_upload_info, 0 ,sizeof(Firmware_Upload_Info));
			write_storage(0, &firmware_upload_info, sizeof(Firmware_Upload_Info));
		}

		return ret;
	}

	return 0;
}

int main(int argc, char* argv[])
{
	int ret;

	RCC_Configuration();
	
	LED_Init(LED1);
	LED_Init(LED2);
	
	LED_On(LED1);
	LED_Off(LED2);

	BOOT_Pin_Init();

	USART1_Configuration();

#if defined(MULTIFLASH_ENABLE)
	probe_flash();
#endif

	/* Load Configure Infomation */
	load_S2E_Packet_from_storage();

	/* Check MAC Address */
	check_mac_address();

	W5500_SPI_Init();
	W5500_Init();
	Timer_Configuration();

	Net_Conf();
	TFTP_init(SOCK_TFTP, socket_buf);

	ret = application_update();

	if((get_bootpin_Status() == 0) && (ret != TFTP_FAIL)) {
		uint32_t tmp;

#if !defined(MULTIFLASH_ENABLE)
		tmp = *(volatile uint32_t *)APP_BASE;
#else
		tmp = *(volatile uint32_t *)flash.flash_app_base;
#endif

		if((tmp & 0xffffffff) != 0xffffffff) {
			application_jump();
		}
	}

	while (1) {
		do_udp_config(SOCK_CONFIG);
	}
}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
