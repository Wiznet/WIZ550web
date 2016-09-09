/**
 * @file		util.c
 * @brief		Common Utility Function Set Header File
 * @version	1.0
 * @date		2013/02/22
 * @par Revision
 *			2013/02/22 - 1.0 Release
 * @author	Mike Jeong
 * \n\n @par Copyright (C) 2013 WIZnet. All rights reserved.
 */

//#define FILE_LOG_SILENCE
#include "at_common.h"

/**
 * @addtogroup common_util
 * @{
 */

/**
 * Count digit's letter
 * Ex) digit_length(12345, 10) : This will return 5.
 * @param dgt The digit value to count
 * @param base Digit base like 2, 8, 10, 16
 * @return >0: Counted digit letter
 * @return RET_NOK: Error
 */
int8_t digit_length(int32_t dgt, int8_t base)
{
	int16_t i, len = 0;

	if(dgt < 0) {
		len++;
		dgt *= -1;
	}

	for(i=0; i<255; i++) {
		len++;
		dgt /= base;
		if(dgt == 0) return len;
	}

	return RET_NOK;
}

/**
 * Check string with standard library method.
 * Below is the method you can use.
 * - isalpha, isupper, islower
 * - isdigit, isxdigit, isalnum
 * - isspace, ispunct, isprint
 * - isgraph, iscntrl, isascii
 *
 * Ex) str_check(isdigit, "12345") : This will return RET_OK. \n
 * Ex) str_check(islower, "AbcDe") : This will return RET_NOK.
 * @param method The method to use for check
 * @param str The string to check
 * @return RET_OK: Success
 * @return RET_NOK: Error
 */
int32_t str_check(int (*method)(int), int8_t *str)
{
	if(method == NULL || str == NULL || *str == 0)
		return RET_NOK;

	while(*str) {
		if(!method((int)*str)) return RET_NOK;
		str++;
	}

	return RET_OK;
}

/**
 * Separate string into small peace by delimiter like strtok.
 * But if the input string contains more than one character from delimiter \n
 * in a row, strsep returns an empty string for each pair of characters from delimiter.
 *
 * Ex) strsep("a,b,c,,,f,gh", ",") : When meet ,,, strtok returns 'f' but this returns NULL.
 *
 * @param stringp String to separate
 * @param delim Delimiter
 * @return Next pointer separated by delimiter
 */
int8_t *strsep_ex(int8_t **stringp, const int8_t *delim)
{
    int8_t *s;
    const int8_t *spanp;
    int32_t c, sc;
    int8_t *tok;

    if ((s = *stringp) == NULL)
        return (NULL);
    for (tok = s;;) {
        c = *s++;
        spanp = delim;
        do {
            if ((sc = *spanp++) == c) {
                if (c == 0)
                    s = NULL;
                else
                    s[-1] = 0;
                *stringp = s;
                return (tok);
            }
        } while (sc != 0);
    }
    /* NOTREACHED */
}

/**
 * Calculate checksum of a stream.
 * @param src The string to calculate checksum.
 * @param len The string length.
 * @return Checksum
 */ 
uint16_t checksum(uint8_t * src, uint32_t len)
{
	uint16_t sum, tsum, i, j;
	uint32_t lsum;

	j = len >> 1;
	lsum = 0;

	for(i = 0; i < j; i++) {
		tsum = src[i*2];
		tsum = tsum << 8;
		tsum += src[i*2+1];
		lsum += tsum;
	}

	if(len % 2) {
		tsum = src[i*2];
		lsum += (tsum << 8);
	}

	sum = lsum;
	sum = ~(sum + (lsum >> 16));

	return (uint16_t)sum;
}

/* @} */

//#ifdef USE_FULL_ASSERT
#if 0
/* NOT USE !!!
 * Asset function which declared at stm32f10x_conf.h file
 * If USE_FULL_ASSERT is defined this will be active.
 * @param file File name in which asset occurred.
 * @param line asserted line number
 */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif



