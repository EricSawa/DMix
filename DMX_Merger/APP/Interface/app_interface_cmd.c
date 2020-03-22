/*
 * app_interface_cmd.c
 *
 *  Created on: 21.03.2020
 *      Author: erics
 */
/******************************************************************************
* Includes
*******************************************************************************/
#include "app_interface_cmd.h"
/******************************************************************************
* Configuration
*******************************************************************************/
stRingBuf cmdBuffer;
/******************************************************************************
* Protocoll
*******************************************************************************/

/******************************************************************************
* Variable Declarations
*******************************************************************************/

/******************************************************************************
* Callback's
*******************************************************************************/

/******************************************************************************
* Function Prototypes
*******************************************************************************/

/******************************************************************************
* Function Definitions
*******************************************************************************/
void app_interface_cmd_initBuffer(app_interface_cmd_Buffer *buf){
	buf->buffer.in = 0;
	buf->buffer.out = 0;
}

bool app_interface_cmd_writeCmd(app_interface_cmd_Cmd *cmd, app_interface_cmd_Buffer *buf){
	uint8_t bufferState =  ringbuffer_NO_BUF;
	uint16_t nextMsgTmp = buf->buffer.in;
	if(cmd != NULL){
		for(int i = 0; i < (app_interface_cmd_LENGTH+1); ++i){
			bufferState = RINGBUFFER_cIn(cmd->data[i], &buf->buffer);
		}
	}
	if(bufferState == ringbuffer_FULL) buf->buffer.in = nextMsgTmp;
	if(bufferState == ringbuffer_OK) return true;
	return false;
}

app_interface_cmd_Cmd *app_interface_cmd_getNext(app_interface_cmd_Cmd *cmd, app_interface_cmd_Buffer *buf){
	uint8_t bufferState =  ringbuffer_OK;
	memset(cmd->data, 0, sizeof(cmd->data));
	for(int i = 0; i < (app_interface_cmd_LENGTH+1); ++i){
		bufferState = RINGBUFFER_cOut(&cmd->data[i],&buf->buffer);
	}
	if(bufferState == ringbuffer_OK)return cmd;
	else return NULL;
}

void app_interface_cmd_killBuffer(app_interface_cmd_Buffer *buf){
	buf->buffer.in = 0;
	buf->buffer.out = 0;
}

bool app_interface_cmd_parseCmd(char data, app_interface_cmd_Buffer *buf){
	static app_interface_cmd_State state = app_interface_WAIT_FOR_CMD;
	static uint16_t cmdCnt = 0;
	static app_interface_cmd_Cmd newCmd;
	if(data == '#'){
		cmdCnt = 1;
		state = app_interface_REC_CMD;

		newCmd.data[0] = data;
	}else if(state == app_interface_REC_CMD){
		newCmd.data[cmdCnt] = data;
		++cmdCnt;
		if(cmdCnt >= app_interface_cmd_LENGTH){
			state = app_interface_WAIT_FOR_CMD;
			app_interface_cmd_writeCmd(&newCmd, buf);
			return true;
		}
	}
	return false;
}

bool app_interface_cmd_isAck(app_interface_cmd_Cmd *cmd){
	if(cmd->data[1] == 'A') return true;
	else return false;
}
