/**
 * @file		atcmd.c
 * @brief		AT Command Module - Interface Part Source File
 * @version	1.0
 * @date		2013/02/22
 * @par Revision
 *			2013/02/22 - 1.0 Release
 * @author	Mike Jeong
 * \n\n @par Copyright (C) 2013 WIZnet. All rights reserved.
 */

//#define FILE_LOG_SILENCE
#include <stdio.h>
#include <string.h>	
#include <stdlib.h>
#include <ctype.h>
#include "stm32f10x.h"
#include "atcmd.h"
#include "cmdrun.h"
#include "library/util.h"

#include "ConfigData.h"
#include "uarthandler.h"
#include "boardutil.h"

#define CMD_CLEAR() { \
	atci.tcmd.op[0] =   atci.tcmd.sign =    atci.tcmd.arg1[0] = atci.tcmd.arg2[0] = 0; \
	atci.tcmd.arg3[0] = atci.tcmd.arg4[0] = atci.tcmd.arg5[0] = atci.tcmd.arg6[0] = 0; \
}
#define RESP_CR(type_v) do{CMD_CLEAR(); cmd_resp(type_v, VAL_NONE); return;}while(0)
#define RESP_CDR(type_v, dgt_v) do{ \
	CMD_CLEAR(); sprintf((char*)atci.tcmd.arg1, "%d", dgt_v); \
	cmd_resp(type_v, VAL_NONE); return; \
}while(0)

#define CMP_CHAR_1(str_p, c1_v) \
	(str_p[1] != 0 || (str_p[0]=toupper(str_p[0]))!=c1_v)
#define CMP_CHAR_2(str_p, c1_v, c2_v) \
	(str_p[1] != 0 || ((str_p[0]=toupper(str_p[0]))!=c1_v && str_p[0]!=c2_v))
#define CMP_CHAR_3(str_p, c1_v, c2_v, c3_v) \
	(str_p[1] != 0 || ((str_p[0]=toupper(str_p[0]))!=c1_v && str_p[0]!=c2_v && str_p[0]!=c3_v))
#define CMP_CHAR_4(str_p, c1_v, c2_v, c3_v, c4_v) \
	(str_p[1] != 0 || ((str_p[0]=toupper(str_p[0]))!=c1_v && str_p[0]!=c2_v && str_p[0]!=c3_v && str_p[0]!=c4_v))
#define CHK_DGT_RANGE(str_p, snum_v, minval_v, maxval_v) \
	((snum_v=atoi((char*)str_p))>maxval_v || snum_v<minval_v)
#define CHK_ARG_LEN(arg_p, maxlen_v, ret_v) { \
	if(maxlen_v == 0) { \
		if(arg_p[0] != 0) RESP_CDR(RET_WRONG_ARG, ret_v); \
	} else if(strlen((char*)arg_p) > maxlen_v) RESP_CDR(RET_WRONG_ARG, ret_v); \
}

uint8_t g_send_buf[WORK_BUF_SIZE+1];
uint8_t g_recv_buf[WORK_BUF_SIZE+1];
extern uint32_t baud_table[11];

static void cmd_set_prev(uint8_t buflen);
static int8_t cmd_divide(int8_t *buf);
static void cmd_assign(void);

static void hdl_nset(void);
static void hdl_nstat(void);
static void hdl_nmac(void);
static void hdl_mstat(void);
static void hdl_musart1(void);
static void hdl_musart2(void);
static void hdl_msave(void);
static void hdl_mrst(void);
static void hdl_fiodir(void);
static void hdl_fioval(void);

#define ATCMD_BUF_SIZE		100
#define PREVBUF_MAX_SIZE	250
#define PREVBUF_MAX_NUM		2
#define PREVBUF_LAST		(PREVBUF_MAX_NUM-1)
#define CMD_SIGN_NONE		0
#define CMD_SIGN_QUEST		1
#define CMD_SIGN_INDIV		2
#define CMD_SIGN_EQUAL		3


const struct at_command g_cmd_table[] =
{
	{ "NSET",		hdl_nset, 		NULL, NULL },
	{ "NSTAT",		hdl_nstat, 		NULL, NULL },
	{ "NMAC",		hdl_nmac, 		NULL, NULL },

	{ "MSTAT",		hdl_mstat,	 	NULL, NULL },
	{ "MUSART1",	hdl_musart1, 	NULL, NULL },
	{ "MUSART2",	hdl_musart2, 	NULL, NULL },
	{ "MSAVE",		hdl_msave,	 	NULL, NULL },
	{ "MRST",		hdl_mrst,	 	NULL, NULL },

	{ "FIODIR",		hdl_fiodir,	 	NULL, NULL },
	{ "FIOVAL",		hdl_fioval,	 	NULL, NULL },

	{ NULL, NULL, NULL, NULL }	// Should be last item
};

static int8_t termbuf[ATCMD_BUF_SIZE];
static int8_t *prevbuf[PREVBUF_MAX_NUM];
static uint8_t previdx = 0, prevcnt = 0;
static int16_t prevlen = 0;

struct atc_info atci;

/**
 * @ingroup atcmd_module
 * Initialize ATCMD Module.
 * This should be called before @ref atc_run
 */
void atc_init()
{
	int8_t i;

	memset(termbuf, 0, ATCMD_BUF_SIZE);
	for(i=0; i<PREVBUF_MAX_NUM; i++) prevbuf[i] = NULL;
	atci.sendsock = VAL_NONE;
	atci.echo = VAL_ENABLE;
	atci.poll = POLL_MODE_SEMI;

	atci.sendbuf = g_send_buf;
	atci.recvbuf = g_recv_buf;

//	sockwatch_open(0, atc_async_cb);	// Assign socket 1 to ATC module

//	UART_write("\r\n\r\n\r\n[W,0]\r\n", 13);
//	UART_write("[S,0]\r\n", 7);
}

/**
 * @ingroup atcmd_module
 * ATCMD Module Handler.
 * If you use ATCMD Module, this should run in the main loop
 */
void atc_run(void)
{
	int8_t ret, recv_char;
	static uint8_t buflen = 0;

	if(UART_read(&recv_char, 1) <= 0) return; // �엯�젰 媛� �뾾�뒗 寃쎌??	printf("RECV: 0x%x\r\n", recv_char);

#if 0	// 2014.09.03 
	if(atci.sendsock != VAL_NONE)
	{
		atci.sendbuf[atci.worklen++] = recv_char;
		if(atci.worklen >= atci.sendlen) { // �엯�젰�씠 �셿?�뚮릺硫�?
			act_nsend(atci.sendsock, (int8_t *)atci.sendbuf, atci.worklen, atci.sendip, &atci.sendport);
			atci.sendsock = VAL_NONE;
		}
		return;
	}
#endif

	if(isgraph(recv_char) == 0)	// �젣�뼱 ?�몄??泥섎??
	{	//printf("ctrl\r\n");
		switch(recv_char) {
			case 0x0d:	// CR(\r)
#if defined(FACTORY_FW)
				check_RS422((uint8_t *)termbuf);
#endif
				break;	//		do nothing
			case 0x0a:	// LF(\n)
				//printf("<ENT>");
				if(atci.echo)
				{
					UART_write(termbuf, buflen);
					UART_write("\r\n", 2);
				}
				termbuf[buflen] = 0;
				break;
			case 0x08:	// BS
				//printf("<BS>\r\n");
				if(buflen != 0) {
					buflen--;
					termbuf[buflen] = 0;
					if(atci.echo) 
						UART_write("\b \b", 3);
				}
				break;
			case 0x1b:	// ESC
				//printf("<ESC>\r\n");
				if(atci.echo) 
					UART_write(&recv_char, 1);
				break;
		}

	}
	else if(buflen < ATCMD_BUF_SIZE-1)		// -1 �씠��? : 0 �씠 �븯�굹 �븘�슂�븯誘�濡�
	{
		if(buflen == 0)	UART2_flush();
		termbuf[buflen++] = (uint8_t)recv_char;	//termbuf[buflen] = 0;
		//if(atci.echo) UART_write(&recv_char, 1);
		//printf(" termbuf(%c, %s)\r\n", recv_char, termbuf);
	}
	//else { printf("input buffer stuffed\r\n"); }

	if(recv_char != 0x0a || buflen == 0) 
		return; 	//LOGA("Command: %d, %s\r\n", buflen, termbuf);

	cmd_set_prev(buflen);
	buflen = 0;

	CMD_CLEAR();
	ret = cmd_divide(termbuf);
	if(ret == RET_OK) {
		cmd_assign();
	} 
	else if(ret != RET_DONE) {
		cmd_resp(ret, VAL_NONE);
	}
}

static void cmd_set_prev(uint8_t buflen)
{
	int8_t idx;

	while(prevcnt >= PREVBUF_MAX_NUM || (prevcnt && prevlen+buflen > PREVBUF_MAX_SIZE-1)) {
		idx = (previdx + PREVBUF_MAX_NUM - prevcnt) % PREVBUF_MAX_NUM;	// oldest index
		if(prevbuf[idx]) {
			prevlen -= strlen((char*)prevbuf[idx]) + 1;
			free(prevbuf[idx]);
			prevbuf[idx] = NULL;
			prevcnt--;
		} else CRITICAL_ERR("ring buf 1");
	}

	prevbuf[previdx] = malloc(buflen+1);

	while(prevcnt && prevbuf[previdx] == NULL) {
		idx = (previdx + PREVBUF_MAX_NUM - prevcnt) % PREVBUF_MAX_NUM;	// oldest index
		if(prevbuf[idx]) {
			prevlen -= strlen((char*)prevbuf[idx]) + 1;
			free(prevbuf[idx]);
			prevbuf[idx] = NULL;
			prevcnt--;
			prevbuf[previdx] = malloc(buflen+1);
		} else CRITICAL_ERR("ring buf 2");
	}

	if(prevbuf[previdx] == NULL) CRITICAL_ERR("malloc fail");	//  留뚯�?�떎�뙣�빐�룄 嫄� �븯?�� �떢�쑝硫� �닔�젙
	else {
		strcpy((char*)prevbuf[previdx], (char*)termbuf);	//printf("$$%s## was set\r\n", prevbuf[previdx]);
		if(previdx == PREVBUF_LAST) previdx = 0;
		else previdx++;
		prevcnt++;
		prevlen += buflen + 1;
	}
}

static int8_t cmd_divide(int8_t *buf)
{
	int8_t ret, *split, *noteptr, *tmpptr = buf;					//printf("cmd_divide 1 \r\n");

	if(strchr((char*)tmpptr, '=')) atci.tcmd.sign = CMD_SIGN_EQUAL;
	else if(strchr((char*)tmpptr, '?')) atci.tcmd.sign = CMD_SIGN_QUEST;
	else if(strchr((char*)tmpptr, '-')) atci.tcmd.sign = CMD_SIGN_INDIV;

	split = strsep_ex(&tmpptr, (int8_t *)"=-?");
	if(split != NULL)
	{
		for (noteptr = split; *noteptr; noteptr++) *noteptr = toupper(*noteptr);
		if(strlen((char*)split) > OP_SIZE+3-1 || 
			split[0] != 'A' || split[1] != 'T' || split[2] != '+')
		{
			if(split[0] == 'A' && split[1] == 'T' && split[2] == 0) {	// Just 'AT' input
				if(atci.tcmd.sign == CMD_SIGN_QUEST) {
					if(prevcnt < 2) UART_write("[D,,0]\r\n", 8);
					else {
						uint8_t idx = previdx;
						uint8_t tmplen;
						char tmpbuf[ATCMD_BUF_SIZE];
						if(previdx < 2) idx += PREVBUF_MAX_NUM - 2; //printf("==%d,%d==", previdx, idx);}
						else idx -= 2; 						//printf("++%d,%d++", previdx, idx);}printf("--%d--", idx);Delay_ms(5);
						//printf("[D,,%d]\r\n%s\r\n", strlen((char*)prevbuf[idx]), prevbuf[idx]);
						tmplen = sprintf(tmpbuf, "[D,,%d]\r\n%s\r\n", strlen((char*)prevbuf[idx]), prevbuf[idx]);
						UART_write(tmpbuf, tmplen);
					}
				}
				else if(atci.tcmd.sign == CMD_SIGN_NONE) UART_write("[S]\r\n", 5);
				else return RET_WRONG_SIGN;
				return RET_DONE;
			} else {
				strcpy((char*)atci.tcmd.op, (char*)split);
			}
		} else {
			strcpy((char*)atci.tcmd.op, (char*)&split[3]);
		}
	}
	else return RET_WRONG_OP;			//printf("first splite is NULL\r\n");

#define ARG_PARSE(arg_v, size_v, dgt_v) \
{ \
	split = strsep_ex(&tmpptr, (int8_t *)","); \
	if(split != NULL) { \
		if(strlen((char*)split) > size_v-1) { \
			ret = RET_WRONG_ARG; \
			CMD_CLEAR(); \
			sprintf((char*)atci.tcmd.arg1, "%d", dgt_v); \
			goto FAIL_END; \
		} else strcpy((char*)arg_v, (char*)split); \
	} else goto OK_END; \
} \

	ARG_PARSE(atci.tcmd.arg1, ARG_1_SIZE, 1);
	ARG_PARSE(atci.tcmd.arg2, ARG_2_SIZE, 2);
	ARG_PARSE(atci.tcmd.arg3, ARG_3_SIZE, 3);
	ARG_PARSE(atci.tcmd.arg4, ARG_4_SIZE, 4);
	ARG_PARSE(atci.tcmd.arg5, ARG_5_SIZE, 5);
	ARG_PARSE(atci.tcmd.arg6, ARG_6_SIZE, 6);
	if(*tmpptr != 0) {
		ret = RET_WRONG_ARG;
		CMD_CLEAR();
		goto FAIL_END;
	}
	DBGA("Debug: (%s)", tmpptr);	//理쒕�?arg�꽆寃� �뱾�뼱�삩 寃� �솗�씤�슜 - Strict Param �젙�?��

OK_END:
	ret = RET_OK;
FAIL_END:
	DBGA("[%s] S(%d),OP(%s),A1(%s),A2(%s),A3(%s),A4(%s),A5(%s),A6(%s)", 
		ret==RET_OK?"OK":"ERR", atci.tcmd.sign, atci.tcmd.op?atci.tcmd.op:"<N>", atci.tcmd.arg1?atci.tcmd.arg1:"<N>", 
		atci.tcmd.arg2?atci.tcmd.arg2:"<N>", atci.tcmd.arg3?atci.tcmd.arg3:"<N>", atci.tcmd.arg4?atci.tcmd.arg4:"<N>", 
		atci.tcmd.arg5?atci.tcmd.arg5:"<N>", atci.tcmd.arg6?atci.tcmd.arg6:"<N>");
	return ret;
}

static void cmd_assign(void)
{
	int i;

	for(i = 0 ; g_cmd_table[i].cmd != NULL ; i++) {
		if(!strcmp((const char *)atci.tcmd.op, g_cmd_table[i].cmd)) {
			g_cmd_table[i].process();
			return;
		}
	}

	/* CMD NOT FOUND */
	CMD_CLEAR();
	cmd_resp(RET_WRONG_OP, VAL_NONE);
}

void cmd_resp_dump(int8_t idval, int8_t *dump)
{
	uint16_t len = dump!=NULL?strlen((char*)dump):0;

	if(len == 0) {
		if(idval == VAL_NONE) printf("[D,,0]\r\n");
		else printf("[D,%d,0]\r\n", idval);
	} else {
		if(idval == VAL_NONE) printf("[D,,%d]\r\n%s\r\n", len, dump);
		else printf("[D,%d,%d]\r\n%s\r\n", idval, len, dump);
		DBG("going to free");
		MEM_FREE(dump);
		DBG("free done");
	}
}

void cmd_resp(int8_t retval, int8_t idval)
{
	uint8_t cnt, len, idx = 0;

	DBGA("ret(%d), id(%d)", retval, idval);
	cnt = (atci.tcmd.arg1[0] != 0) + (atci.tcmd.arg2[0] != 0) + (atci.tcmd.arg3[0] != 0) + 
		  (atci.tcmd.arg4[0] != 0) + (atci.tcmd.arg5[0] != 0) + (atci.tcmd.arg6[0] != 0);
#define MAKE_RESP(item_v, size_v) \
{ \
	if(item_v[0] != 0) { \
		termbuf[idx++] = ','; \
		len = strlen((char*)item_v); \
		if(len > size_v-1) CRITICAL_ERR("resp buf overflow"); \
		memcpy((char*)&termbuf[idx], (char*)item_v, len); \
		idx += len; \
		cnt--; \
	} else if(cnt) { \
		termbuf[idx++] = ','; \
	} \
}//printf("MakeResp-(%s)(%d)", item_v, len); 
	termbuf[idx++] = '[';
	if(retval >= RET_OK) {
		if(retval == RET_OK) termbuf[idx++] = 'S';
		else if(retval == RET_OK_DUMP) CRITICAL_ERR("use cmd_resp_dump for dump");
		else if(retval == RET_ASYNC) termbuf[idx++] = 'W';
		else if(retval == RET_RECV) termbuf[idx++] = 'R';
		else CRITICAL_ERRA("undefined return value (%d)", retval);

		if(idval != VAL_NONE) {
			termbuf[idx++] = ',';
			sprintf((char*)&termbuf[idx], "%d", idval);
			len = digit_length(idval, 10);
			idx += len;
		} else if(cnt) termbuf[idx++] = ',';
	} else {
		termbuf[idx++] = 'F';
		termbuf[idx++] = ',';
		if(idval != VAL_NONE) {
			sprintf((char*)&termbuf[idx], "%d", idval);
			len = digit_length(idval, 10);
			idx += len;
		}
		termbuf[idx++] = ',';
#define CMD_SWT_DEF(errval_v) termbuf[idx++] = errval_v;
#define CMD_SWT_EXT(base_v, errval_v) termbuf[idx++]=base_v;termbuf[idx++] = errval_v;
		switch(retval) {
		case RET_UNSPECIFIED: CMD_SWT_DEF(ERRVAL_UNSPECIFIED); break;
		case RET_WRONG_OP: CMD_SWT_DEF(ERRVAL_WRONG_OP); break;
		case RET_WRONG_SIGN: CMD_SWT_DEF(ERRVAL_WRONG_SIGN); break;
		case RET_WRONG_ARG: CMD_SWT_DEF(ERRVAL_WRONG_ARG); break;
		case RET_RANGE_OUT: CMD_SWT_DEF(ERRVAL_RANGE_OUT); break;
		case RET_DISABLED: CMD_SWT_DEF(ERRVAL_DISABLED); break;
		case RET_NOT_ALLOWED: CMD_SWT_DEF(ERRVAL_NOT_ALLOWED); break;
		case RET_BUSY: CMD_SWT_DEF(ERRVAL_BUSY); break;
		case RET_TIMEOUT: CMD_SWT_DEF(ERRVAL_TIMEOUT); break;
		case RET_NO_SOCK: CMD_SWT_EXT('1', ERRVAL_NO_SOCK); break;
		case RET_SOCK_CLS: CMD_SWT_EXT('1', ERRVAL_SOCK_CLS); break;
		case RET_USING_PORT: CMD_SWT_EXT('1', ERRVAL_USING_PORT); break;
		case RET_NOT_CONN: CMD_SWT_EXT('1', ERRVAL_NOT_CONN); break;
		case RET_WRONG_ADDR: CMD_SWT_EXT('1', ERRVAL_WRONG_ADDR); break;
		case RET_NO_DATA: CMD_SWT_EXT('1', ERRVAL_NO_DATA); break;
		case RET_NO_FREEMEM: CMD_SWT_EXT('2', ERRVAL_NO_FREEMEM); break;
		default:termbuf[idx++] = '0';break;
		}
	}
	MAKE_RESP(atci.tcmd.arg1, ARG_1_SIZE);
	MAKE_RESP(atci.tcmd.arg2, ARG_2_SIZE);
	MAKE_RESP(atci.tcmd.arg3, ARG_3_SIZE);
	MAKE_RESP(atci.tcmd.arg4, ARG_4_SIZE);
	MAKE_RESP(atci.tcmd.arg5, ARG_5_SIZE);
	MAKE_RESP(atci.tcmd.arg6, ARG_6_SIZE);
	termbuf[idx++] = ']';
	termbuf[idx++] = 0;

	// print basic response
	UART_write(termbuf, strlen((char*)termbuf));
	UART_write("\r\n", 2);
}

static void hdl_nset(void)
{
	int8_t mode, num = -1;
	uint8_t ip[4];

	if(atci.tcmd.sign == CMD_SIGN_NONE) atci.tcmd.sign = CMD_SIGN_QUEST;	// x�뒗 ?濡� 移섑??
	if(atci.tcmd.sign == CMD_SIGN_QUEST)
	{
		if(atci.tcmd.arg1[0] != 0) {
			if(str_check(isdigit, atci.tcmd.arg1) != RET_OK) RESP_CDR(RET_WRONG_ARG, 1);
			if(CHK_DGT_RANGE(atci.tcmd.arg1, num, 1, 6)) RESP_CDR(RET_RANGE_OUT, 1);
		}
		CMD_CLEAR();
		act_nset_q(num);
	}
	else if(atci.tcmd.sign == CMD_SIGN_INDIV)
	{
		if(atci.tcmd.arg1[0] != 0) {
			if(str_check(isdigit, atci.tcmd.arg1) != RET_OK) RESP_CDR(RET_WRONG_ARG, 1);
			if(CHK_DGT_RANGE(atci.tcmd.arg1, num, 1, 6)) RESP_CDR(RET_RANGE_OUT, 1);
			if(num == 1) {
				if(CMP_CHAR_2(atci.tcmd.arg2, 'D', 'S')) RESP_CDR(RET_WRONG_ARG, 2);
				mode = atci.tcmd.arg2[0];
				CMD_CLEAR();
				act_nset_a(mode, NULL, NULL, NULL, NULL, NULL);
			} else {
				if(ip_check(atci.tcmd.arg2, ip) != RET_OK) RESP_CDR(RET_WRONG_ARG, 2);
				CMD_CLEAR();
				switch(num) {
				case 2: act_nset_a(0, ip, NULL, NULL, NULL, NULL); return;
				case 3: act_nset_a(0, NULL, ip, NULL, NULL, NULL); return;
				case 4: act_nset_a(0, NULL, NULL, ip, NULL, NULL); return;
				case 5: act_nset_a(0, NULL, NULL, NULL, ip, NULL); return;
				//case 6: act_nset_a(0, NULL, NULL, NULL, NULL, ip); return;
				case 6: RESP_CDR(RET_NOT_ALLOWED, 2); return;
				default: CRITICAL_ERR("nset wrong num");
				}
			}
		} else RESP_CDR(RET_WRONG_ARG, 1);
	}
	else if(atci.tcmd.sign == CMD_SIGN_EQUAL)
	{
		uint8_t sn[4], gw[4], dns1[4], dns2[4], *ptr[5];
		num = 0;
		if(atci.tcmd.arg1[0] != 0) {
			if(CMP_CHAR_2(atci.tcmd.arg1, 'D', 'S')) RESP_CDR(RET_WRONG_ARG, 1);
			else num++;
		}

#define NSET_ARG_SET(arg_p, addr_p, idx_v, ret_v) \
if(arg_p[0] != 0) { \
	num++; \
	if(ip_check(arg_p, addr_p) != RET_OK) RESP_CDR(RET_WRONG_ARG, ret_v); \
	ptr[idx_v] = addr_p; \
} else ptr[idx_v] = NULL

		NSET_ARG_SET(atci.tcmd.arg2, ip, 0, 2);
		NSET_ARG_SET(atci.tcmd.arg3, sn, 1, 3);
		NSET_ARG_SET(atci.tcmd.arg4, gw, 2, 4);
		NSET_ARG_SET(atci.tcmd.arg5, dns1, 3, 5);
		NSET_ARG_SET(atci.tcmd.arg6, dns2, 4, 6);
		if(num == 0) RESP_CR(RET_NOT_ALLOWED);
		mode = atci.tcmd.arg1[0];
		CMD_CLEAR();
		act_nset_a(mode, ptr[0], ptr[1], ptr[2], ptr[3], ptr[4]);
	} 
	else CRITICAL_ERRA("wrong sign(%d)", atci.tcmd.sign);
}

static void hdl_nstat(void)
{
	int8_t num = -1;

	if(atci.tcmd.sign == CMD_SIGN_NONE) atci.tcmd.sign = CMD_SIGN_QUEST;
	if(atci.tcmd.sign == CMD_SIGN_QUEST)
	{
		if(atci.tcmd.arg1[0] != 0) {
			if(str_check(isdigit, atci.tcmd.arg1) != RET_OK) RESP_CDR(RET_WRONG_ARG, 1);
			if(CHK_DGT_RANGE(atci.tcmd.arg1, num, 1, 6)) RESP_CDR(RET_RANGE_OUT, 1);
		}
		CMD_CLEAR();
		act_nstat(num);
	}
	else if(atci.tcmd.sign == CMD_SIGN_INDIV) RESP_CR(RET_WRONG_SIGN);
	else if(atci.tcmd.sign == CMD_SIGN_EQUAL) RESP_CR(RET_WRONG_SIGN);
	else CRITICAL_ERRA("wrong sign(%d)", atci.tcmd.sign);
}

static void hdl_nmac(void)
{
	int8_t num = -1;
	uint8_t mac[6];

	if(atci.tcmd.sign == CMD_SIGN_NONE) atci.tcmd.sign = CMD_SIGN_QUEST;
	if(atci.tcmd.sign == CMD_SIGN_QUEST)
	{
		if(atci.tcmd.arg1[0] != 0) {
			if(str_check(isdigit, atci.tcmd.arg1) != RET_OK) RESP_CDR(RET_WRONG_ARG, 1);
			if(CHK_DGT_RANGE(atci.tcmd.arg1, num, 1, 1)) RESP_CDR(RET_RANGE_OUT, 1);
		}
		CMD_CLEAR();
		act_nmac_q();
	}
	else if(atci.tcmd.sign == CMD_SIGN_INDIV) RESP_CR(RET_WRONG_SIGN);
	else if(atci.tcmd.sign == CMD_SIGN_EQUAL)
	{
		if(mac_check(atci.tcmd.arg1, mac) != RET_OK) RESP_CDR(RET_WRONG_ARG, 1);
		CMD_CLEAR();
		act_nmac_a(mac);
	}
	else CRITICAL_ERRA("wrong sign(%d)", atci.tcmd.sign);
}
static void hdl_mstat(void)
{
	int8_t num = -1;

	if(atci.tcmd.sign == CMD_SIGN_NONE) atci.tcmd.sign = CMD_SIGN_QUEST;
	if(atci.tcmd.sign == CMD_SIGN_QUEST)
	{
		if(atci.tcmd.arg1[0] != 0) {
			if(str_check(isdigit, atci.tcmd.arg1) != RET_OK) RESP_CDR(RET_WRONG_ARG, 1);
			if(CHK_DGT_RANGE(atci.tcmd.arg1, num, 1, 1)) RESP_CDR(RET_RANGE_OUT, 1);
		}
		CMD_CLEAR();
		act_mstat();
	}
	else if(atci.tcmd.sign == CMD_SIGN_INDIV) RESP_CR(RET_WRONG_SIGN);
	else if(atci.tcmd.sign == CMD_SIGN_EQUAL) RESP_CR(RET_WRONG_SIGN);
	else CRITICAL_ERRA("wrong sign(%d)", atci.tcmd.sign);
}

static void hdl_musart1(void)
{
	int8_t num = -1;
	S2E_Packet *value = get_S2E_Packet_pointer();

	if(atci.tcmd.sign == CMD_SIGN_NONE) atci.tcmd.sign = CMD_SIGN_QUEST;
	if(atci.tcmd.sign == CMD_SIGN_QUEST)
	{
		if(atci.tcmd.arg1[0] != 0) {
			if(str_check(isdigit, atci.tcmd.arg1) != RET_OK) RESP_CDR(RET_WRONG_ARG, 1);
			if(CHK_DGT_RANGE(atci.tcmd.arg1, num, 1, 5)) RESP_CDR(RET_RANGE_OUT, 1);
		}
		CMD_CLEAR();
		act_uart_q(USART1, num);
	}
	else if(atci.tcmd.sign == CMD_SIGN_INDIV) 
	{
		if(atci.tcmd.arg1[0] != 0) {
			if(str_check(isdigit, atci.tcmd.arg1) != RET_OK) RESP_CDR(RET_WRONG_ARG, 1);
			if(CHK_DGT_RANGE(atci.tcmd.arg1, num, 1, 5)) RESP_CDR(RET_RANGE_OUT, 1);
			if(num == 1) {			// Baud Rate
				uint32_t baud, i, loop, valid_arg = 0;
				if(str_check(isdigit, atci.tcmd.arg2) != RET_OK) RESP_CDR(RET_WRONG_ARG, 2);
				baud = atoi((const char *)atci.tcmd.arg2);

				loop = sizeof(baud_table) / sizeof(baud_table[0]);
				for(i = 0 ; i < loop ; i++) {
					if(baud == baud_table[i]) {
						value->serial_info[0].baud_rate = baud;
						valid_arg = 1;
						break;
					}
				}
				if(valid_arg == 0) RESP_CDR(RET_WRONG_ARG, 2);
				CMD_CLEAR();
				act_uart_a(USART1, &(value->serial_info[0]));
			} else if(num == 2) {	// Word Length
				if(str_check(isdigit, atci.tcmd.arg2) != RET_OK) RESP_CDR(RET_WRONG_ARG, 2);
				if(CHK_DGT_RANGE(atci.tcmd.arg2, num, 8, 9)) RESP_CDR(RET_RANGE_OUT, 2);
				else value->serial_info[0].data_bits = num;
				CMD_CLEAR();
				act_uart_a(USART1, &(value->serial_info[0]));
			} else if(num == 3) {	// Parity Bit
				if(str_check(isalpha, atci.tcmd.arg2) != RET_OK) RESP_CDR(RET_WRONG_ARG, 2);
				if(CMP_CHAR_3(atci.tcmd.arg2, 'N', 'O', 'E')) RESP_CDR(RET_WRONG_ARG, 2);
				else {
					if(atci.tcmd.arg2[0] == 'N') value->serial_info[0].parity = parity_none;
					else if(atci.tcmd.arg2[0] == 'O') value->serial_info[0].parity = parity_odd;
					else if(atci.tcmd.arg2[0] == 'E') value->serial_info[0].parity = parity_even;
				}
				CMD_CLEAR();
				act_uart_a(USART1, &(value->serial_info[0]));
			} else if(num == 4) {	// Stop Bit
				if(str_check(isdigit, atci.tcmd.arg2) != RET_OK) RESP_CDR(RET_WRONG_ARG, 2);
				if(CHK_DGT_RANGE(atci.tcmd.arg2, num, 1, 2)) RESP_CDR(RET_RANGE_OUT, 2);
				else value->serial_info[0].stop_bits = num;
				CMD_CLEAR();
				act_uart_a(USART1, &(value->serial_info[0]));
			} else if(num == 5) {	// Flow Control 
				if(str_check(isdigit, atci.tcmd.arg2) != RET_OK) RESP_CDR(RET_WRONG_ARG, 2);
				if(CHK_DGT_RANGE(atci.tcmd.arg2, num, 0, 3)) RESP_CDR(RET_RANGE_OUT, 2);
				else value->serial_info[0].flow_control = num;
				CMD_CLEAR();
				act_uart_a(USART1, &(value->serial_info[0]));
			} else RESP_CDR(RET_NOT_ALLOWED, 2);	// ?��媛� �꽕�젙 �븘吏� ?�ы쁽�븞�븿
		} else RESP_CDR(RET_WRONG_ARG, 1);
	}
	else if(atci.tcmd.sign == CMD_SIGN_EQUAL)
	{
		num = 0;
		if(atci.tcmd.arg1[0] != 0) {			// Baud Rate
			uint32_t baud, i, loop, valid_arg = 0;
			if(str_check(isdigit, atci.tcmd.arg1) != RET_OK) RESP_CDR(RET_WRONG_ARG, 1);
			baud = atoi((const char *)atci.tcmd.arg1);

			loop = sizeof(baud_table) / sizeof(baud_table[0]);
			for(i = 0 ; i < loop ; i++) {
				if(baud == baud_table[i]) {
					value->serial_info[0].baud_rate = baud;
					valid_arg = 1;
					break;
				}
			}
			if(valid_arg == 0) RESP_CDR(RET_WRONG_ARG, 1);
		}
		if(atci.tcmd.arg2[0] != 0) {			// Word Length
			if(str_check(isdigit, atci.tcmd.arg2) != RET_OK) RESP_CDR(RET_WRONG_ARG, 2);
			if(CHK_DGT_RANGE(atci.tcmd.arg2, num, 8, 9)) RESP_CDR(RET_RANGE_OUT, 2);
			else value->serial_info[0].data_bits = num;
		}
		if(atci.tcmd.arg3[0] != 0) {			// Parity Bit
			if(str_check(isalpha, atci.tcmd.arg3) != RET_OK) RESP_CDR(RET_WRONG_ARG, 3);
			if(CMP_CHAR_3(atci.tcmd.arg3, 'N', 'O', 'E')) RESP_CDR(RET_WRONG_ARG, 3);
			else {
				if(atci.tcmd.arg3[0] == 'N') value->serial_info[0].parity = parity_none;
				else if(atci.tcmd.arg3[0] == 'O') value->serial_info[0].parity = parity_odd;
				else if(atci.tcmd.arg3[0] == 'E') value->serial_info[0].parity = parity_even;
			}
		}
		if(atci.tcmd.arg4[0] != 0) {			// Stop Bit
			if(str_check(isdigit, atci.tcmd.arg4) != RET_OK) RESP_CDR(RET_WRONG_ARG, 4);
			if(CHK_DGT_RANGE(atci.tcmd.arg4, num, 1, 2)) RESP_CDR(RET_RANGE_OUT, 4);
			else value->serial_info[0].stop_bits = num;
		}
		if(atci.tcmd.arg5[0] != 0) {			// Flow Control
			if(str_check(isdigit, atci.tcmd.arg5) != RET_OK) RESP_CDR(RET_WRONG_ARG, 5);
			if(CHK_DGT_RANGE(atci.tcmd.arg5, num, 0, 1)) RESP_CDR(RET_RANGE_OUT, 5);
			else value->serial_info[0].flow_control = num;
		}

		CMD_CLEAR();
		act_uart_a(USART1, &(value->serial_info[0]));
	}
	else CRITICAL_ERRA("wrong sign(%d)", atci.tcmd.sign);
}

static void hdl_musart2(void)
{
	int8_t num = -1;
	S2E_Packet *value = get_S2E_Packet_pointer();

	if(atci.tcmd.sign == CMD_SIGN_NONE) atci.tcmd.sign = CMD_SIGN_QUEST;
	if(atci.tcmd.sign == CMD_SIGN_QUEST)
	{
		if(atci.tcmd.arg1[0] != 0) {
			if(str_check(isdigit, atci.tcmd.arg1) != RET_OK) RESP_CDR(RET_WRONG_ARG, 1);
			if(CHK_DGT_RANGE(atci.tcmd.arg1, num, 1, 5)) RESP_CDR(RET_RANGE_OUT, 1);
		}
		CMD_CLEAR();
		act_uart_q(USART2, num);
	}
	else if(atci.tcmd.sign == CMD_SIGN_INDIV) 
	{
		if(atci.tcmd.arg1[0] != 0) {
			if(str_check(isdigit, atci.tcmd.arg1) != RET_OK) RESP_CDR(RET_WRONG_ARG, 1);
			if(CHK_DGT_RANGE(atci.tcmd.arg1, num, 1, 5)) RESP_CDR(RET_RANGE_OUT, 1);
			if(num == 1) {			// Baud Rate
				uint32_t baud, i, loop, valid_arg = 0;
				if(str_check(isdigit, atci.tcmd.arg2) != RET_OK) RESP_CDR(RET_WRONG_ARG, 2);
				baud = atoi((const char *)atci.tcmd.arg2);

				loop = sizeof(baud_table) / sizeof(baud_table[0]);
				for(i = 0 ; i < loop ; i++) {
					if(baud == baud_table[i]) {
						value->serial_info[1].baud_rate = baud;
						valid_arg = 1;
						break;
					}
				}
				if(valid_arg == 0) RESP_CDR(RET_WRONG_ARG, 2);
				CMD_CLEAR();
				act_uart_a(USART2, &(value->serial_info[1]));
			} else if(num == 2) {	// Word Length
				if(str_check(isdigit, atci.tcmd.arg2) != RET_OK) RESP_CDR(RET_WRONG_ARG, 2);
				if(CHK_DGT_RANGE(atci.tcmd.arg2, num, 8, 9)) RESP_CDR(RET_RANGE_OUT, 2);
				else value->serial_info[1].data_bits = num;
				CMD_CLEAR();
				act_uart_a(USART2, &(value->serial_info[1]));
			} else if(num == 3) {	// Parity Bit
				if(str_check(isalpha, atci.tcmd.arg2) != RET_OK) RESP_CDR(RET_WRONG_ARG, 2);
				if(CMP_CHAR_3(atci.tcmd.arg2, 'N', 'O', 'E')) RESP_CDR(RET_WRONG_ARG, 2);
				else {
					if(atci.tcmd.arg2[0] == 'N') value->serial_info[1].parity = parity_none;
					else if(atci.tcmd.arg2[0] == 'O') value->serial_info[1].parity = parity_odd;
					else if(atci.tcmd.arg2[0] == 'E') value->serial_info[1].parity = parity_even;
				}
				CMD_CLEAR();
				act_uart_a(USART2, &(value->serial_info[1]));
			} else if(num == 4) {	// Stop Bit
				if(str_check(isdigit, atci.tcmd.arg2) != RET_OK) RESP_CDR(RET_WRONG_ARG, 2);
				if(CHK_DGT_RANGE(atci.tcmd.arg2, num, 1, 2)) RESP_CDR(RET_RANGE_OUT, 2);
				else value->serial_info[1].stop_bits = num;
				CMD_CLEAR();
				act_uart_a(USART2, &(value->serial_info[1]));
			} else if(num == 5) {	// Flow Control 
				if(str_check(isdigit, atci.tcmd.arg2) != RET_OK) RESP_CDR(RET_WRONG_ARG, 2);
				if(CHK_DGT_RANGE(atci.tcmd.arg2, num, 0, 3)) RESP_CDR(RET_RANGE_OUT, 2);
				else value->serial_info[1].flow_control = num;
				CMD_CLEAR();
				act_uart_a(USART2, &(value->serial_info[1]));
			} else RESP_CDR(RET_NOT_ALLOWED, 2);	// ?��媛� �꽕�젙 �븘吏� ?�ы쁽�븞�븿
		} else RESP_CDR(RET_WRONG_ARG, 1);
	}
	else if(atci.tcmd.sign == CMD_SIGN_EQUAL)
	{
		num = 0;
		if(atci.tcmd.arg1[0] != 0) {			// Baud Rate
			uint32_t baud, i, loop, valid_arg = 0;
			if(str_check(isdigit, atci.tcmd.arg1) != RET_OK) RESP_CDR(RET_WRONG_ARG, 1);
			baud = atoi((const char *)atci.tcmd.arg1);

			loop = sizeof(baud_table) / sizeof(baud_table[0]);
			for(i = 0 ; i < loop ; i++) {
				if(baud == baud_table[i]) {
					value->serial_info[1].baud_rate = baud;
					valid_arg = 1;
					break;
				}
			}
			if(valid_arg == 0) RESP_CDR(RET_WRONG_ARG, 1);
		}
		if(atci.tcmd.arg2[0] != 0) {			// Word Length
			if(str_check(isdigit, atci.tcmd.arg2) != RET_OK) RESP_CDR(RET_WRONG_ARG, 2);
			if(CHK_DGT_RANGE(atci.tcmd.arg2, num, 8, 9)) RESP_CDR(RET_RANGE_OUT, 2);
			else value->serial_info[1].data_bits = num;
		}
		if(atci.tcmd.arg3[0] != 0) {			// Parity Bit
			if(str_check(isalpha, atci.tcmd.arg3) != RET_OK) RESP_CDR(RET_WRONG_ARG, 3);
			if(CMP_CHAR_3(atci.tcmd.arg3, 'N', 'O', 'E')) RESP_CDR(RET_WRONG_ARG, 3);
			else {
				if(atci.tcmd.arg3[0] == 'N') value->serial_info[1].parity = parity_none;
				else if(atci.tcmd.arg3[0] == 'O') value->serial_info[1].parity = parity_odd;
				else if(atci.tcmd.arg3[0] == 'E') value->serial_info[1].parity = parity_even;
			}
		}
		if(atci.tcmd.arg4[0] != 0) {			// Stop Bit
			if(str_check(isdigit, atci.tcmd.arg4) != RET_OK) RESP_CDR(RET_WRONG_ARG, 4);
			if(CHK_DGT_RANGE(atci.tcmd.arg4, num, 1, 2)) RESP_CDR(RET_RANGE_OUT, 4);
			else value->serial_info[1].stop_bits = num;
		}
		if(atci.tcmd.arg5[0] != 0) {			// Flow Control
			if(str_check(isdigit, atci.tcmd.arg5) != RET_OK) RESP_CDR(RET_WRONG_ARG, 5);
			if(CHK_DGT_RANGE(atci.tcmd.arg5, num, 0, 3)) RESP_CDR(RET_RANGE_OUT, 5);
			else value->serial_info[1].flow_control = num;
		}

		CMD_CLEAR();
		act_uart_a(USART2, &(value->serial_info[1]));
	}
	else CRITICAL_ERRA("wrong sign(%d)", atci.tcmd.sign);
}

static void hdl_msave(void)
{
	if(atci.tcmd.sign == CMD_SIGN_NONE) {
		CMD_CLEAR();
		act_msave();
	} else RESP_CR(RET_WRONG_SIGN);
}

static void hdl_mrst(void)
{
	if(atci.tcmd.sign == CMD_SIGN_NONE) {
		cmd_resp(RET_OK, VAL_NONE);
//		whil(RingBuffer_GetCount(&txring) != 0);
		NVIC_SystemReset();
	} else RESP_CR(RET_WRONG_SIGN);
}

static void hdl_fiodir(void)
{
	int8_t pin_num = -1, pin_dir = -1;

	if(atci.tcmd.sign == CMD_SIGN_NONE) RESP_CR(RET_WRONG_SIGN);
	else if(atci.tcmd.sign == CMD_SIGN_QUEST) RESP_CR(RET_WRONG_SIGN);
	else if(atci.tcmd.sign == CMD_SIGN_INDIV) RESP_CR(RET_WRONG_SIGN);
	else if(atci.tcmd.sign == CMD_SIGN_EQUAL) {
		if(atci.tcmd.arg1[0] != 0) {
			if(str_check(isdigit, atci.tcmd.arg1) != RET_OK) RESP_CDR(RET_WRONG_ARG, 1);
			if(CHK_DGT_RANGE(atci.tcmd.arg1, pin_num, 1, 16)) RESP_CDR(RET_RANGE_OUT, 1);
		}
		if(atci.tcmd.arg2[0] != 0) {
			if(str_check(isdigit, atci.tcmd.arg2) != RET_OK) RESP_CDR(RET_WRONG_ARG, 2);
			if(CHK_DGT_RANGE(atci.tcmd.arg2, pin_dir, 1, 2)) RESP_CDR(RET_RANGE_OUT, 2);
		}
		CMD_CLEAR();
		act_fiodir(pin_num, pin_dir);
	} else CRITICAL_ERRA("wrong sign(%d)", atci.tcmd.sign);
}

static void hdl_fioval(void)
{
	int8_t pin_num = -1, pin_output = -1;

	if(atci.tcmd.sign == CMD_SIGN_NONE) RESP_CR(RET_WRONG_SIGN);
	else if(atci.tcmd.sign == CMD_SIGN_QUEST) RESP_CR(RET_WRONG_SIGN);
	else if(atci.tcmd.sign == CMD_SIGN_INDIV) RESP_CR(RET_WRONG_SIGN);
	else if(atci.tcmd.sign == CMD_SIGN_EQUAL) {
		if(atci.tcmd.arg1[0] != 0) {
			if(str_check(isdigit, atci.tcmd.arg1) != RET_OK) RESP_CDR(RET_WRONG_ARG, 1);
			if(CHK_DGT_RANGE(atci.tcmd.arg1, pin_num, 1, 16)) RESP_CDR(RET_RANGE_OUT, 1);
		}
		if(atci.tcmd.arg2[0] != 0) {
			if(str_check(isdigit, atci.tcmd.arg2) != RET_OK) RESP_CDR(RET_WRONG_ARG, 2);
			if(CHK_DGT_RANGE(atci.tcmd.arg2, pin_output, 0, 1)) RESP_CDR(RET_RANGE_OUT, 2);
		}
		CMD_CLEAR();
		act_fioval(pin_num, pin_output);
	} else CRITICAL_ERRA("wrong sign(%d)", atci.tcmd.sign);
}
