/**
 * @file		sockutil.h
 * @brief		Socket Utility Header File
 * @version	1.0
 * @date		2013/02/22
 * @par Revision
 *			2013/02/22 - 1.0 Release
 * @author	modified by Mike Jeong
 * \n\n @par Copyright (C) 2013 WIZnet. All rights reserved.
 */

#ifndef _SOCKUTIL_H
#define _SOCKUTIL_H

#include "at_common.h"

/**
 * @addtogroup sockwatch_module
 * @{
 * @def WATCH_SOCK_UDP_SEND
 * Indicate that 'UDP SEND' completion of this socket has to be watched.
 * @def WATCH_SOCK_TCP_SEND
 * Indicate that 'TCP SEND' completion of this socket has to be watched.
 * @def WATCH_SOCK_CONN_TRY
 * Indicate that 'CONNECT' completion of this socket has to be watched.
 * @def WATCH_SOCK_CONN_EVT
 * Indicate that 'CONNECT' event of this socket has to be watched.
 * @def WATCH_SOCK_CLS_TRY
 * Indicate that 'CLOSE' completion of this socket has to be watched.
 * @def WATCH_SOCK_CLS_EVT
 * Indicate that 'CLOSE' event of this socket has to be watched.
 * @def WATCH_SOCK_RECV
 * Indicate that 'RECEIVE' event of this socket has to be watched.
 * @def WATCH_SOCK_MASK_LOW
 * Mask all Completions of the socket.
 * @def WATCH_SOCK_MASK_HIGH
 * Mask all Events of the socket.
 * @def WATCH_SOCK_ALL_MASK
 * Mask all Completions and Events.
 * @typedef watch_cbfunc
 * Watch call back function form.
 * @}
 */
#define WATCH_SOCK_UDP_SEND		0x01
#define WATCH_SOCK_TCP_SEND 	0x02
#define WATCH_SOCK_CONN_TRY		0x04
#define WATCH_SOCK_CLS_TRY		0x08
#define WATCH_SOCK_CONN_EVT		0x10
#define WATCH_SOCK_CLS_EVT		0x20
#define WATCH_SOCK_RECV			0x40
#define WATCH_SOCK_MASK_LOW		0x0F
#define WATCH_SOCK_MASK_HIGH	0x70
#define WATCH_SOCK_ALL_MASK		0x7F

typedef void (*watch_cbfunc)(uint8_t id, uint8_t item, int32_t ret);

int8_t sockwatch_open(uint8_t sock, watch_cbfunc cb);
int8_t sockwatch_close(uint8_t sock);
int8_t sockwatch_set(uint8_t sock, uint8_t item);
int8_t sockwatch_clr(uint8_t sock, uint8_t item);
int8_t sockwatch_chk(uint8_t sock, uint8_t item);
void sockwatch_run(void);

int8_t ip_check(int8_t *str, uint8_t *ip);
int8_t port_check(int8_t *str, uint16_t *port);
int8_t mac_check(int8_t *str, uint8_t *mac);

#endif	//_SOCKUTIL_H
