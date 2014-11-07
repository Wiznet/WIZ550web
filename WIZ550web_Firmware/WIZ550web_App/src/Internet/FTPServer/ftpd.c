/*
* Wiznet.
* (c) Copyright 2002, Wiznet.
*
* Filename	: ftpd.c
* Version	: 1.0
* Programmer(s)	: 
* Created	: 2003/01/28
* Description   : FTP daemon. (AVR-GCC Compiler)
*/


#include <stdio.h> 
#include <ctype.h> 
#include <string.h>
#include <limits.h>
#include <stdarg.h>
#include "stdio_private.h"
#include "Ethernet/socket.h"
#include "ftpd.h"
#include "netutil/netutil.h"
#include "common.h"

/* Command table */
static char *commands[] = {
	"user",
	"acct",
	"pass",
	"type",
	"list",
	"cwd",
	"dele",
	"name",
	"quit",
	"retr",
	"stor",
	"port",
	"nlst",
	"pwd",
	"xpwd",
	"mkd",
	"xmkd",
	"xrmd",
	"rmd ",
	"stru",
	"mode",
	"syst",
	"xmd5",
	"xcwd",
	"feat",
	"pasv",
	"size",
	"mlsd",
	"appe",
	NULL
};

#if 0
/* Response messages */
char banner[] = "220 %s FTP version %s ready.\r\n";
char badcmd[] = "500 Unknown command '%s'\r\n";
char binwarn[] = "100 Warning: type is ASCII and %s appears to be binary\r\n";
char unsupp[] = "500 Unsupported command or option\r\n";
char givepass[] = "331 Enter PASS command\r\n";
char logged[] = "230 Logged in\r\n";
char typeok[] = "200 Type %s OK\r\n";
char only8[] = "501 Only logical bytesize 8 supported\r\n";
char deleok[] = "250 File deleted\r\n";
char mkdok[] = "200 MKD ok\r\n";
char delefail[] = "550 Delete failed: %s\r\n";
char pwdmsg[] = "257 \"%s\" is current directory\r\n";
char badtype[] = "501 Unknown type \"%s\"\r\n";
char badport[] = "501 Bad port syntax\r\n";
char unimp[] = "502 Command does not implemented yet.\r\n";
char bye[] = "221 Goodbye!\r\n";
char nodir[] = "553 Can't read directory \"%s\": %s\r\n";
char cantopen[] = "550 Can't read file \"%s\": %s\r\n";
char sending[] = "150 Opening data connection for %s (%d.%d.%d.%d,%d)\r\n";
char cantmake[] = "553 Can't create \"%s\": %s\r\n";
char writerr[] = "552 Write error: %s\r\n";
char portok[] = "200 PORT command successful.\r\n";
char rxok[] = "226 Transfer complete.\r\n";
char txok[] = "226 Transfer complete.\r\n";
char noperm[] = "550 Permission denied\r\n";
char noconn[] = "425 Data connection reset\r\n";
char lowmem[] = "421 System overloaded, try again later\r\n";
char notlog[] = "530 Please log in with USER and PASS\r\n";
char userfirst[] = "503 Login with USER first.\r\n";
char okay[] = "200 Ok\r\n";
char syst[] = "215 %s Type: L%d Version: %s\r\n";
char sizefail[] = "550 File not found\r\n";
#endif

un_l2cval remote_ip;
uint16_t  remote_port;
un_l2cval local_ip;
uint16_t  local_port;
uint8_t connect_state_control = 0;
uint8_t connect_state_data = 0;

struct ftpd ftp;
//extern uint8_t * rx_buf;

int fsprintf(uint8_t s, const char *format, ...)
{
	int i;
/*
	char buf[LINELEN];
	FILE f;
	va_list ap;

	f.flags = __SWR | __SSTR;
	f.buf = buf;
	f.size = INT_MAX;
	va_start(ap, format);
	i = vfprintf(&f, format, ap);
	va_end(ap);
	buf[f.len] = 0;

	send(s, (uint8_t *)buf, strlen(buf));
*/
	return i;
}

void ftpd_init(uint8_t csock, uint8_t dsock, uint8_t * src_ip)
{
	ftp.control = csock;
	ftp.data = dsock;
	ftp.state = FTPS_NOT_LOGIN;
	ftp.current_cmd = NO_CMD;
	ftp.dsock_mode = ACTIVE_MODE;

	local_ip.cVal[0] = src_ip[0];
	local_ip.cVal[1] = src_ip[1];
	local_ip.cVal[2] = src_ip[2];
	local_ip.cVal[3] = src_ip[3];
	local_port = 35000;
	
	strcpy(ftp.workingdir, "/");

	socket(ftp.control, Sn_MR_TCP, IPPORT_FTP, 0x0);
}

uint8_t ftpd_run(uint8_t * buf)
{
	uint16_t size = 0, i;
	long ret = 0;
	uint16_t blocklen, send_byte;
	uint32_t remain_filesize;
	FILINFO fno;


//	 memset(buf, 0, sizeof(DATA_BUF_SIZE));
	
   switch(getSn_SR(ftp.control))
   {
   case SOCK_ESTABLISHED :

		if(!connect_state_control)
		{
			printf("%d:FTP Connected\r\n", ftp.control);
//			fsprintf(ftp.control, banner, HOSTNAME, VERSION);
			sprintf((char *)buf, "220 %s FTP version %s ready.\r\n", HOSTNAME, VERSION);
			ret = send(ftp.control, (uint8_t *)buf, strlen((const char *)buf));
			if(ret < 0)
			{
				printf("%d:send() error:%ld\r\n",ftp.control,ret);
				close(ftp.control);
				return ret;
			}
			connect_state_control = 1;
		}
	
//       printf("ftp socket %d\r\n", ftp.control);
			 
		if((size = getSn_RX_RSR(ftp.control)) > 0) // Don't need to check SOCKERR_BUSY because it doesn't not occur.
		{
			printf("size: %d\r\n", size);

			if(size > DATA_BUF_SIZE) size = DATA_BUF_SIZE - 1;

			ret = recv(ftp.control,buf,size);
			buf[ret] = '\0';
		    if(ret != size)
		    {
				if(ret==SOCK_BUSY) return 0;
				if(ret < 0)
				{
					printf("%d:recv() error:%ld\r\n",ftp.control,ret);
					close(ftp.control);
					return ret;
				}
			}
			printf("Rcvd Command: %s", buf);
			proc_ftpd((char *)buf);
		}
	   break;
   case SOCK_CLOSE_WAIT :
       printf("%d:CloseWait\r\n",ftp.control);
       if((ret=disconnect(ftp.control)) != SOCK_OK) return ret;
       printf("%d:Closed\r\n",ftp.control);
	   break;
   case SOCK_CLOSED :
       printf("%d:FTPStart\r\n",ftp.control);
       if((ret=socket(ftp.control, Sn_MR_TCP, IPPORT_FTP, 0x0)) != ftp.control)
       {
    	   printf("%d:socket() error:%ld\r\n", ftp.control, ret);
    	   close(ftp.control);
    	   return ret;
       }
	   break;
   case SOCK_INIT :
       printf("%d:Opened\r\n",ftp.control);
       strcpy(ftp.workingdir, "/");
       if( (ret = listen(ftp.control)) != SOCK_OK)
       {
           printf("%d:Listen error\r\n",ftp.control);
           return ret;
       }
			 connect_state_control = 0;

       printf("%d:Listen ok\r\n",ftp.control);
	   break;
   default :
	   break;
   }

#if 1
   switch(getSn_SR(ftp.data))
   {
   case SOCK_ESTABLISHED :

		if(!connect_state_data)
		{
			printf("%d:FTP Data socket Connected\r\n", ftp.data);
			connect_state_data = 1;
		}
	
		switch(ftp.current_cmd)
		{
		case LIST_CMD:
		case MLSD_CMD:
			printf("previous size: %d\r\n", size);
			scan_files(ftp.workingdir, buf, (int *)&size);
			printf("returned size: %d\r\n", size);
			printf("%s\r\n", buf);
//			size = sprintf(buf, "drwxr-xr-x 1 ftp ftp              0 Apr 07  2014 $RECYCLE.BIN\r\n");
			send(ftp.data, buf, size);
			ftp.current_cmd = NO_CMD;
			disconnect(ftp.data);
			size = sprintf(buf, "226 Successfully transferred \"%s\"\r\n", ftp.workingdir);
			send(ftp.control, buf, size);
			break;
		case RETR_CMD:
			printf("filename to retrieve : %s\r\n", ftp.filename);
			ftp.fr = f_open(&(ftp.fil), (const char *)ftp.filename, FA_READ);
//			print_filedsc(&(ftp.fil));
			if(ftp.fr == FR_OK){
				remain_filesize = ftp.fil.fsize;
				printf("f_open return FR_OK\r\n");
				do{
//					printf("remained file size: %d\r\n", ftp.fil.fsize);
					if(remain_filesize > 512)
						send_byte = 512;
					else
						send_byte = remain_filesize;

			       	memset(buf, 0, DATA_BUF_SIZE);
					ftp.fr = f_read(&(ftp.fil), buf, send_byte , (void *)&blocklen);
					if(ftp.fr != FR_OK)
						break;
					printf("#", ftp.fr, send_byte, blocklen);
					send(ftp.data, buf, blocklen);
					remain_filesize -= blocklen;
				}while(remain_filesize != 0);
				printf("\r\nFile read finished\r\n");

				ftp.fr = f_close(&(ftp.fil));
			}else{
				printf("File Open Error: %d\r\n", ftp.fr);
			}


			ftp.current_cmd = NO_CMD;
			disconnect(ftp.data);
			size = sprintf(buf, "226 Successfully transferred \"%s\"\r\n", ftp.filename);
			send(ftp.control, buf, size);
			break;
		case STOR_CMD:
			printf("filename to store : %s\r\n", ftp.filename);
			ftp.fr = f_open(&(ftp.fil), (const char *)ftp.filename, FA_CREATE_ALWAYS | FA_WRITE);
//			print_filedsc(&(ftp.fil));
			if(ftp.fr == FR_OK){

				printf("f_open return FR_OK\r\n");
				while(1){
					if((size = getSn_RX_RSR(ftp.data)) > 0){
				       	if(size > 512) size = 512;

				       	memset(buf, 0, DATA_BUF_SIZE);
				        ret = recv(ftp.data, buf, size);

				        ftp.fr = f_write(&(ftp.fil), buf, (UINT)ret, (void *)&blocklen);
				        if(ftp.fr != FR_OK){
				        	printf("f_write failed\r\n");
				        	break;
				        }
			        	printf("#");
					}else{
						if(getSn_SR(ftp.data) != SOCK_ESTABLISHED)
							break;
					}
				}
				printf("\r\nFile write finished\r\n");
				ftp.fr = f_close(&(ftp.fil));
			}else{
				printf("File Open Error: %d\r\n", ftp.fr);
			}

			ftp.current_cmd = NO_CMD;
			disconnect(ftp.data);
			size = sprintf(buf, "226 Successfully transferred \"%s\"\r\n", ftp.filename);
			send(ftp.control, buf, size);
			break;
		case NO_CMD:
		default:
			break;
		}
	   break;
   case SOCK_CLOSE_WAIT :
       printf("%d:CloseWait\r\n",ftp.data);
       if((ret=disconnect(ftp.data)) != SOCK_OK) return ret;
       printf("%d:Closed\r\n",ftp.data);
	   break;
   case SOCK_CLOSED :
       if(ftp.dsock_state == DATASOCK_READY)
       {
    	   if(ftp.dsock_mode == PASSIVE_MODE){
			   printf("%d:FTPDataStart, port : %d\r\n",ftp.data, local_port);

			   if((ret=socket(ftp.data, Sn_MR_TCP, local_port, 0x0)) != ftp.data)
			   {
				   printf("%d:socket() error:%ld\r\n", ftp.data, ret);
				   close(ftp.data);

				   return ret;
			   }

			   local_port++;
			   if(local_port > 50000)
				   local_port = 35000;
    	   }else{
			   printf("%d:FTPDataStart, port : %d\r\n",ftp.data, IPPORT_FTPD);
			   if((ret=socket(ftp.data, Sn_MR_TCP, IPPORT_FTPD, 0x0)) != ftp.data)
			   {
				   printf("%d:socket() error:%ld\r\n", ftp.data, ret);
				   close(ftp.data);

				   return ret;
			   }
    	   }

    	   ftp.dsock_state = DATASOCK_START;
       }
	   break;
   case SOCK_INIT :
       printf("%d:Opened\r\n",ftp.data);
	   if(ftp.dsock_mode == PASSIVE_MODE){
		   if( (ret = listen(ftp.data)) != SOCK_OK)
		   {
			   printf("%d:Listen error\r\n",ftp.data);
			   return ret;
		   }

		   printf("%d:Listen ok\r\n",ftp.data);
	   }else{
		   if((ret = connect(ftp.data, remote_ip.cVal, remote_port)) != SOCK_OK){
			   printf("%d:Connect error\r\n", ftp.data);
			   return ret;
		   }
	   }
	   connect_state_data = 0;
	   break;
   default :
	   break;
   }
#endif

	 return 0;
}

char proc_ftpd(char * buf)
{
	char **cmdp, *cp, *arg, *tmpstr;
	char sendbuf[200];
	int slen;
	long ret;
	

	/* Translate first word to lower case */
	for (cp = buf; *cp != ' ' && *cp != '\0'; cp++)
		*cp = tolower(*cp);

	/* Find command in table; if not present, return syntax error */
	for (cmdp = commands; *cmdp != NULL; cmdp++)
		if (strncmp(*cmdp, buf, strlen(*cmdp)) == 0)
			break;

	if (*cmdp == NULL)
	{
//		fsprintf(ftp.control, badcmd, buf);
		slen = sprintf(sendbuf, "500 Unknown command '%s'\r\n", buf);
		send(ftp.control, (uint8_t *)sendbuf, slen);
		return 0;
	}
	/* Allow only USER, PASS and QUIT before logging in */
	if (ftp.state == FTPS_NOT_LOGIN)
	{
		switch(cmdp - commands)
		{
		case USER_CMD:
		case PASS_CMD:
		case QUIT_CMD:
			break;
		default:
//			fsprintf(ftp.control, notlog);
		slen = sprintf(sendbuf, "530 Please log in with USER and PASS\r\n");
		send(ftp.control, (uint8_t *)sendbuf, slen);
			return 0;
		}
	}
	
	arg = &buf[strlen(*cmdp)];
	while(*arg == ' ') arg++;

	/* Execute specific command */
	switch (cmdp - commands)
	{
	case USER_CMD :
		printf("USER_CMD : %s", arg);
		slen = strlen(arg);
		arg[slen - 1] = 0x00;
		arg[slen - 2] = 0x00;
		strcpy(ftp.username, arg);
//		fsprintf(ftp.control, givepass);
		slen = sprintf(sendbuf, "331 Enter PASS command\r\n");
		ret = send(ftp.control, (uint8_t *)sendbuf, slen);
		if(ret < 0)
		{
			printf("%d:send() error:%ld\r\n",ftp.control,ret);
			close(ftp.control);
			return ret;
		}

		break;
	case PASS_CMD :
		printf("PASS_CMD : %s", arg);
		slen = strlen(arg);
		arg[slen - 1] = 0x00;
		arg[slen - 2] = 0x00;
		ftplogin(arg);
		break;			
	case TYPE_CMD :
		slen = strlen(arg);
		arg[slen - 1] = 0x00;
		arg[slen - 2] = 0x00;
		switch(arg[0])
		{
		case 'A':
		case 'a':	/* Ascii */
			ftp.type = ASCII_TYPE;
	//		fsprintf(ftp.control, typeok, arg);
		slen = sprintf(sendbuf, "200 Type set to %s\r\n", arg);
		send(ftp.control, (uint8_t *)sendbuf, slen);
			break;
		case 'B':
		case 'b':	/* Binary */
		case 'I':
		case 'i':	/* Image */
			ftp.type = IMAGE_TYPE;
	//		fsprintf(ftp.control, typeok, arg);
		slen = sprintf(sendbuf, "200 Type set to %s\r\n", arg);
		send(ftp.control, (uint8_t *)sendbuf, slen);
			break;
		default:	/* Invalid */
//			fsprintf(ftp.control, badtype, arg);
		slen = sprintf(sendbuf, "501 Unknown type \"%s\"\r\n", arg);
		send(ftp.control, (uint8_t *)sendbuf, slen);
			break;
		}
		break;
	case FEAT_CMD :
		slen = sprintf(sendbuf, "211-Features:\r\n MDTM\r\n REST STREAM\r\n SIZE\r\n MLST size*;type*;create*;modify*;\r\n MLSD\r\n UTF8\r\n CLNT\r\n MFMT\r\n211 END\r\n");
		send(ftp.control, (uint8_t *)sendbuf, slen);
			break;
	case QUIT_CMD :
		printf("QUIT_CMD\r\n");
//		fsprintf(ftp.control, bye);
		slen = sprintf(sendbuf, "221 Goodbye!\r\n");
		send(ftp.control, (uint8_t *)sendbuf, slen);
		disconnect(ftp.control);
		break;
	case RETR_CMD :
		slen = strlen(arg);
		arg[slen - 1] = 0x00;
		arg[slen - 2] = 0x00;
		printf("RETR_CMD\r\n");
		if(strlen(ftp.workingdir) == 1)
			sprintf(ftp.filename, "/%s", arg);
		else
			sprintf(ftp.filename, "%s/%s", ftp.workingdir, arg);
		slen = sprintf(sendbuf, "150 Opening data channel for file downloand from server of \"%s\"\r\n", ftp.filename);
		send(ftp.control, (uint8_t *)sendbuf, slen);
		ftp.current_cmd = RETR_CMD;
		break;
	case APPE_CMD :
	case STOR_CMD:
		slen = strlen(arg);
		arg[slen - 1] = 0x00;
		arg[slen - 2] = 0x00;
		printf("STOR_CMD\r\n");
		if(strlen(ftp.workingdir) == 1)
			sprintf(ftp.filename, "/%s", arg);
		else
			sprintf(ftp.filename, "%s/%s", ftp.workingdir, arg);
		slen = sprintf(sendbuf, "150 Opening data channel for file upload to server of \"%s\"\r\n", ftp.filename);
		send(ftp.control, (uint8_t *)sendbuf, slen);
		ftp.current_cmd = STOR_CMD;
		break;
	case PORT_CMD:
		printf("PORT_CMD\r\n");
		if (pport(arg) == -1){
	//		fsprintf(ftp.control, badport);
			slen = sprintf(sendbuf, "501 Bad port syntax\r\n");
			send(ftp.control, (uint8_t *)sendbuf, slen);
		} else{
//			fsprintf(ftp.control, portok);
			ftp.dsock_mode = ACTIVE_MODE;
			ftp.dsock_state = DATASOCK_READY;
			slen = sprintf(sendbuf, "200 PORT command successful.\r\n");
			send(ftp.control, (uint8_t *)sendbuf, slen);
		}
		break;
	case MLSD_CMD:
		printf("MLSD_CMD\r\n");
		slen = sprintf(sendbuf, "150 Opening data channel for directory listing of \"%s\"\r\n", ftp.workingdir);
		send(ftp.control, (uint8_t *)sendbuf, slen);
		ftp.current_cmd = MLSD_CMD;
		break;
	case LIST_CMD:
		printf("LIST_CMD\r\n");
		slen = sprintf(sendbuf, "150 Opening data channel for directory listing of \"%s\"\r\n", ftp.workingdir);
		send(ftp.control, (uint8_t *)sendbuf, slen);
		ftp.current_cmd = LIST_CMD;
		break;
	case NLST_CMD:
		printf("NLST_CMD\r\n");
		break;
	case SYST_CMD:
		slen = sprintf(sendbuf, "215 UNIX emulated by WIZnet\r\n");
		send(ftp.control, (uint8_t *)sendbuf, slen);
		break;
	case PWD_CMD:
		slen = sprintf(sendbuf, "257 \"%s\" is current directory.\r\n", ftp.workingdir);
		send(ftp.control, (uint8_t *)sendbuf, slen);
		break;
	case PASV_CMD:
		slen = sprintf(sendbuf, "227 Entering Passive Mode (%d,%d,%d,%d,%d,%d)\r\n", local_ip.cVal[0], local_ip.cVal[1], local_ip.cVal[2], local_ip.cVal[3], local_port >> 8, local_port & 0x00ff);
		send(ftp.control, (uint8_t *)sendbuf, slen);
		disconnect(ftp.data);
		ftp.dsock_mode = PASSIVE_MODE;
		ftp.dsock_state = DATASOCK_READY;
		printf("PASV port: %d\r\n", local_port);
		break;
	case SIZE_CMD:
		slen = strlen(arg);
		arg[slen - 1] = 0x00;
		arg[slen - 2] = 0x00;
		if(slen > 3)
		{
			tmpstr = strrchr(arg, '/');
			*tmpstr = 0;
			slen = get_filesize(arg, tmpstr + 1);
			if(slen > 0)
				slen = sprintf(sendbuf, "213 %d\r\n", slen);
			else
				slen = sprintf(sendbuf, "550 File not Found\r\n");
		}else
		{
			slen = sprintf(sendbuf, "550 File not Found\r\n");
		}
		send(ftp.control, (uint8_t *)sendbuf, slen);
		break;
	case CWD_CMD:
		slen = strlen(arg);
		arg[slen - 1] = 0x00;
		arg[slen - 2] = 0x00;
		if(slen > 3)
		{
			arg[slen - 3] = 0x00;
			tmpstr = strrchr(arg, '/');
			*tmpstr = 0;
			slen = get_filesize(arg, tmpstr + 1);
			*tmpstr = '/';
			if(slen == 0){
				slen = sprintf(sendbuf, "213 %d\r\n", slen);
				strcpy(ftp.workingdir, arg);
				slen = sprintf(sendbuf, "250 CWD successful. \"%s\" is current directory.\r\n", ftp.workingdir);
			}
			else{
				slen = sprintf(sendbuf, "550 CWD failed. \"%s\"\r\n", arg);
			}
		}else
		{
			strcpy(ftp.workingdir, arg);
			slen = sprintf(sendbuf, "250 CWD successful. \"%s\" is current directory.\r\n", ftp.workingdir);
		}
		send(ftp.control, (uint8_t *)sendbuf, slen);
		break;
	case MKD_CMD:
#if 1
		if (f_mkdir(arg) != 0)
		{
			slen = sprintf(sendbuf, "550 Can't create directory. \"%s\"\r\n", arg);
		}
		else
		{
			slen = sprintf(sendbuf, "257 MKD command successful. \"%s\"\r\n", arg);
			//strcpy(ftp.workingdir, arg);
		}
#else
		slen = sprintf(sendbuf, "550 Can't create directory. Permission denied\r\n");
#endif
		send(ftp.control, (uint8_t *)sendbuf, slen);
		break;
	case XCWD_CMD:
	case XPWD_CMD:
	case ACCT_CMD:		
	case DELE_CMD:
	case XMKD_CMD:
	case XRMD_CMD:
	case RMD_CMD:
	case STRU_CMD:
	case MODE_CMD:
	case XMD5_CMD:
//		fsprintf(ftp.control, unimp);
		slen = sprintf(sendbuf, "502 Command does not implemented yet.\r\n");
		send(ftp.control, (uint8_t *)sendbuf, slen);
		break;

	default:	/* Invalid */
//		fsprintf(ftp.control, badcmd, arg);
		slen = sprintf(sendbuf, "500 Unknown command \'%s\'\r\n", arg);
		send(ftp.control, (uint8_t *)sendbuf, slen);
break;
	}
	
	return 1;
}


char ftplogin(char * pass)
{
	char sendbuf[100];
	int slen = 0;
	
//	memset(sendbuf, 0, DATA_BUF_SIZE);
	
	printf("%s logged in\r\n", ftp.username);
//	fsprintf(ftp.control, logged);
		slen = sprintf(sendbuf, "230 Logged on\r\n");
		send(ftp.control, (uint8_t *)sendbuf, slen);
	ftp.state = FTPS_LOGIN;
	
	return 1;
}

int pport(char * arg)
{
	int i;
	char* tok=0;

	for (i = 0; i < 4; i++)
	{
		if(i==0) tok = strtok(arg,",\r\n");
		else	 tok = strtok(NULL,",");
		remote_ip.cVal[i] = (uint8_t)ATOI(tok, 10);
		if (!tok)
		{
			printf("bad pport : %s\r\n", arg);
			return -1;
		}
	}
	remote_port = 0;
	for (i = 0; i < 2; i++)
	{
		tok = strtok(NULL,",\r\n");
		remote_port <<= 8;
		remote_port += ATOI(tok, 10);
		if (!tok)
		{
			printf("bad pport : %s\r\n", arg);
			return -1;
		}
	}
	printf("ip : %d.%d.%d.%d, port : %d\r\n", remote_ip.cVal[0], remote_ip.cVal[1], remote_ip.cVal[2], remote_ip.cVal[3], remote_port);

	return 0;
}


void print_filedsc(FIL *fil)
{
	printf("File System pointer : %08X\r\n", fil->fs);
	printf("File System mount ID : %d\r\n", fil->id);
	printf("File status flag : %08X\r\n", fil->flag);
	printf("File System pads : %08X\r\n", fil->pad1);
	printf("File read write pointer : %08X\r\n", fil->fptr);
	printf("File size : %08X\r\n", fil->fsize);
	printf("File start cluster : %08X\r\n", fil->sclust);
	printf("current cluster : %08X\r\n", fil->clust);
	printf("current data sector : %08X\r\n", fil->dsect);
	printf("dir entry sector : %08X\r\n", fil->dir_sect);
	printf("dir entry pointer : %08X\r\n", fil->dir_ptr);
}
