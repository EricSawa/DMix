/*
 * ringbuffer.c
 *
 *  Created on: 15.01.2018
 *      Author: sawadee
 */
#include "ringbuffer.h"

uint8_t RINGBUFFER_cIn(char data, stRingBuf *pBuf){
	if(pBuf->data != NULL){
		if ( ( pBuf->in + 1 == pBuf->out ) || ( pBuf->out == 0 && pBuf->in + 1 == pBuf->size ) ) return ringbuffer_FULL;
		pBuf->data[pBuf->in] = data;
		pBuf->in++;
		if (pBuf->in >= pBuf->size) pBuf->in = 0;
		return ringbuffer_OK;
	}else return ringbuffer_NO_BUF;
}
uint8_t RINGBUFFER_cOut(char *pData, stRingBuf *pBuf){
	if(pBuf->data != NULL){
		if (pBuf->out == pBuf->in) return ringbuffer_EMPTY;
		*pData = pBuf->data[pBuf->out];
		pBuf->out++;
		if (pBuf->out >= pBuf->size)
		  pBuf->out = 0;
		return ringbuffer_OK;
	}else return ringbuffer_NO_BUF;
}

uint8_t RINGBUFFER_isEmpty(stRingBuf *pBuf){
	if(pBuf->out == pBuf->in) return 1;
	else return 0;
}
