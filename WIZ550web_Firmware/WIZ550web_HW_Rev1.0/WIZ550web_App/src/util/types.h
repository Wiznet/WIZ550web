/**
 * @file		types.h
 * @brief		Common Type Definition Header File.
 * This file influence on all library area through @ref common.h
 * @version	1.1
 * @date		2013/02/22
 * @par Revision
 *			2013/02/22 - 1.0 Release
 *			2013/07/09 - 1.1 Added 'bool' type
 * @author	Mike Jeong
 * \n\n @par Copyright (C) 2013 WIZnet. All rights reserved.
 */

#ifndef _TYPE_H
#define _TYPE_H


typedef signed char int8;
typedef unsigned char uint8;
typedef volatile signed char vint8;
typedef volatile unsigned char vuint8;

typedef signed short int16;
typedef unsigned short uint16;
typedef volatile signed short vint16;
typedef volatile unsigned short vuint16;

typedef signed long int32;
typedef unsigned long uint32;
typedef volatile signed long vint32;
typedef volatile unsigned long vuint32;

typedef enum {false = 0, true = !false} bool;

/**
 * @def RET_FAIL
 * Fail return value
 * @def RET_OK
 * Success return value
 * @def RET_NOK
 * Error return value
 *
 * @def VAL_HIGH
 * Indicate the value is 'high'
 * @def VAL_LOW
 * Indicate the value is 'low'
 *
 * @def VAL_TOG
 * Indicate the value is 'toggle'
 * @def VAL_ON
 * Indicate the value is 'on'
 * @def VAL_OFF
 * Indicate the value is 'off'
 *
 * @def VAL_SET
 * Indicate the value is 'set'
 * @def VAL_CLEAR
 * Indicate the value is 'clear'
 *
 * @def VAL_TRUE
 * Indicate the value is 'true'
 * @def VAL_FALSE
 * Indicate the value is 'false'
 *
 * @def VAL_ENABLE
 * Indicate the value is 'enable'
 * @def VAL_DISABLE
 * Indicate the value is 'disable'
 *
 * @def VAL_NONE
 * Indicate the value is 'none'
 * @def VAL_INVALID
 * Indicate the value is 'invalid'
 *
 * @def CRLF
 * Replace New Line
 */
#define RET_FAIL	1
#define RET_OK		0
#define RET_NOK		-1

#define VAL_HIGH	1
#define VAL_LOW		0

#define VAL_TOG		2
#define VAL_ON		1
#define VAL_OFF		0

#define VAL_SET		1
#define VAL_CLEAR	0

#define VAL_TRUE	1
#define VAL_FALSE	0

#define VAL_ENABLE	1
#define VAL_DISABLE	0

#define VAL_NONE	-1
#define VAL_INVALID	-2

#define CRLF	"\r\n"

#ifndef _BSDTYPES_DEFINED
/* also defined in mingw/gmon.h and in w32api/winsock[2].h */
#ifndef __u_char_defined
typedef	unsigned char	u_char;
#define __u_char_defined
#endif
#ifndef __u_short_defined
typedef	unsigned short	u_short;
#define __u_short_defined
#endif
#ifndef __u_int_defined
typedef	unsigned int	u_int;
#define __u_int_defined
#endif
#ifndef __u_long_defined
typedef	unsigned long	u_long;
#define __u_long_defined
#endif
#define _BSDTYPES_DEFINED
#endif

#endif //_TYPE_H



