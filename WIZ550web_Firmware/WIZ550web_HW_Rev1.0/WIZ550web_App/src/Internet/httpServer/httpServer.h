/**
 @file		httpServer.h
 @brief 	Define constants and functions related HTTP Web server.
 */

#include <stdint.h>

#ifndef	__HTTPSERVER_H__
#define	__HTTPSERVER_H__

//#define _HTTPSERVER_DEBUG_

#define INITIAL_WEBPAGE				"index.html"
#define M_INITIAL_WEBPAGE			"m/index.html"
#define MOBILE_INITIAL_WEBPAGE		"mobile/index.html"

/* Web Server Content Storage Select */
#define _USE_SDCARD_
#ifndef _USE_SDCARD_
#define _USE_FLASH_
#endif

/* Watchdog timer */
#define _USE_WATCHDOG_

/*********************************************
* HTTP Process states list
*********************************************/
#define STATE_HTTP_IDLE             0           /* browser에서 연결된 직후 상태*/
#define STATE_HTTP_REQ_INPROC  		1           /* 최소한 한번 browser로부터 request 메시지를 받은 상태*/
#define STATE_HTTP_REQ_DONE    		2           /* request 메시지의 parsing이 끝난 상태*/
#define STATE_HTTP_RES_INPROC  		3           /* 최소한 한번 response 메시지를 보내고 아직 보낼 데이터가 남은 상태*/
#define STATE_HTTP_RES_DONE    		4           /* response 메시지의 전송이 끝난 상태*/

/*********************************************
* HTTP Simple Return Value
*********************************************/
#define HTTP_FAILED					0
#define HTTP_OK						1
#define HTTP_RESET					2


/*********************************************
* HTTP Content NAME length
*********************************************/
#define MAX_CONTENT_NAME_LEN		128


typedef struct _st_http_socket
{
	uint8_t			sock_status;
	uint8_t			file_name[MAX_CONTENT_NAME_LEN];
	uint32_t 		file_start;
	uint32_t 		file_len;
	uint32_t 		file_offset; // (start addr + sent size...)
}st_http_socket;


void httpServer_init(uint8_t * tx_buf, uint8_t * rx_buf, uint8_t cnt, uint8_t * socklist);
void reg_httpServer_cbfunc(void(*mcu_reset)(void), void(*wdt_reset)(void));
void httpServer_run(uint8_t seqnum);

/*
 * @brief HTTP Server 1sec Tick Timer handler
 * @note SHOULD BE register to your system 1s Tick timer handler
 */
void httpServer_time_handler(void);
uint32_t get_httpServer_timecount(void);

#endif
