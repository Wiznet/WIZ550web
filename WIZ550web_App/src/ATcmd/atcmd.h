/**
 * @file		atcmd.h
 * @brief		AT Command Module - Interface Part Header File
 * @version	1.0
 * @date		2013/02/22
 * @par Revision
 *			2013/02/22 - 1.0 Release
 * @author	Mike Jeong
 * \n\n @par Copyright (C) 2013 WIZnet. All rights reserved.
 */

#ifndef	_ATCMD_H
#define	_ATCMD_H

#define WORK_BUF_SIZE	1024	
#include "library/at_common.h"

struct at_command
{
	const char *cmd;
	void (*process)(void);
	char *description;
	char *usage;
};

void atc_init();
void atc_run(void);

#endif	//_ATCMD_H
