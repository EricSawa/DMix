/*
 * msg.h
 *
 *  Created on: 10.04.2018
 *      Author: sawadee
 */

#ifndef MSG_H_
#define MSG_H_

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <string.h>
#include "ringbuffer.h"
/************************************************************************************/
/* Internal messages ****************************************************************/
/************************************************************************************/
/* Types ---------------------------------------------------------------------*/
typedef struct{
	uint16_t index;
	int16_t value;
	char type;
	char origin;
	int16_t ctx;
}msg_Message;

typedef struct{
	stRingBuf buffer;
	uint16_t lastMsg;
}msg_Buffer;

/* Macros and constants (#define) --------------------------------------------------*/
#define msg_BUF(PBUF, SIZE) {.buffer.data = (PBUF), .buffer.size = (SIZE), .buffer.in = 0, .buffer.out = 0, .lastMsg = 0}
#define msg_LITERAL(TYPE, INDEX, VALUE, CTX, ORIGIN)	{.type = (TYPE), .index = (INDEX), .value = (VALUE), .ctx = (CTX), .origin =(ORIGIN)}
#define ACTION_MSK	(1 << 15)
#define msg_ACTION(INDEX) ((INDEX) | (ACTION_MSK))
#define msg_EVENT(INDEX) ((INDEX) & (~(ACTION_MSK)))
#define msg_KILL_ACTION_MSK(INDEX) msg_EVENT(INDEX)
/* Public functions (prototypes) ---------------------------------------------------*/
void msg_initBuffer(char *pData, uint16_t size);

uint8_t msg_writeEvent(msg_Message *message, msg_Buffer *buf);
uint8_t msg_writeAction(msg_Message *message, msg_Buffer *buf);
msg_Message *msg_getNext(msg_Message *message, msg_Buffer *buf);
uint8_t msg_isAction(msg_Message *message);
uint8_t msg_isEvent(msg_Message *message);

msg_Message *msg_getLast(msg_Message *message, msg_Buffer *buf);
void msg_killBuffer(msg_Buffer *buf);

msg_Message *msg_init(msg_Message *buffer, char type, uint16_t index, int16_t value, char origin, int16_t ctx);
#endif /* MSG_H_ */
