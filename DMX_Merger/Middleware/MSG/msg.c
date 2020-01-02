/*
 * msg.c
 *
 *  Created on: 10.04.2018
 *      Author: sawadee
 */
/* Own Includes --------------------------------------------------------------------*/
#include "msg.h"
/* Macros and constants (#define) --------------------------------------------------*/
#define TRUE	1
#define FALSE	0
#define CAST_MSG_TO_EVENT(INDEX) ((INDEX) & ~ACTION_MSK)
#define CAST_MSG_TO_ACTION(INDEX) ((INDEX) | ACTION_MSK)
/* Private variables  --------------------------------------------------------------*/
stRingBuf msgBuffer;
/* Private functions (prototypes) --------------------------------------------------*/

static inline void clearMsg(msg_Message *message){
	message->type = 0;
	message->index = 0;
	message->origin = 0;
	message->value = 0;
	message->ctx = 0;
}

static inline uint8_t isSimilarToLast(msg_Message *message, msg_Buffer *buf){
	char type;
	uint16_t idx;
	if(RINGBUFFER_isEmpty(&buf->buffer)) return 0;
	uint16_t lastTmp = buf->lastMsg;
	type = buf->buffer.data[lastTmp];
	if(++lastTmp >= buf->buffer.size) lastTmp = 0;
	idx = (buf->buffer.data[lastTmp] << 8) & 0xFF00;
	if(++lastTmp >= buf->buffer.size) lastTmp = 0;
	idx |= (buf->buffer.data[lastTmp] << 0) & 0xFF;
	if((type == message->type) && (idx == message->index)) return 1;
	else return 0;
}

static inline uint8_t writeMsg(msg_Message *message, msg_Buffer *buf){
	uint8_t bufferState =  ringbuffer_NO_BUF;
	uint8_t isSimilarActive = 0;
	uint16_t nextMsgTmp = buf->buffer.in;
	if(message != NULL){
		if(isSimilarToLast(message, buf)){
			buf->buffer.in = buf->lastMsg;
			isSimilarActive = 1;
		}
		bufferState = RINGBUFFER_cIn(message->type, &buf->buffer);
		bufferState = RINGBUFFER_cIn((char)(message->index >> 8), &buf->buffer);
		bufferState = RINGBUFFER_cIn((char)(message->index), &buf->buffer);
		bufferState = RINGBUFFER_cIn((char)(message->value >> 8), &buf->buffer);
		bufferState = RINGBUFFER_cIn((char)(message->value), &buf->buffer);
		bufferState = RINGBUFFER_cIn((char)(message->ctx >> 8), &buf->buffer);
		bufferState = RINGBUFFER_cIn((char)(message->ctx), &buf->buffer);
		bufferState = RINGBUFFER_cIn(message->origin, &buf->buffer);
	}
	if(bufferState ==  ringbuffer_FULL) buf->buffer.in = nextMsgTmp;
	else if(!isSimilarActive) buf->lastMsg = nextMsgTmp;
	return bufferState;
}

void msg_initBuffer(char *pData, uint16_t size){
	msgBuffer.in = 0;
	msgBuffer.out = 0;
	msgBuffer.size = size;
	msgBuffer.data = pData;
}

uint8_t msg_writeEvent(msg_Message *message, msg_Buffer *buf){
	message->index = CAST_MSG_TO_EVENT(message->index);
	uint8_t bufferState = writeMsg(message, buf);
	return bufferState;
}

uint8_t msg_writeAction(msg_Message *message, msg_Buffer *buf){
	message->index = CAST_MSG_TO_ACTION(message->index);
	uint8_t bufferState = writeMsg(message, buf);
	return bufferState;
}

msg_Message *msg_getNext(msg_Message *message, msg_Buffer *buf){
	uint8_t bufferState =  ringbuffer_OK;
	char tmp = 0;
	bufferState = RINGBUFFER_cOut(&tmp,&buf->buffer);
	message->type = tmp;

	bufferState = RINGBUFFER_cOut(&tmp,&buf->buffer);
	message->index |= (uint16_t)((tmp & 0xFF) << 8);
	bufferState = RINGBUFFER_cOut(&tmp,&buf->buffer);
	message->index |= (uint16_t)(tmp & 0xFF);

	bufferState = RINGBUFFER_cOut(&tmp,&buf->buffer);
	message->value |= (uint16_t)((tmp & 0xFF) << 8);
	bufferState = RINGBUFFER_cOut(&tmp,&buf->buffer);
	message->value |= (uint16_t)(tmp & 0xFF);

	bufferState = RINGBUFFER_cOut(&tmp,&buf->buffer);
	message->ctx |= (uint16_t)((tmp & 0xFF) << 8);
	bufferState = RINGBUFFER_cOut(&tmp,&buf->buffer);
	message->ctx |= (uint16_t)(tmp & 0xFF);

	bufferState = RINGBUFFER_cOut(&tmp,&buf->buffer);
	message->origin = tmp;
	if(bufferState ==  ringbuffer_OK)return message;
	else return NULL;
}

uint8_t msg_isAction(msg_Message *message){
	if((message->index & ACTION_MSK) == ACTION_MSK) return TRUE;
	else return FALSE;
}
uint8_t msg_isEvent(msg_Message *message){
	if((message->index & ACTION_MSK) != ACTION_MSK) return TRUE;
	else return FALSE;
}

void msg_killBuffer(msg_Buffer *buf){
	buf->buffer.in = 0;
	buf->buffer.out = 0;
}

msg_Message *msg_init(msg_Message *buffer, char type, uint16_t index, int16_t value, char origin, int16_t ctx){
	if(buffer == NULL) return NULL;
	buffer->type = type;
	buffer->index = index;
	buffer->value = value;
	buffer->origin = origin;
	buffer->ctx = ctx;
	return buffer;
}
