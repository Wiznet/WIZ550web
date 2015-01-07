/**
 * @file	userHandler.h
 * @brief	Header File for User Control Example
 * @version 1.0
 * @date	2014/07/15
 * @par Revision
 *			2014/07/15 - 1.0 Release
 * @author	
 * \n\n @par Copyright (C) 1998 - 2014 WIZnet. All rights reserved.
 */

#ifndef __USERHANDLER_H
#define __USERHANDLER_H

#include <stdint.h>
#include "types.h"

//#define _WEB_DEBUG_

#define MAX_ALIAS_SIZE			25

typedef struct
{
	uint8_t io_statuscode[2];				// Flag for I/O info modification Indicator (initial / saved)
	uint8_t io[16];							// Digital I/O directions (in/out/notused)
	uint8_t ios[16];						// Digital I/O status (high/low)
	uint8_t io_alias[16][MAX_ALIAS_SIZE];	// Digital I/O alias (string < 25)
	uint8_t factory_flag[2];
	uint8_t spiflash_flag[2];
} IOStorage;

extern IOStorage IOdata;

/* Serial configuration index for web page */
// Baudrate
#define BAUD_600_INDEX			0
#define BAUD_1200_INDEX			1
#define BAUD_2400_INDEX			2
#define BAUD_4800_INDEX			3
#define BAUD_9600_INDEX			4
#define BAUD_19200_INDEX		5
#define BAUD_38400_INDEX		6
#define BAUD_57600_INDEX		7
#define BAUD_115200_INDEX		8
#define BAUD_230400_INDEX		9

// Data bit
#define DATA_BIT8_INDEX			0
#define DATA_BIT9_INDEX			1

// Stop bit
#define STOP_BIT1_INDEX			0
#define STOP_BIT2_INDEX			1

// Parity bit 	: Using original setting value
// flow control : Using original setting value

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Public Functions
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void IO_status_init(void);
int read_IOstorage(void *data, uint16_t size);
int write_IOstorage(void *data, uint16_t size);
uint16_t get_ADC_val(uint8_t index);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Pre-defined CGI Interface Functions Handler
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t predefined_get_cgi_processor(uint8_t * uri_name, uint8_t * buf, uint16_t * len);
uint8_t predefined_set_cgi_processor(uint8_t * uri_name, uint8_t * uri, uint8_t * buf, uint16_t * len);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// HTTP GET Method CGI Functions
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void make_basic_config_setting_json_callback(uint8_t * buf, uint16_t * len);
void make_json_io_status(uint8_t * buf, uint16_t * len);
void make_json_adc_status(uint8_t * buf, uint16_t * len);
void make_json_serial_data(uint8_t * buf, uint16_t * len);
void make_json_ioalias_status(uint8_t * buf, uint16_t * len);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// HTTP POST Method CGI Functions
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void make_cgi_basic_config_response_page(uint16_t delay, uint8_t * url, uint8_t * cgi_response_buf, uint16_t * len);
uint8_t * set_basic_config_setting(uint8_t * uri);
uint8_t set_digital_io_output(uint8_t * uri);
uint8_t set_digital_io_alias(uint8_t * uri);
uint8_t set_digital_io_direction(uint8_t * uri);
void set_factory_default_io_status(uint8_t * uri);
void get_serial_data_from_web(uint8_t * uri);

#endif

