/*
 * storageHandler.h
 *
 *  Created on: 2014. 9. 16.
 *      Author: Raphael Kwon
 */

#ifndef STORAGEHANDLER_H_
#define STORAGEHANDLER_H_

#include <stdint.h>

int read_storage(uint8_t isConfig, void *data, uint16_t size);
int write_storage(uint8_t isConfig, void *data, uint16_t size);

#endif /* STORAGEHANDLER_H_ */
