/**
 * @file		cmdrun.c
 * @brief		AT Command Module - Implementation Part Source File
 * @version	1.0
 * @date		2013/02/22
 * @par Revision
 *			2013/02/22 - 1.0 Release
 * @author	Mike Jeong
 * \n\n @par Copyright (C) 2013 WIZnet. All rights reserved.
 */

//#define FILE_LOG_SILENCE
#include "stm32f10x.h"
#include "common.h"
#include "cmdrun.h"

#include "boardutil.h"
#include "ConfigData.h"
#include "uartHandler.h"
#include "userHandler.h"
#include "gpioHandler.h"
#include "userHandler.h"

#define SOCK_STAT_TCP_MASK	0x2
#define SOCK_STAT_PROTMASK	0x3
#define SOCK_STAT_IDLE		0x0
#define SOCK_STAT_UDP		0x1
#define SOCK_STAT_TCP_SRV	0x2
#define SOCK_STAT_TCP_CLT	0x3
#define SOCK_STAT_CONNECTED	0x4

#define ARG_CLEAR(arg_p) arg_p[0] = 0;
#define MAKE_TCMD_DIGIT(arg_p, dgt_v)	sprintf((char*)arg_p, "%d", dgt_v)
#define MAKE_TCMD_CHAR(arg_p, char_v)	sprintf((char*)arg_p, "%c", char_v)
#define MAKE_TCMD_ADDR(arg_p, a1_p, a2_p, a3_p, a4_p) \
	sprintf((char*)arg_p, "%d.%d.%d.%d", a1_p, a2_p, a3_p, a4_p)
#define MAKE_TCMD_STRING(arg_p, argsize_v, str_p) do { \
	uint8 len = strlen((char*)str_p); \
	if(len > argsize_v-1) {memcpy((char*)arg_p, (char*)str_p, argsize_v-1); arg_p[argsize_v-1]=0;} \
	else {memcpy((char*)arg_p, (char*)str_p, len); arg_p[len]=0;} \
} while(0)
#define EVENT_RESP(id_v, evt_v)			printf("[V,%d,%d]\r\n", id_v, evt_v)
#define EVENT_RESP_SIZE(id_v, evt_v, size_v) printf("[V,%d,%d,%d]\r\n", id_v, evt_v, size_v)

void act_nset_q(int8_t num)
{
	wiz_NetInfo ni = {{0, },{0, },{0, },{0, },{0, },0};

	ctlnetwork(CN_GET_NETINFO, (void*) &ni);
	if(num == 1) {
		if(ni.dhcp == NETINFO_DHCP) 
			MAKE_TCMD_CHAR(atci.tcmd.arg1, 'D');
		else MAKE_TCMD_CHAR(atci.tcmd.arg1, 'S');
		cmd_resp(RET_OK, VAL_NONE);
		ARG_CLEAR(atci.tcmd.arg1);
		return;
	} else if(num > 1) {
		switch(num) {
		case 2: MAKE_TCMD_ADDR(atci.tcmd.arg1, ni.ip[0], ni.ip[1], ni.ip[2], ni.ip[3]); break;
		case 3: MAKE_TCMD_ADDR(atci.tcmd.arg1, ni.sn[0], ni.sn[1], ni.sn[2], ni.sn[3]); break;
		case 4: MAKE_TCMD_ADDR(atci.tcmd.arg1, ni.gw[0], ni.gw[1], ni.gw[2], ni.gw[3]); break;
		case 5: MAKE_TCMD_ADDR(atci.tcmd.arg1, ni.dns[0], ni.dns[1], ni.dns[2], ni.dns[3]); break;
		case 6: cmd_resp(RET_NOT_ALLOWED, VAL_NONE); return;
		}
		cmd_resp(RET_OK, VAL_NONE);
		ARG_CLEAR(atci.tcmd.arg1);
		return;
	} else {
		if(ni.dhcp == NETINFO_DHCP) 
			MAKE_TCMD_CHAR(atci.tcmd.arg1, 'D');
		else MAKE_TCMD_CHAR(atci.tcmd.arg1, 'S');
		dhcp_get_storage(&ni);
		MAKE_TCMD_ADDR(atci.tcmd.arg2, ni.ip[0], ni.ip[1], ni.ip[2], ni.ip[3]);
		MAKE_TCMD_ADDR(atci.tcmd.arg3, ni.sn[0], ni.sn[1], ni.sn[2], ni.sn[3]);
		MAKE_TCMD_ADDR(atci.tcmd.arg4, ni.gw[0], ni.gw[1], ni.gw[2], ni.gw[3]);
		MAKE_TCMD_ADDR(atci.tcmd.arg5, ni.dns[0], ni.dns[1], ni.dns[2], ni.dns[3]);
		cmd_resp(RET_OK, VAL_NONE);
		ARG_CLEAR(atci.tcmd.arg1);
		ARG_CLEAR(atci.tcmd.arg2);
		ARG_CLEAR(atci.tcmd.arg3);
		ARG_CLEAR(atci.tcmd.arg4);
		ARG_CLEAR(atci.tcmd.arg5);
		return;
	}
}

void act_nset_a(int8_t mode, uint8_t *ip, uint8_t *sn, uint8_t *gw, uint8_t *dns1, uint8_t *dns2)
{
	wiz_NetInfo ni = {{0, },{0, },{0, },{0, },{0, },0};

	get_S2E_Packet(&ni);

	if(ip) memcpy(ni.ip, ip, 4);
	if(sn) memcpy(ni.sn, sn, 4);
	if(gw) memcpy(ni.gw, gw, 4);
	if(dns1) memcpy(ni.dns, dns1, 4);
	if(dns2) {
		MAKE_TCMD_DIGIT(atci.tcmd.arg1, 6);
		cmd_resp(RET_NOT_ALLOWED, VAL_NONE);
		ARG_CLEAR(atci.tcmd.arg1);
		return;
	}

	if(mode == 'S') 
		ni.dhcp = NETINFO_STATIC;
	if(mode == 'D') 
		ni.dhcp = NETINFO_DHCP;

	set_S2E_Packet(&ni);
	Net_Conf();
	cmd_resp(RET_OK, VAL_NONE);
	return;
}

void act_nstat(int8_t num)
{
	wiz_NetInfo ni = {{0, },{0, },{0, },{0, },{0, },0};

	//GetNetInfo(&ni);
	ctlnetwork(CN_GET_NETINFO, (void*) &ni);
	if(num == 1) {
		if(ni.dhcp == NETINFO_DHCP) 
			MAKE_TCMD_CHAR(atci.tcmd.arg1, 'D');
		else 
			MAKE_TCMD_CHAR(atci.tcmd.arg1, 'S');
		cmd_resp(RET_OK, VAL_NONE);
		ARG_CLEAR(atci.tcmd.arg1);
		return;
	} 
	else if(num > 1) {
		switch(num) {
			case 2:
				MAKE_TCMD_ADDR(atci.tcmd.arg1, ni.ip[0], ni.ip[1], ni.ip[2], ni.ip[3]); 
				break;
			case 3: 
				MAKE_TCMD_ADDR(atci.tcmd.arg1, ni.sn[0], ni.sn[1], ni.sn[2], ni.sn[3]); 
				break;
			case 4:
				MAKE_TCMD_ADDR(atci.tcmd.arg1, ni.gw[0], ni.gw[1], ni.gw[2], ni.gw[3]); 
				break;
			case 5:
				MAKE_TCMD_ADDR(atci.tcmd.arg1, ni.dns[0], ni.dns[1], ni.dns[2], ni.dns[3]); 
				break;
			case 6:
				cmd_resp(RET_NOT_ALLOWED, VAL_NONE); 
				return;
		}
		cmd_resp(RET_OK, VAL_NONE);
		ARG_CLEAR(atci.tcmd.arg1);
		return;
	} 
	else {
		if(ni.dhcp == NETINFO_DHCP) 
			MAKE_TCMD_CHAR(atci.tcmd.arg1, 'D');
		else
			MAKE_TCMD_CHAR(atci.tcmd.arg1, 'S');
		MAKE_TCMD_ADDR(atci.tcmd.arg2, ni.ip[0], ni.ip[1], ni.ip[2], ni.ip[3]);
		MAKE_TCMD_ADDR(atci.tcmd.arg3, ni.sn[0], ni.sn[1], ni.sn[2], ni.sn[3]);
		MAKE_TCMD_ADDR(atci.tcmd.arg4, ni.gw[0], ni.gw[1], ni.gw[2], ni.gw[3]);
		MAKE_TCMD_ADDR(atci.tcmd.arg5, ni.dns[0], ni.dns[1], ni.dns[2], ni.dns[3]);
		cmd_resp(RET_OK, VAL_NONE);
		ARG_CLEAR(atci.tcmd.arg1);
		ARG_CLEAR(atci.tcmd.arg2);
		ARG_CLEAR(atci.tcmd.arg3);
		ARG_CLEAR(atci.tcmd.arg4);
		ARG_CLEAR(atci.tcmd.arg5);
		return;
	}
}

void act_nmac_q(void)
{
	wiz_NetInfo ni = {{0, },{0, },{0, },{0, },{0, },0};

	ctlnetwork(CN_GET_NETINFO, (void*) &ni);
	sprintf((char*)atci.tcmd.arg1, "%02x:%02x:%02x:%02x:%02x:%02x", 
		ni.mac[0], ni.mac[1], ni.mac[2], ni.mac[3], ni.mac[4], ni.mac[5]);
	cmd_resp(RET_OK, VAL_NONE);
}

void act_nmac_a(uint8_t *mac)
{
#if 1 // Enable MAC change
	wiz_NetInfo ni = {{0, },{0, },{0, },{0, },{0, },0};

	set_mac(mac);
	//save_S2E_Packet_to_eeprom();
	save_S2E_Packet_to_storage();

	ctlnetwork(CN_GET_NETINFO, (void*) &ni);
	memcpy(ni.mac, mac, 6);
	ctlnetwork(CN_SET_NETINFO, (void*) &ni);
	cmd_resp(RET_OK, VAL_NONE);
#else
	cmd_resp(RET_NOT_ALLOWED, VAL_NONE);
#endif
}

void act_mstat(void)
{
	sprintf((char *)(atci.tcmd.arg1), "%d.%d.%d", MAJOR_VER, MINOR_VER, MAINTENANCE_VER);
	cmd_resp(RET_OK, VAL_NONE);
	ARG_CLEAR(atci.tcmd.arg1);
}

char parity_char[] = {
	'N', 'O', 'E'
};
void act_uart_q(USART_TypeDef *pUART, int8_t num)
{
	struct __serial_info *serial;
	if(pUART == USART1)
		serial = (struct __serial_info *)get_S2E_Packet_pointer()->serial_info;
	else
		serial = ((struct __serial_info *)get_S2E_Packet_pointer()->serial_info) + 1;

	if(num >= 1) {
		switch(num) {
			case 1:
				sprintf((char*)atci.tcmd.arg1, "%ld", serial->baud_rate);
				//MAKE_TCMD_DIGIT(atci.tcmd.arg1, serial->baud_rate);
				break;
			case 2:
				MAKE_TCMD_DIGIT(atci.tcmd.arg1, serial->data_bits);
				break;
			case 3:
				MAKE_TCMD_CHAR(atci.tcmd.arg1, parity_char[serial->parity]);
				break;
			case 4:
				MAKE_TCMD_DIGIT(atci.tcmd.arg1, serial->stop_bits);
				break;
			case 5:
				MAKE_TCMD_DIGIT(atci.tcmd.arg1, serial->flow_control);
				break;
			default:
				cmd_resp(RET_NOT_ALLOWED, VAL_NONE); 
				break;
		}
		cmd_resp(RET_OK, VAL_NONE);
		ARG_CLEAR(atci.tcmd.arg1);
		return;
	} else {
		sprintf((char*)atci.tcmd.arg1, "%ld", serial->baud_rate);
		//MAKE_TCMD_DIGIT(atci.tcmd.arg1, serial->baud_rate);
		MAKE_TCMD_DIGIT(atci.tcmd.arg2, serial->data_bits);
		MAKE_TCMD_CHAR(atci.tcmd.arg3, parity_char[serial->parity]);
		MAKE_TCMD_DIGIT(atci.tcmd.arg4, serial->stop_bits);
		MAKE_TCMD_DIGIT(atci.tcmd.arg5, serial->flow_control);
		cmd_resp(RET_OK, VAL_NONE);
		ARG_CLEAR(atci.tcmd.arg1);
		ARG_CLEAR(atci.tcmd.arg2);
		ARG_CLEAR(atci.tcmd.arg3);
		ARG_CLEAR(atci.tcmd.arg4);
		ARG_CLEAR(atci.tcmd.arg5);
	}
}

void act_uart_a(USART_TypeDef *pUART, struct __serial_info *serial)
{
	cmd_resp(RET_OK, VAL_NONE);
	while(UART_TxRB_GetCount() != 0);
	Delay(999999);
	serial_info_init(pUART, serial);
}

void act_msave(void)
{
	extern int write_IOstorage(void *data, uint16_t size);
	save_S2E_Packet_to_storage();
	write_IOstorage(&IOdata, sizeof(IOdata));
	cmd_resp(RET_OK, VAL_NONE);
}

void act_fiodir(int8_t pin_num, int8_t pin_dir)
{
	pin_num--;

	if(pin_dir == -1) {
		switch(IOdata.io[pin_num]) {
		case Input:
		case Output:
			MAKE_TCMD_DIGIT(atci.tcmd.arg1, IOdata.io[pin_num]);
			break;

		default:
			MAKE_TCMD_DIGIT(atci.tcmd.arg1, 0);
			break;
		}
	} else {
		IOdata.io[pin_num] = pin_dir;
		IO_Init(pin_num);
	}
	cmd_resp(RET_OK, VAL_NONE);
}

void act_fioval(int8_t pin_num, int8_t pin_output)
{
	pin_num--;

	if(pin_output == -1)
		MAKE_TCMD_DIGIT(atci.tcmd.arg1, get_IO_Status(pin_num));
	else {
		if(IOdata.io[pin_num] == Output) {
			IOdata.ios[pin_num] = pin_output;

			if(pin_output)
				IO_On(pin_num);
			else
				IO_Off(pin_num);
		} else {
			cmd_resp(RET_NOT_ALLOWED, VAL_NONE); 
		}
	}
	
	cmd_resp(RET_OK, VAL_NONE);
}
