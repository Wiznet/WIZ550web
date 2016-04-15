/**
 * @file		wiz_types.h
 * @brief		Common Type Definition Header File.
 * This file influence on all library area through @ref common.h
 * @version	1.1
 * @date		2013/02/22
 * @par Revision
 *			2013/02/22 - 1.0 Release
 *			2013/07/09 - 1.1 Added 'bool' type
 *			2014/04/25 - 1.2 remove type definitions
 * @author	Mike Jeong
 * \n\n @par Copyright (C) 2013 WIZnet. All rights reserved.
 */

#ifndef _WIZ_TYPES_H
#define _WIZ_TYPES_H


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

#endif //_WIZ_TYPES_H
