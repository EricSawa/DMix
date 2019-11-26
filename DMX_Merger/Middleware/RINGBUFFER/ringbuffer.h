/*
 * ringbuffer.h
 *
 *  Created on: 15.01.2018
 *      Author: sawadee
 */

#ifndef RINGBUFFER_H_
#define RINGBUFFER_H_

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <string.h>
//#include "stm32f1xx_hal.h"
//#include "main.h"

#define ringbuffer_OK		1
#define ringbuffer_EMPTY	2
#define ringbuffer_FULL 	3
#define ringbuffer_NO_BUF	0

typedef struct{
	uint16_t in;
	uint16_t out;
	uint16_t size;
	char *data;
}stRingBuf;

/* Public functions (prototypes) --------------------------------------------------*/
uint8_t RINGBUFFER_cIn(char data, stRingBuf *pBuf);
uint8_t RINGBUFFER_cOut(char *pData, stRingBuf *pBuf);
uint8_t RINGBUFFER_isEmpty(stRingBuf *pBuf);

#endif /* RINGBUFFER_H_ */
