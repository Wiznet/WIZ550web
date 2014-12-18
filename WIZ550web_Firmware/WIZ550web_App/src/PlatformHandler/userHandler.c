/**
 * @file	userHandler.c
 * @brief	User Control Example
 * @version 1.0
 * @date	2014/07/15
 * @par Revision
 *			2014/07/15 - 1.0 Release
 * @author	
 * \n\n @par Copyright (C) 1998 - 2014 WIZnet. All rights reserved.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "userHandler.h"
#include "stm32f10x.h"
#include "types.h"
#include "boardutil.h"
#include "common.h"
#include "adcHandler.h"
#include "ConfigData.h"
#include "flashHandler.h"
#include "gpioHandler.h"

#include "httpParser.h"
#include "uartHandler.h"

#if !defined(MULTIFLASH_ENABLE)
//#define IO_PAGE_ADDR	0x801F400	// Page125
//#define IO_PAGE_ADDR	(0x8000000+(FLASH_PAGE_SIZE*61))	// Page125
#endif

IOStorage IOdata;

uint16_t ADC_value;
uint16_t TemperatureC;

uint32_t baudrate_table[10] = {
	baud_600,
	baud_1200,
	baud_2400,
	baud_4800,
	baud_9600,
	baud_19200,
	baud_38400,
	baud_57600,
	baud_115200,
	baud_230400
};

int read_IOstorage(void *data, uint16_t size)
{
	uint32_t address;

#if !defined(MULTIFLASH_ENABLE)
	address = IO_PAGE_ADDR;
#else
	address = flash.io_page_addr;
#endif

	return read_flash(address, data, size);
}

int write_IOstorage(void *data, uint16_t size)
{
	uint32_t address;

#if !defined(MULTIFLASH_ENABLE)
	address = IO_PAGE_ADDR;
#else
	address = flash.io_page_addr;
#endif

	erase_flash_page(address);
	return write_flash(address, data, size);
}

/**
 * @brief This function used to read the saved IO status or register default IO settings
 * @param None
 * @return None
 */
void IO_status_init(void)
{
	uint8_t i;
	read_IOstorage(&IOdata, sizeof(IOdata));

	// IO Status & Alias Initialization
	// Initialize for WIZ550WEB Baseboard Configuration
	if(!((IOdata.io_statuscode[0] == 0xAA) && (IOdata.io_statuscode[1] == 0x01)))
	{
#ifdef _WEB_DEBUG_
		printf("\r\nIO Status Detection code: [%.02x][%.02x]\r\n", IOdata.io_statuscode[0], IOdata.io_statuscode[1]);
#endif
		// IO Status Detection code Initialize
		IOdata.io_statuscode[0] = 0xAA;
		IOdata.io_statuscode[1] = 0x01;

		// IO Direction Initialize
		IOdata.io[D0] = Output;
		IOdata.io[D1] = Output;
		IOdata.io[D2] = Output;
		IOdata.io[D3] = Output;
		IOdata.io[D4] = Output;
		IOdata.io[D5] = Output;
		IOdata.io[D6] = Output;
		IOdata.io[D7] = Output;
		IOdata.io[D8] = Input;
		IOdata.io[D9] = Input;
		IOdata.io[D10] = Input;
		IOdata.io[D11] = Input;
		IOdata.io[D12] = Input;
		IOdata.io[D13] = Input;
		IOdata.io[D14] = Input;
		IOdata.io[D15] = Input;

		// Each IO Status Initialize
		for(i = 0; i < IOn; i++)
		{
			IOdata.ios[i] = Off;
		}

		// IO Alias fields Initialize
		for(i = 0; i < IOn; i++)
		{
			memset(IOdata.io_alias[i], 0x00, 25);
			sprintf((char *)IOdata.io_alias[i], "Digital IO #%d", i);
		}

		write_IOstorage(&IOdata, sizeof(IOdata));

#ifdef _WEB_DEBUG_
		printf(">> Default IO Status / Alias Initialize Success\r\n\r\n");
#endif
	}
#ifdef _WEB_DEBUG_
	else
	{
		printf(">> IO Status / Alias Load Success\r\n\r\n");
	}
#endif

	GPIO_PinRemapConfig (GPIO_Remap_SWJ_Disable, ENABLE);

	// IO Direction setting after load or set default
	for(i = 0; i < IOn; i++)
	{
		if((IOdata.io[i] != NotUsed) && (IOdata.io[i] != 0xFF))
		{
			IO_Init(i);
		}
	}

	// Each IO Status Initialize
	for(i = 0; i < IOn; i++)
	{
		if(IOdata.io[i] == Output)
		{
			if(IOdata.ios[i] == Off)	IO_Off(i);
			else						IO_On(i);
		}
	}
}

/**
 * @brief To get adc value of A0 ~ A3 from a WIZnet module.
 * @param index The sequence for A0 ~ A3 registration
 * @return adc value (uint16_t)
 */
uint16_t get_ADC_val(uint8_t index)
{
	uint16 adc_value = 0;

#if 0
	switch(index)
	{
		case A0: // WIZ550WEB BaseBoard: Potentiometer
			if(ADC_GetFlagStatus(ADC2, ADC_FLAG_EOC) == SET) adc_value = ADC_GetConversionValue(ADC2);
			break;
		case A1: // WIZ550WEB BaseBoard: Temperature Sensor
			adc_value = ADC1ConvertedValue; // TemperatureC = (((ADC_value * 3300) / 1023) - 500) / 10;
			break;
		case A2:
			adc_value = 1000;
			break;
		case A3:
			adc_value = 2000;
			break;
		default:
			adc_value = 0;
			break;
	}
#else
	adc_value = ADC_DualConvertedValueTab[index];
#endif

	return adc_value;
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// HTTP GET Method PL Functions
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void make_basic_config_setting_json_callback(uint8_t * buf, uint16_t * len)
{
	S2E_Packet *value = get_S2E_Packet_pointer();
	uint8_t baudrate_index[2] = {0, };
	uint8_t databit_index[2] = {0, };
	uint8_t stopbit_index[2] = {0, };
	uint8_t i;

	for(i = 0; i < 2; i++)
	{
		switch(value->serial_info[i].baud_rate)
		{
			case baud_230400 :
				baudrate_index[i] = BAUD_230400_INDEX;
				break;
			case baud_115200 :
				baudrate_index[i] = BAUD_115200_INDEX;
				break;
			case baud_57600 :
				baudrate_index[i] = BAUD_57600_INDEX;
				break;
			case baud_38400 :
				baudrate_index[i] = BAUD_38400_INDEX;
				break;
			case baud_19200 :
				baudrate_index[i] = BAUD_19200_INDEX;
				break;
			case baud_9600 :
				baudrate_index[i] = BAUD_9600_INDEX;
				break;
			case baud_4800 :
				baudrate_index[i] = BAUD_4800_INDEX;
				break;
			case baud_2400 :
				baudrate_index[i] = BAUD_2400_INDEX;
				break;
			case baud_1200 :
				baudrate_index[i] = BAUD_1200_INDEX;
				break;
			case baud_600 :
				baudrate_index[i] = BAUD_600_INDEX;
				break;
			default :
				baudrate_index[i] = BAUD_115200_INDEX;
				break;
		}

		if(value->serial_info[i].data_bits == word_len9) databit_index[i] = DATA_BIT9_INDEX;
		else databit_index[i] = DATA_BIT8_INDEX;

		if(value->serial_info[i].stop_bits == stop_bit2) stopbit_index[i] = STOP_BIT2_INDEX;
		else stopbit_index[i] = STOP_BIT1_INDEX;
	}

	*len = sprintf((char *)buf, "BasicSettingCallback({\"ver\":\"%d.%d.%d\",\
                                        \"dname\":\"%s\",\
                                        \"pcode\":\"%d-%d-%d\",\
                                        \"mac\":\"%02X:%02X:%02X:%02X:%02X:%02X\",\
                                        \"dhcp\":\"%d\",\
                                        \"lip\":\"%d.%d.%d.%d\",\
                                        \"gw\":\"%d.%d.%d.%d\",\
                                        \"sub\":\"%d.%d.%d.%d\",\
                                        \"dns\":\"%d.%d.%d.%d\",\
                                        \"baud1\":\"%d\",\
                                        \"databit1\":\"%d\",\
                                        \"parity1\":\"%d\",\
                                        \"stopbit1\":\"%d\",\
                                        \"flow1\":\"%d\",\
										\"baud2\":\"%d\",\
                                        \"databit2\":\"%d\",\
                                        \"parity2\":\"%d\",\
                                        \"stopbit2\":\"%d\",\
                                        \"flow2\":\"%d\"\
                                        });",value->fw_ver[0], value->fw_ver[1], value->fw_ver[2],
                                        value->module_name,
                                        value->module_type[0],value->module_type[1],value->module_type[2],
                                        value->network_info_common.mac[0],value->network_info_common.mac[1],value->network_info_common.mac[2],value->network_info_common.mac[3],value->network_info_common.mac[4],value->network_info_common.mac[5],
                                        value->options.dhcp_use,
                                        value->network_info_common.local_ip[0],value->network_info_common.local_ip[1],value->network_info_common.local_ip[2],value->network_info_common.local_ip[3],
                                        value->network_info_common.gateway[0],value->network_info_common.gateway[1],value->network_info_common.gateway[2],value->network_info_common.gateway[3],
                                        value->network_info_common.subnet[0],value->network_info_common.subnet[1],value->network_info_common.subnet[2],value->network_info_common.subnet[3],
                                        value->options.dns_server_ip[0],value->options.dns_server_ip[1],value->options.dns_server_ip[2],value->options.dns_server_ip[3],
                                        baudrate_index[0], databit_index[0], value->serial_info[0].parity, stopbit_index[0], value->serial_info[0].flow_control,
                                        baudrate_index[1], databit_index[1], value->serial_info[1].parity, stopbit_index[1], value->serial_info[1].flow_control
                                        );
}


void make_json_io_status(uint8_t * buf, uint16_t * len)
{

	*len = sprintf((char *)buf, "IoStatusCallback({\"din\":[{\"v\":\"%d\"},\
															{\"v\":\"%d\"},\
															{\"v\":\"%d\"},\
															{\"v\":\"%d\"},\
															{\"v\":\"%d\"},\
															{\"v\":\"%d\"},\
															{\"v\":\"%d\"},\
															{\"v\":\"%d\"},\
															{\"v\":\"%d\"},\
															{\"v\":\"%d\"},\
															{\"v\":\"%d\"},\
															{\"v\":\"%d\"},\
															{\"v\":\"%d\"},\
															{\"v\":\"%d\"},\
															{\"v\":\"%d\"},\
															{\"v\":\"%d\"}],\
												  \"dout\":[{\"v\":\"%d\"},\
															{\"v\":\"%d\"},\
															{\"v\":\"%d\"},\
															{\"v\":\"%d\"},\
															{\"v\":\"%d\"},\
															{\"v\":\"%d\"},\
															{\"v\":\"%d\"},\
															{\"v\":\"%d\"},\
															{\"v\":\"%d\"},\
															{\"v\":\"%d\"},\
															{\"v\":\"%d\"},\
															{\"v\":\"%d\"},\
															{\"v\":\"%d\"},\
															{\"v\":\"%d\"},\
															{\"v\":\"%d\"},\
															{\"v\":\"%d\"}],\
												  \"led\":[{\"v\":\"%d\"},\
															{\"v\":\"%d\"}]});",
															get_IO_Status(D0), get_IO_Status(D1), get_IO_Status(D2), get_IO_Status(D3),
															get_IO_Status(D4), get_IO_Status(D5), get_IO_Status(D6), get_IO_Status(D7),
															get_IO_Status(D8), get_IO_Status(D9), get_IO_Status(D10), get_IO_Status(D11),
															get_IO_Status(D12), get_IO_Status(D13), get_IO_Status(D14), get_IO_Status(D15),
															IOdata.io[D0], IOdata.io[D1], IOdata.io[D2], IOdata.io[D3],
															IOdata.io[D4], IOdata.io[D5], IOdata.io[D6], IOdata.io[D7],
															IOdata.io[D8], IOdata.io[D9], IOdata.io[D10], IOdata.io[D11],
															IOdata.io[D12], IOdata.io[D13], IOdata.io[D14], IOdata.io[D15],
															get_LED_Status(0),
															get_LED_Status(1)
															);
}

void make_json_ioalias_status(uint8_t * buf, uint16_t * len)
{

	*len = sprintf((char *)buf, "IoAliasStatusCallback({\"dstate\":[{\"v\":\"%d\"},\
															{\"v\":\"%d\"},\
															{\"v\":\"%d\"},\
															{\"v\":\"%d\"},\
															{\"v\":\"%d\"},\
															{\"v\":\"%d\"},\
															{\"v\":\"%d\"},\
															{\"v\":\"%d\"},\
															{\"v\":\"%d\"},\
															{\"v\":\"%d\"},\
															{\"v\":\"%d\"},\
															{\"v\":\"%d\"},\
															{\"v\":\"%d\"},\
															{\"v\":\"%d\"},\
															{\"v\":\"%d\"},\
															{\"v\":\"%d\"}],\
												   \"dir\":[{\"v\":\"%d\"},\
															{\"v\":\"%d\"},\
															{\"v\":\"%d\"},\
															{\"v\":\"%d\"},\
															{\"v\":\"%d\"},\
															{\"v\":\"%d\"},\
															{\"v\":\"%d\"},\
															{\"v\":\"%d\"},\
															{\"v\":\"%d\"},\
															{\"v\":\"%d\"},\
															{\"v\":\"%d\"},\
															{\"v\":\"%d\"},\
															{\"v\":\"%d\"},\
															{\"v\":\"%d\"},\
															{\"v\":\"%d\"},\
															{\"v\":\"%d\"}],\
												 \"alias\":[{\"a\":\"%s\"},\
															{\"a\":\"%s\"},\
															{\"a\":\"%s\"},\
															{\"a\":\"%s\"},\
															{\"a\":\"%s\"},\
															{\"a\":\"%s\"},\
															{\"a\":\"%s\"},\
															{\"a\":\"%s\"},\
															{\"a\":\"%s\"},\
															{\"a\":\"%s\"},\
															{\"a\":\"%s\"},\
															{\"a\":\"%s\"},\
															{\"a\":\"%s\"},\
															{\"a\":\"%s\"},\
															{\"a\":\"%s\"},\
															{\"a\":\"%s\"}]});",
															get_IO_Status(D0), get_IO_Status(D1), get_IO_Status(D2), get_IO_Status(D3),
															get_IO_Status(D4), get_IO_Status(D5), get_IO_Status(D6), get_IO_Status(D7),
															get_IO_Status(D8), get_IO_Status(D9), get_IO_Status(D10), get_IO_Status(D11),
															get_IO_Status(D12), get_IO_Status(D13), get_IO_Status(D14), get_IO_Status(D15),
															IOdata.io[D0], IOdata.io[D1], IOdata.io[D2], IOdata.io[D3],
															IOdata.io[D4], IOdata.io[D5], IOdata.io[D6], IOdata.io[D7],
															IOdata.io[D8], IOdata.io[D9], IOdata.io[D10], IOdata.io[D11],
															IOdata.io[D12], IOdata.io[D13], IOdata.io[D14], IOdata.io[D15],
															IOdata.io_alias[D0], IOdata.io_alias[D1], IOdata.io_alias[D2], IOdata.io_alias[D3],
															IOdata.io_alias[D4], IOdata.io_alias[D5], IOdata.io_alias[D6], IOdata.io_alias[D7],
															IOdata.io_alias[D8], IOdata.io_alias[D9], IOdata.io_alias[D10], IOdata.io_alias[D11],
															IOdata.io_alias[D12], IOdata.io_alias[D13], IOdata.io_alias[D14], IOdata.io_alias[D15]
															);
}

void make_json_adc_status(uint8_t * buf, uint16_t * len)
{

	*len = sprintf((char *)buf, "ADCStatusCallback({\"ain\":[{\"v\":\"%d\"},\
															{\"v\":\"%d\"},\
															{\"v\":\"%d\"},\
															{\"v\":\"%d\"}],\
												  \"aout\":[{\"v\":\"%d\"},\
															{\"v\":\"%d\"}]});",
															get_ADC_val(A0),
															get_ADC_val(A1),
															get_ADC_val(A2),
															get_ADC_val(A3),
															100, 200
															);
}

void make_json_serial_data(uint8_t * buf, uint16_t * len)
{
	uint8_t * buf_ptr;
	buf_ptr = getUSART1buf();
	*len = sprintf((char *)buf,"getRs232DataCallback({\"data\":\"%s\"});", buf_ptr); // serial buffer

	// Serial data buffer clear
	USART1_flush();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// HTTP POST Method PL Functions
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

uint8_t * set_basic_config_setting(uint8_t * uri)
{
	uint8_t * param;
	uint8_t str_size, baudrate_idx;
	S2E_Packet *value = get_S2E_Packet_pointer();

	// Redirection IP address init
	uint8_t return_ip[4] = {value->network_info_common.local_ip[0],
							value->network_info_common.local_ip[1],
							value->network_info_common.local_ip[2],
							value->network_info_common.local_ip[3]};

	uint8_t * ip = return_ip;

	if((param = get_http_param_value((char *)uri, "devicename")))
	{
		memset(value->module_name, 0x00, 25);
		if((str_size = strlen((char*)param)) > 24) str_size = 24; // exception handling
		memcpy(value->module_name, param, str_size);
	}

	if((param = get_http_param_value((char *)uri, "dhcp")))
	{
		if(strstr((char const*)param, "1") != NULL) value->options.dhcp_use = 1; // DHCP mode
		else value->options.dhcp_use = 0; // Static mode
	}

	if(value->options.dhcp_use == 0) // Static mode
	{
		if((param = get_http_param_value((char *)uri, "lip")))
		{
			inet_addr_((u_char*)param, value->network_info_common.local_ip);

			return_ip[0] = value->network_info_common.local_ip[0];
			return_ip[1] = value->network_info_common.local_ip[1];
			return_ip[2] = value->network_info_common.local_ip[2];
			return_ip[3] = value->network_info_common.local_ip[3];
		}
		if((param = get_http_param_value((char *)uri, "gw")))
		{
			inet_addr_((u_char*)param, value->network_info_common.gateway);
		}
		if((param = get_http_param_value((char *)uri, "sub")))
		{
			inet_addr_((u_char*)param, value->network_info_common.subnet);
		}
		if((param = get_http_param_value((char *)uri, "dns")))
		{
			inet_addr_((u_char*)param, value->options.dns_server_ip);
		}
	}

	if((param = get_http_param_value((char *)uri, "baudrate")))
	{
		baudrate_idx = ATOI(param, 10);
		value->serial_info[0].baud_rate = baudrate_table[baudrate_idx];
	}

	if((param = get_http_param_value((char *)uri, "databit")))
	{
		value->serial_info[0].data_bits = ATOI(param, 10);
	}

	if((param = get_http_param_value((char *)uri, "parity")))
	{
		value->serial_info[0].parity = ATOI(param, 10);
	}

	if((param = get_http_param_value((char *)uri, "stopbit")))
	{
		value->serial_info[0].stop_bits = ATOI(param, 10);
	}

	if((param = get_http_param_value((char *)uri, "flowcontrol")))
	{
		value->serial_info[0].flow_control = ATOI(param, 10);
	}

	save_S2E_Packet_to_storage();

	return ip;
}

uint8_t set_digital_io_output(uint8_t * uri)
{
	uint8_t * param;
	uint8_t pin = 0, val = 0;

	if((param = get_http_param_value((char *)uri, "pin")))
	{
		pin = (uint8_t)ATOI(param, 10);
	}
	if((param = get_http_param_value((char *)uri, "val")))
	{
		val = (uint8_t)ATOI(param, 10);
	}
#ifdef _WEB_DEBUG_
	printf(">> Set digital IO output val: pin = %d, val = %d\r\n", pin, val);
#endif

	if(pin == 90 || pin == 91)						// LED0, tmp pin num
	{
		if(val == On) LED_Off(pin-90);
		else LED_On(pin-90);
	}
	else if(IOdata.io[pin] == Output)	// GPIOs (D0 ~ D15)
	{
		if(val == On)
		{
			IOdata.ios[pin] = Off;
			IO_Off(pin);
		}
		else
		{
			IOdata.ios[pin] = On;
			IO_On(pin);
		}
		write_IOstorage(&IOdata, sizeof(IOdata));
	}

	return val;
}


uint8_t set_digital_io_alias(uint8_t * uri)
{
	uint8_t * param;
	uint8_t pin = 0;
	uint8_t str_size = 0;

	if((param = get_http_param_value((char *)uri, "pin")))
	{
		pin = (uint8_t)ATOI(param, 10);

		if((param = get_http_param_value((char *)uri, "val")))
		{
			memset(IOdata.io_alias[pin], 0x00, 25);
			if((str_size = strlen((char*)param)) > 24) str_size = 24; // exception handling
			memcpy(IOdata.io_alias[pin], param, str_size);
		}
		write_IOstorage(&IOdata, sizeof(IOdata));
	}

	return pin;
}

uint8_t set_digital_io_direction(uint8_t * uri)
{
	uint8_t * param;
	uint8_t pin = 0, dir = 0;

	if((param = get_http_param_value((char *)uri, "pin")))
	{
		pin = (uint8_t)ATOI(param, 10);
	}
	if((param = get_http_param_value((char *)uri, "dir")))
	{
		dir = (uint8_t)ATOI(param, 10);
	}

	if(dir == Output) 		// Direction: Output
	{
		IOdata.io[pin] = Output;
		IO_Init(pin);
	}
	else if(dir == Input) 	// Direction: Input
	{
		IOdata.io[pin] = Input;
		IO_Init(pin);
	}
	else					// Not used
	{
		IO_Off(pin);
		IOdata.io[pin] = NotUsed;
	}

	write_IOstorage(&IOdata, sizeof(IOdata));

	return dir;
}

void get_serial_data_from_web(uint8_t * uri)
{
	uint8_t * param;
	uint16_t i;

	if((param = get_http_param_value((char *)uri, "data")))
	{
#ifdef _WEB_DEBUG_
		printf("\r\n>> SerialData from Web [%d] byte : %s\r\n", strlen((char *)param), param);
#endif
		// Serial Data transfer to USART1
		for(i = 0; i < strlen((char *)param); i++)
		{
			Usart_putc(*(param+i));
		}

#ifdef _WEB_DEBUG_
		printf("\r\n");
#endif
	}
}

void set_factory_default_io_status(uint8_t * uri)
{
	uint8_t * param;
	uint8_t val = 0;

	if((param = get_http_param_value((char *)uri, "io_reset")))
	{
		val = (uint8_t)ATOI(param, 10);

		if(val == 1)
		{
			// IO Status Detection code clear
			IOdata.io_statuscode[0] = 0xff;
			IOdata.io_statuscode[1] = 0xff;

			write_IOstorage(&IOdata, sizeof(IOdata));

#ifdef _WEB_DEBUG_
			printf(">>IO Status / Alias Set to the factory default value\r\n");
#endif
		}
	}
}

void make_pl_basic_config_response_page(uint16_t delay, uint8_t * url, uint8_t * pl_response_buf, uint16_t * len)
{
	S2E_Packet *value = get_S2E_Packet_pointer();

	if(value->options.dhcp_use == 1) // Static -> DHCP, DHCP -> DHCP
	{
		*len = sprintf((char *)pl_response_buf,"<html><head><title>WIZ550WEB - Configuration</title><body>Reboot Complete. Please try to connect to the IP address assigned by the <span style='color:red;'>DHCP server</span></body></html>");
	}
	else // Static -> Static, DHCP -> Static
	{
		*len = sprintf((char *)pl_response_buf,"<html><head><title>WIZ550WEB - Configuration</title><script language=javascript>j=%d;function func(){document.getElementById('delay').innerText=' '+j+' ';if(j>0)j--;setTimeout('func()',1000);if(j<=0)location.href='http://%d.%d.%d.%d';}</script></head><body onload='func()'>Please wait for a while, the module will boot in<span style='color:red;' id='delay'></span> seconds.</body></html>", delay, url[0], url[1], url[2], url[3]);
	}
	return;
}
