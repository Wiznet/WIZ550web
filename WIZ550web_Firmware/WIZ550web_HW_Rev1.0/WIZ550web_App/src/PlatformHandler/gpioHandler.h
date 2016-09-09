/**
 * @file	gpioHandler.h
 * @brief	Header File for GPIO Handler Example
 * @version 1.0
 * @date	2014/07/15
 * @par Revision
 *			2014/07/15 - 1.0 Release
 * @author	
 * \n\n @par Copyright (C) 1998 - 2014 WIZnet. All rights reserved.
 */

#ifndef __GPIOHANDLER_H
#define __GPIOHANDLER_H

#include "boardutil.h"

void GPIO_Configuration(void);

void IO_Init(IO_Type io);
void IO_On(IO_Type io);
void IO_Off(IO_Type io);
void IO_Toggle(IO_Type io);
uint8 get_IO_Status(IO_Type io);

#endif

