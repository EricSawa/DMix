/*
 * app_interface_cmd.h
 *
 *  Created on: 21.03.2020
 *      Author: erics
 */

#ifndef INTERFACE_APP_INTERFACE_CMD_H_
#define INTERFACE_APP_INTERFACE_CMD_H_
/******************************************************************************
* Includes
*******************************************************************************/
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "ringbuffer.h"
/******************************************************************************
* Constants
*******************************************************************************/
#define app_interface_cmd_LENGTH	16
#define app_interface_cmd_TYPE_POS	1
/******************************************************************************
* Typedefs
*******************************************************************************/
typedef enum{
	app_interface_WAIT_FOR_CMD, app_interface_REC_CMD
}app_interface_cmd_State;

typedef struct{
	char data[app_interface_cmd_LENGTH+1];
}app_interface_cmd_Cmd;

typedef struct{
	app_interface_cmd_State state;
	uint16_t cmdCnt;
	app_interface_cmd_Cmd cmd;
}app_interface_cmd_Interface;

typedef struct{
	stRingBuf buffer;
	uint16_t lastCmd;
}app_interface_cmd_Buffer;
/******************************************************************************
* Callback's
*******************************************************************************/

/******************************************************************************
* Function Prototypes
*******************************************************************************/
void app_interface_cmd_initBuffer(app_interface_cmd_Buffer *buf);
bool app_interface_cmd_writeCmd(app_interface_cmd_Cmd *cmd, app_interface_cmd_Buffer *buf);
app_interface_cmd_Cmd *app_interface_cmd_getNext(app_interface_cmd_Cmd *cmd, app_interface_cmd_Buffer *buf);
void app_interface_cmd_killBuffer(app_interface_cmd_Buffer *buf);

bool app_interface_cmd_parseCmd(char data, app_interface_cmd_Buffer *buf);
bool app_interface_cmd_isAck(app_interface_cmd_Cmd *cmd);
#endif /* INTERFACE_APP_INTERFACE_CMD_H_ */
