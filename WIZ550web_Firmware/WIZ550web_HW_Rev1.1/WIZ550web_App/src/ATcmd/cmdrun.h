/**
 * @file		cmdrun.h
 * @brief		AT Command Module - Implementation Part Header File
 * @version	1.0
 * @date		2013/02/22
 * @par Revision
 *			2013/02/22 - 1.0 Release
 * @author	Mike Jeong
 * \n\n @par Copyright (C) 2013 WIZnet. All rights reserved.
 */

#ifndef	_CMDRUN_H
#define	_CMDRUN_H

#include "library/at_common.h"
#include "ConfigData.h"


#define RET_DONE		2
#define RET_OK_DUMP		3
#define RET_ASYNC		4
#define RET_RECV		5

#define RET_UNSPECIFIED	-2
#define RET_WRONG_OP	-3
#define RET_WRONG_SIGN	-4
#define RET_WRONG_ARG	-5
#define RET_RANGE_OUT	-6
#define RET_DISABLED	-7
#define RET_NOT_ALLOWED	-8
#define RET_BUSY		-9
#define RET_TIMEOUT		-10
#define RET_NO_SOCK		-20
#define RET_SOCK_CLS	-21
#define RET_USING_PORT	-22
#define RET_NOT_CONN	-23
#define RET_WRONG_ADDR	-24
#define RET_NO_DATA		-25
#define RET_NO_FREEMEM	-30

#define ERRVAL_UNSPECIFIED	'0'	// under 10
#define ERRVAL_WRONG_OP		'1'
#define ERRVAL_WRONG_SIGN	'2'
#define ERRVAL_WRONG_ARG	'3'
#define ERRVAL_RANGE_OUT	'4'
#define ERRVAL_DISABLED		'5'
#define ERRVAL_NOT_ALLOWED	'6'
#define ERRVAL_BUSY			'7'
#define ERRVAL_TIMEOUT		'8'
#define ERRVAL_NO_SOCK		'0'	// under 20
#define ERRVAL_SOCK_CLS		'1'
#define ERRVAL_USING_PORT	'2'
#define ERRVAL_NOT_CONN		'3'
#define ERRVAL_WRONG_ADDR	'4'
#define ERRVAL_NO_DATA		'5'
#define ERRVAL_NO_FREEMEM	'0'	// under 30

#define ATC_SOCK_NUM_START	0
#define ATC_SOCK_NUM_END	0
#define ATC_SOCK_NUM_TOTAL	ATC_SOCK_NUM_END - ATC_SOCK_NUM_START + 1
#define ATC_SOCK_AO			1	// Array Offset
//#define WORK_BUF_SIZE		2048
#define EVENT_QUEUE_SIZE	20
#define ATC_VERSION	"1.0"

#define OP_SIZE 8
#define LONG_ARG_SIZE 36
#define SHORT_ARG_SIZE 20
#define ARG_1_SIZE LONG_ARG_SIZE
#define ARG_2_SIZE LONG_ARG_SIZE
#define ARG_3_SIZE SHORT_ARG_SIZE
#define ARG_4_SIZE SHORT_ARG_SIZE
#define ARG_5_SIZE SHORT_ARG_SIZE
#define ARG_6_SIZE SHORT_ARG_SIZE

#define POLL_MODE_NONE 0x0
#define POLL_MODE_SEMI 0x1
#define POLL_MODE_FULL 0x2


typedef struct atct_t {
	int8_t sign;
	int8_t op[OP_SIZE];
	int8_t arg1[ARG_1_SIZE];	// could be SSID
	int8_t arg2[ARG_2_SIZE];
	int8_t arg3[ARG_3_SIZE];
	int8_t arg4[ARG_4_SIZE];
	int8_t arg5[ARG_5_SIZE];
	int8_t arg6[ARG_6_SIZE];
} atct;

struct atc_info {
	wiz_NetInfo net;
	atct tcmd;
#if 1
	int8_t sendsock;
	uint8_t sendip[4];
	uint16_t sendport;
//	int8_t sendbuf[WORK_BUF_SIZE+1];
//	int8_t recvbuf[WORK_BUF_SIZE+1];
	uint8_t *sendbuf;
	uint8_t *recvbuf;
	uint16_t worklen;
	uint16_t sendlen;
	uint16_t recvlen;
#endif
	int8_t echo;
	int8_t mode;		// Reserved
	int8_t poll;
	int8_t country;	// Reserved
};

extern struct atc_info atci;

#define CRITICAL_ERR(fmt) \
	do{printf("### ERROR ### %s(%d): "fmt"\r\n", __FUNCTION__, __LINE__); while(1){__WFI();};}while(0)
#define CRITICAL_ERRA(fmt, ...) \
	do{printf("### ERROR ### %s(%d): "fmt"\r\n", __FUNCTION__, __LINE__, __VA_ARGS__); while(1){__WFI();};}while(0)

extern void cmd_resp_dump(int8_t idval, int8_t *dump);
extern void cmd_resp(int8_t retval, int8_t idval);

void act_nset_q(int8_t num);
void act_nset_a(int8_t mode, uint8_t *ip, uint8_t *sn, uint8_t *gw, uint8_t *dns1, uint8_t *dns2);
void act_nstat(int8_t num);
void act_nmac_q(void);
void act_nmac_a(uint8_t *mac);
void act_mstat(void);
void act_uart_q(USART_TypeDef *pUART, int8_t num);
void act_uart_a(USART_TypeDef *pUART, struct __serial_info *serial);
void act_msave(void);
void act_fiodir(int8_t pin_num, int8_t pin_dir);
void act_fioval(int8_t pin_num, int8_t pin_output);

#endif	//_CMDRUN_H
