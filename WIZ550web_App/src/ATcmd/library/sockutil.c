/**
 * @file		sockutil.c
 * @brief		Socket Utility Source File
 * @version	1.0
 * @date		2013/02/22
 * @par Revision
 *			2013/02/22 - 1.0 Release
 * @author	modified by Mike Jeong
 * \n\n @par Copyright (C) 2013 WIZnet. All rights reserved.
 */

//#define FILE_LOG_SILENCE
#include "at_common.h"

static watch_cbfunc watch_cb[TOTAL_SOCK_NUM] = {0,};
static uint8_t watch_sock[TOTAL_SOCK_NUM] = {0,};


/**
 * Assign a callback function to a socket.
 * When @ref sockwatch_run function detected a event, \n
 * this callback function will be called.
 * 
 * @ingroup sockwatch_module
 * @param sock The socket number which is corresponding to 'cb' param
 * @param cb The callback function to be called when \n 
 * 		the socket has any completion or event.
 * @return RET_OK: Success
 * @return RET_NOK: Error
 */
int8_t sockwatch_open(uint8_t sock, watch_cbfunc cb)
{
	DBGA("WATCH Open - sock(%d), CB(%p)", sock, (void*)cb);
	if(cb == NULL || sock >= TOTAL_SOCK_NUM) {
		ERRA("wrong arg - sock(%d)", sock);
		return RET_NOK;
	}
	if(watch_cb[sock] == NULL) watch_cb[sock] = cb;
	else return RET_NOK;

	return RET_OK;
}

/**
 * Remove callback function from a socket and Stop to watch all event.
 *
 * @ingroup sockwatch_module
 * @param sock The socket number to close
 * @return RET_OK: Success
 * @return RET_NOK: Error
 */
int8_t sockwatch_close(uint8_t sock)
{
	DBGA("WATCH Close - sock(%d)", sock);
	if(sock >= TOTAL_SOCK_NUM) {
		ERRA("wrong sock(%d)", sock);
		return RET_NOK;
	}

	sockwatch_clr(sock, WATCH_SOCK_ALL_MASK);
	watch_cb[sock] = NULL;

	return RET_OK;
}

/**
 * Set a item of event to watch at @ref sockwatch_run.
 * Once opened a socket by @ref sockwatch_open, \n
 * you have to register events for watching that using this function.
 * It is possible to set plural item of event at the same time.
 *
 * @ingroup sockwatch_module
 * @param sock The socket number to watch
 * @param item The item of event to watch
 * @return RET_OK: Success
 * @return RET_NOK: Error
 */
int8_t sockwatch_set(uint8_t sock, uint8_t item)
{
	DBGA("WATCH Set - sock(%d), item(0x%x)", sock, item);
	if(sock >= TOTAL_SOCK_NUM) {
		ERRA("wrong sock(%d)", sock);
		return RET_NOK;
	}

	BITSET(watch_sock[sock], 0x7F & item);

	return RET_OK;
}

/**
 * Clear a item of event which you don't care anymore.
 * @ref sockwatch_run stop to detect the item on the socket.
 * It is possible to set plural item of event at the same time.
 *
 * @ingroup sockwatch_module
 * @param sock The socket number to clear
 * @param item The item of event to clear
 * @return RET_OK: Success
 * @return RET_NOK: Error
 */
int8_t sockwatch_clr(uint8_t sock, uint8_t item)
{
	DBGA("WATCH Clear - sock(%d), item(0x%x)", sock, item);
	if(sock >= TOTAL_SOCK_NUM) {
		ERRA("wrong sock(%d)", sock);
		return RET_NOK;
	}

	BITCLR(watch_sock[sock], 0x7F & item);

	return RET_OK;
}

/**
 * Check a item of event has been set on the socket.
 *
 * @ingroup sockwatch_module
 * @param sock The socket number to clear
 * @param item The item of event to clear
 * @return RET_OK: There is the item queried on the socket.
 * @return RET_NOK: There is not the item queried on the socket.
 */
int8_t sockwatch_chk(uint8_t sock, uint8_t item)
{
	if((sock < TOTAL_SOCK_NUM) && (watch_sock[sock] & item)) 
		return RET_OK;

	return RET_NOK;
}

/**
 * Sockwatch Module Handler
 * If you use Sockwatch Module, this should run in the main loop
 * @ingroup sockwatch_module
 */
void sockwatch_run(void)
{
#define WCF_HANDLE(item_v, ret_v) \
do { \
	BITCLR(watch_sock[i], item_v); \
	watch_cb[i](i, item_v, ret_v); \
} while(0)

	uint8_t i;
	int32_t ret;

	for(i=0; i<TOTAL_SOCK_NUM; i++) {
		if(watch_sock[i] == 0) continue;
		if(watch_sock[i] & WATCH_SOCK_RECV) {	// checked every time when 'connected' state
			getsockopt(i, SO_RECVBUF, (uint16_t*)&ret);
			if((uint16_t)ret > 0) WCF_HANDLE(WATCH_SOCK_RECV, RET_OK);
		}
		if(watch_sock[i] & WATCH_SOCK_CLS_EVT) {	// checked every time when 'connected' state
			getsockopt(i, SO_STATUS, (uint8_t*)&ret);
			switch((uint8_t)ret) {
			case SOCK_CLOSED:
				WCF_HANDLE(WATCH_SOCK_CLS_EVT, RET_OK);
				break;
			case SOCK_CLOSE_WAIT:
				disconnect(i);
				//close(i);
				break;
			default:
				break;
			}
		}
		if(watch_sock[i] & WATCH_SOCK_CONN_EVT) {	// checked every time when 'listen' state
			getsockopt(i, SO_STATUS, (uint8_t*)&ret);
			switch((uint8_t)ret) {
			case SOCK_ESTABLISHED:
				WCF_HANDLE(WATCH_SOCK_CONN_EVT, RET_OK);
				break;
			default:
				break;
			}
		}
		if((watch_sock[i] & WATCH_SOCK_MASK_LOW) == 0) continue;	// things which occurs occasionally will be checked all together
		if(watch_sock[i] & WATCH_SOCK_CLS_TRY) {
			getsockopt(i, SO_STATUS, (uint8_t*)&ret);
			switch((uint8_t)ret) {
			case SOCK_LISTEN:
				close(i);
			case SOCK_CLOSED:
				WCF_HANDLE(WATCH_SOCK_CLS_TRY, RET_OK);
				break;
			case SOCK_FIN_WAIT:
				close(i);	
				break;
			default:
				ctlsocket(i, CS_GET_INTERRUPT, (uint8_t*)&ret);
				if((uint8_t)ret & Sn_IR_TIMEOUT){
					//ctlsocket(i, CS_CLR_INTERRUPT, (uint8_t*)&ret);
					close(i);
					WCF_HANDLE(WATCH_SOCK_CLS_TRY, SOCKERR_TIMEOUT);
				}
				disconnect(i);
				//close(i);
				break;
			}
		}
		if(watch_sock[i] & WATCH_SOCK_CONN_TRY) {
			getsockopt(i, SO_STATUS, (uint8_t*)&ret);
			switch((uint8_t)ret) {
			case SOCK_ESTABLISHED:
				WCF_HANDLE(WATCH_SOCK_CONN_TRY, RET_OK);
				break;
			default:
				ctlsocket(i, CS_GET_INTERRUPT, (uint8_t*)&ret);
				if((uint8_t)ret & Sn_IR_TIMEOUT){
					//ctlsocket(i, CS_CLR_INTERRUPT, (uint8_t*)&ret);
					close(i);
					WCF_HANDLE(WATCH_SOCK_CONN_TRY, SOCKERR_TIMEOUT);
				}
				break;
			}
		}
		if(watch_sock[i] & WATCH_SOCK_TCP_SEND) {
			// 블로킹 모드로만 동작함 그러므로 Watch할 필요가 없음
		}
		if(watch_sock[i] & WATCH_SOCK_UDP_SEND) {
			ctlsocket(i, CS_GET_INTERRUPT, (uint8_t*)&ret);
			if((uint8_t)ret & Sn_IR_SENDOK) {
				uint8_t set = (uint8_t)Sn_IR_SENDOK;
				ctlsocket(i, CS_CLR_INTERRUPT, &set);
				WCF_HANDLE(WATCH_SOCK_UDP_SEND, RET_OK);
			}
			else if((uint8_t)ret & Sn_IR_TIMEOUT) {
				uint8_t set = Sn_IR_TIMEOUT;
				ctlsocket(i, CS_CLR_INTERRUPT, &set);
				WCF_HANDLE(WATCH_SOCK_UDP_SEND, RET_NOK);
			}
		}
	}

	// ToDo: not socket part
	
}

/**
 * Check a string is right IP Address, and if right, copy the address to the 'ip' variable as array.
 *
 * @param str The string to investigate if it is right IP Address.
 * @param ip The array pointer in which the address will enter when it is right IP address.
 * @return RET_OK: This is right IP Address.
 * @return RET_NOK: This is not IP Address.
 */
int8_t ip_check(int8_t *str, uint8_t *ip)
{
	uint8_t cnt=0;
	int8_t tmp[16], *split;
	int32_t digit, sumchk = 0;

	digit = strlen((char*)str);
	if(digit > 15 || digit < 7) {
		return RET_NOK;
	}

	strcpy((char*)tmp, (char*)str);
	split = (int8_t*)strtok((char*)tmp, ".");
	while(split != NULL && str_check(isdigit, split) == RET_OK) {
		digit = atoi((char*)split);
		if(digit > 255 || digit < 0) return RET_NOK;
		if(ip) ip[cnt] = digit;
		sumchk += digit;
		cnt++;
		split = (int8_t*)strtok(NULL, ".");
	}

	if(cnt != 4 || sumchk == 0) {		//printf("not 4 digit (%d)\r\n", cnt);
		return RET_NOK;
	}

	return RET_OK;
}

/**
 * Check a string is right TCP Port number, and if right, copy the number to the 'port' variable.
 *
 * @param str The string to investigate if it is right TCP Port.
 * @param port The variable pointer in which the number will enter when it is right TCP Port.
 * @return RET_OK: This is right TCP Port number.
 * @return RET_NOK: This is not TCP Port number.
 */
int8_t port_check(int8_t *str, uint16_t *port)
{
	int8_t *ptr;
	uint32_t val;

	val = strtol((char*)str, (char**)&ptr, 10);		//printf("ptr(%p, %x), arg(%p), val(%d)\r\n", ptr, *ptr, str, val);

	if(val == 0 || val > 65535 || *ptr != 0) return RET_NOK;
	if(port) *port = val;

	return RET_OK;
}

/**
 * Check a string is right MAC Address, and if right, copy the address to the 'mac' variable as array.
 *
 * @param str The string to investigate if it is right MAC Address.
 * @param mac The array pointer in which the address will enter when it is right MAC address.
 * @return RET_OK: This is right MAC Address.
 * @return RET_NOK: This is not MAC Address.
 */
int8_t mac_check(int8_t *str, uint8_t *mac)
{
	uint8_t cnt=0;
	int8_t tmp[18], *split;
	int32_t digit;

	if(strlen((char*)str) != 17) {
		return RET_NOK;
	}

	strcpy((char*)tmp, (char*)str);
	split = (int8_t*)strtok((char*)tmp, ":");
	while(split != NULL && str_check(isxdigit, split) == RET_OK) {
		digit = strtol((char*)split, NULL, 16);
		if(digit > 255 || digit < 0) return RET_NOK;
		if(mac) mac[cnt] = digit;
		cnt++;
		split = (int8_t*)strtok(NULL, ":");
	}

	if(cnt != 6) {		//printf("not 6 digit (%d)\r\n", cnt);
		return RET_NOK;
	}

	return RET_OK;
}
