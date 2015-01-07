/**
 * @file	httpUtil.c
 * @brief	HTTP Server Utilities	
 * @version 1.0
 * @date	2014/07/15
 * @par Revision
 *			2014/07/15 - 1.0 Release
 * @author	
 * \n\n @par Copyright (C) 1998 - 2014 WIZnet. All rights reserved.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "httpUtil.h"
#include "types.h"
#include "common.h"
#include "socket.h"
#include "userHandler.h"
#include "ConfigData.h"

#ifdef _USE_FLASH_
#include "dataflash.h"
#endif

uint8_t http_get_cgi_handler(uint8_t * uri_name, uint8_t * buf, uint32_t * file_len)
{
	uint8_t ret = HTTP_OK;
	uint16_t len = 0;

	if(predefined_get_cgi_processor(uri_name, buf, &len))
	{
		;
	}
	else if(strcmp((const char *)uri_name, "widget.cgi") == 0)
	{
		make_basic_config_setting_json_callback(buf, &len);
	}
	else if(strcmp((const char *)uri_name, "io.cgi") == 0)
	{
		make_json_io_status(buf, &len);
	}
	else if(strcmp((const char *)uri_name, "io_alias.cgi") == 0)
	{
		make_json_ioalias_status(buf, &len);
	}
	else if(strcmp((const char *)uri_name, "get_serial_data.cgi") == 0)
	{
		make_json_serial_data(buf, &len);
	}
	else if(strcmp((const char *)uri_name, "adc.cgi") == 0)
	{
		make_json_adc_status(buf, &len);
	}
	else
	{
		// CGI file not found
		ret = HTTP_FAILED;
	}

	if(ret)	*file_len = len;

	return ret;
}

uint8_t http_post_cgi_handler(uint8_t * uri_name, st_http_request * p_http_request, uint8_t * buf, uint32_t * file_len)
{
	uint8_t ret = HTTP_OK;
	uint16_t len = 0;
	uint8_t * device_ip;
	uint8_t val;

	if(predefined_set_cgi_processor(uri_name, p_http_request->URI, buf, &len))
	{
		;
	}
	else if(strcmp((const char *)uri_name, "config.cgi") == 0)
	{
		device_ip = set_basic_config_setting(p_http_request->URI);
		make_cgi_basic_config_response_page(5, device_ip, buf, &len);

		ret = HTTP_RESET;
	}
	else if(strcmp((const char *)uri_name, "dout.cgi") == 0)
	{
		val = set_digital_io_output(p_http_request->URI);
		sprintf((char *)buf, "%d", val);
		len = 1;
	}
	else if(strcmp((const char *)uri_name, "save_alias.cgi") == 0)
	{
		val = set_digital_io_alias(p_http_request->URI);
		sprintf((char *)buf, "%d", val);
		len = 1;
	}
	else if(strcmp((const char *)uri_name, "dir_change.cgi") == 0)
	{
		val = set_digital_io_direction(p_http_request->URI);
		sprintf((char *)buf, "%d", val);
		len = 1;
	}
	else if(strcmp((const char *)uri_name, "tx_serial_data.cgi") == 0)
	{
		get_serial_data_from_web(p_http_request->URI);
	}
	else if(strcmp((const char *)uri_name, "set_default_io_status.cgi") == 0)
	{
		set_factory_default_io_status(p_http_request->URI);
		len = 0;
		ret = HTTP_RESET;
	}
	else
	{
		// CGI file not found
		ret = HTTP_FAILED;
	}

	if(ret)	*file_len = len;
	return ret;
}




