/*
 * app_interface.c
 *
 *  Created on: 12.01.2020
 *      Author: erics
 */
/******************************************************************************
* Includes
*******************************************************************************/
#include "app_interface.h"
#include <stdlib.h>
/******************************************************************************
* Configuration
*******************************************************************************/
#define UART_TX_SIZE 10240
#define UART_RX_SIZE 1024
char uart4TxData[UART_TX_SIZE] = {0};
char uart4RxData[UART_RX_SIZE] = {0};

stRingBuf uart4TxBuf = {.data = uart4TxData, .size = UART_TX_SIZE, .in = 0, .out = 0};
stRingBuf uart4RxBuf = {.data = uart4RxData, .size = UART_RX_SIZE, .in = 0, .out = 0};

#define USB_TX_SIZE 10240
#define USB_RX_SIZE 1024
char usbTxData[USB_TX_SIZE] = {0};
char usbRxData[USB_RX_SIZE] = {0};

typedef enum{
	app_interface_WAIT_FOR_CMD, app_interface_REC_CMD
}app_interface_CmdState;

typedef struct{
	app_interface_CmdState state;
	uint16_t cmdCnt;
	char cmd[app_cfg_CMD_LENGTH+1];
}app_interface_CmdInterface;

static app_interface_CmdInterface btInterface;
static app_interface_CmdInterface usbInterface;

static app_interface_CmdInterface *activeInterface;
/******************************************************************************
* Protocoll
*******************************************************************************/
#define TYPE_POS	1
#define UPLOAD_TIMER 10
/******************************************************************************
* Variable Declarations
*******************************************************************************/
static volatile bool cyclicFlag = false;
static volatile int16_t internalTimer = 0;
static int16_t uploadTimer = 0;

enum{
	upload_NONE, upload_PRESET_NAME, upload_PRESET_TRANSITION, upload_FOOTSWITCH
};

static uint8_t uploadState = upload_NONE;
static uint16_t uploadCmdCounter = 0;

#define PRESET_CMDS		2
#define PRESET_CMD_NMBR	(app_cfg_NMBR_PRESETS*PRESET_CMDS)

#define TRANSITION_CMDS	3
#define TRANSITION_CMD_NMBR	(app_cfg_NMBR_PRESETS*app_cfg_NMBR_TRANSITIONS*TRANSITION_CMDS)

#define FOOTSWITCH_CMDS	1
#define FOOTSWITCH_CMD_NMBR	(app_cfg_NMBR_BTNS*FOOTSWITCH_CMDS)

#define UPLOAD_CMD_MAX_NMBR	(PRESET_CMD_NMBR+TRANSITION_CMD_NMBR+FOOTSWITCH_CMD_NMBR)
/******************************************************************************
* Callback's
*******************************************************************************/

/******************************************************************************
* Function Prototypes
*******************************************************************************/
static void checkInterfaces(eal_task_Task *self);
static void parseCommand(eal_task_Task *self, app_interface_CmdInterface *interface, char data);
static bool commandFound(eal_task_Task *self, char *command);
static bool parseTransition(eal_task_Task *self, char *command);
static bool sendTransition(eal_task_Task *self, uint8_t index);
static bool parseFootswitch(eal_task_Task *self, char *command);
static bool sendFootswitch(eal_task_Task *self, uint8_t index);
static bool parsePreset(eal_task_Task *self, char *command);
static bool sendPresetName(eal_task_Task *self, uint8_t index);
static bool parseLoadConfig(eal_task_Task *self, char *command);

static void uploadConfig(eal_task_Task *self);
static void sendToInterface(eal_task_Task *self, app_interface_CmdInterface *interface, char *data);
static void sendDmxPreset(eal_task_Task *self, uint8_t preset);
/******************************************************************************
* Function Definitions
*******************************************************************************/
void app_interface_init(eal_task_Task *self){
	app_usb_init();
	app_usb_rxBufferInit(usbRxData, sizeof(usbRxData));
	app_usb_txBufferInit(usbTxData, sizeof(usbTxData));

	app_uart_bufferInit(&uart4TxBuf, uart4TxData, sizeof(uart4TxData));
	app_uart_bufferInit(&uart4RxBuf, uart4RxData, sizeof(uart4RxData));
	app_uart_enableReceiveInt(huart4.Instance);
}

void app_interface_process(eal_task_Task *self){
	if(!self->enable) return;
	checkInterfaces(self);
	if(!cyclicFlag) return;
	if(++uploadTimer >= UPLOAD_TIMER) {
		uploadConfig(self);
		uploadTimer = 0;
	}
	cyclicFlag = false;
}

void app_interface_cyclic1ms(eal_task_Task *self, bool stat){
	cyclicFlag = true;
	if(internalTimer > 0)--internalTimer;
}

void app_interface_receiveMsg(eal_task_Task *self, msg_Message *message){
}

/*Helper --------------------------------------------------------------------*/
static void checkInterfaces(eal_task_Task *self){
	app_usb_txLoop();
	char intData;
	if(app_usb_getCharFromRxBuffer(&intData) == ringbuffer_OK) parseCommand(self, &usbInterface, intData);
	if(app_uart_getCharFromBuffer(&intData, &uart4RxBuf) == ringbuffer_OK) parseCommand(self, &btInterface, intData);
}

static void parseCommand(eal_task_Task *self, app_interface_CmdInterface *interface, char data){
	if(data == '#'){
		interface->cmdCnt = 1;
		interface->state = app_interface_REC_CMD;
		interface->cmd[0] = data;
	}else if(interface->state == app_interface_REC_CMD){
		interface->cmd[interface->cmdCnt] = data;
		++interface->cmdCnt;
		if(interface->cmdCnt >= app_cfg_CMD_LENGTH){
			interface->state = app_interface_WAIT_FOR_CMD;
			if(commandFound(self, interface->cmd)){
				activeInterface = interface;
				//TODO: Maybe send to both interfaces.
				sendToInterface(self, activeInterface, interface->cmd);
			}
		}
	}
}

static bool commandFound(eal_task_Task *self, char *command){
	char type = command[TYPE_POS];
	if(type == 'T') return parseTransition(self, command);
	else if(type == 'F') return parseFootswitch(self, command);
	else if(type == 'P') return parsePreset(self, command);
	else if(type == 'L') return parseLoadConfig(self, command);
	return true;
}

static bool parseTransition(eal_task_Task *self, char *command){
	char cmdIdx = command[2];
	char prNrString[3];
	char trNrString[3];
	prNrString[0] = command[3]; prNrString[1] = command[4];
	trNrString[0] = command[5]; trNrString[1] = command[6];
	long prNr = strtol(prNrString, NULL, 16);
	long trNr = strtol(trNrString, NULL, 16);
	if(cmdIdx == '0'){
		char chString[3] = "";
		char startString[3] = "";
		chString[0] = command[7]; chString[1] = command[8];
		startString[0] = command[9]; startString[1] = command[10];
		long ch = strtol(chString, NULL, 16);
		long start = strtol(startString, NULL, 16);
		app_model_model.presets[prNr].presetTransitionSettings[trNr].dmxCh = ch;
		app_model_model.presets[prNr].presetTransitionSettings[trNr].dmxStartVal = start;
	}else if(cmdIdx == '1'){
		char endString[3] = "";
		char timeString[5] = "";
		endString[0] = command[7]; endString[1] = command[8];
		timeString[0] = command[9]; timeString[1] = command[10]; timeString[2] = command[11]; timeString[3] = command[12];
		long end = strtol(endString, NULL, 16);
		long time = strtol(timeString, NULL, 16);
		app_model_model.presets[prNr].presetTransitionSettings[trNr].dmxEndVal = end;
		app_model_model.presets[prNr].presetTransitionSettings[trNr].transTime_ms = time;
	}else if(cmdIdx == '2'){
		char delayString[5] = "";
		char rampString[3] = "";
		char activeString;
		delayString[0] = command[7]; delayString[1] = command[8]; delayString[2] = command[9]; delayString[3] = command[10];
		rampString[0] = command[11]; rampString[1] = command[12];
		activeString = command[13];
		long delay = strtol(delayString, NULL, 16);
		long ramp = strtol(rampString, NULL, 16);
		long active = strtol(&activeString, NULL, 16);
		app_model_model.presets[prNr].presetTransitionSettings[trNr].transDelay_ms = delay;
		app_model_model.presets[prNr].presetTransitionSettings[trNr].transRamp = ramp;
		app_model_model.presets[prNr].presetTransitionSettings[trNr].enable = active;
		app_model_model.presets[prNr].transitionEnableSize = 0;
		for(int j = 0; j < app_dmx_preset_cfg_NMBR_TRANSITIONS; ++j){
			if(app_model_model.presets[prNr].presetTransitionSettings[j].enable) ++app_model_model.presets[prNr].transitionEnableSize;
		}
		app_model_model.dirty = true;
	}
	return true;
}
static bool sendTransition(eal_task_Task *self, uint8_t index){
	uint8_t prNr = index/app_cfg_NMBR_TRANSITIONS;
	uint8_t trNr = index - (prNr*app_cfg_NMBR_TRANSITIONS);
	uint8_t ch = app_model_model.presets[prNr].presetTransitionSettings[trNr].dmxCh;
	uint8_t start = app_model_model.presets[prNr].presetTransitionSettings[trNr].dmxStartVal;
	uint8_t end = app_model_model.presets[prNr].presetTransitionSettings[trNr].dmxEndVal;
	uint16_t time = app_model_model.presets[prNr].presetTransitionSettings[trNr].transTime_ms;
	uint16_t delay = app_model_model.presets[prNr].presetTransitionSettings[trNr].transDelay_ms;
	uint8_t ramp = app_model_model.presets[prNr].presetTransitionSettings[trNr].transRamp;
	bool active = app_model_model.presets[prNr].presetTransitionSettings[trNr].enable;
	char cmdString[15] = "";
	snprintf(cmdString, sizeof(cmdString),"#T0%02x%02x%02x%02x000\r", prNr, trNr, ch, start);
	cmdString[14] = '\r';
	sendToInterface(self, activeInterface, cmdString);
	snprintf(cmdString, sizeof(cmdString),"#T1%02x%02x%02x%04x0\r", prNr, trNr, end, time);
	cmdString[14] = '\r';
	sendToInterface(self, activeInterface, cmdString);
	snprintf(cmdString, sizeof(cmdString),"#T2%02x%02x%04x%02x%01x\r", prNr, trNr, delay, ramp, active);
	cmdString[14] = '\r';
	sendToInterface(self, activeInterface, cmdString);
	return true;
}

static bool parseFootswitch(eal_task_Task *self, char *command){
	char cmdIdx = command[2];
	char fNrString[3];
	fNrString[0] = command[3]; fNrString[1] = command[4];
	long fNr = strtol(fNrString, NULL, 16);
	if(cmdIdx == '0'){
		char pressedString[3];
		char releasedString[3];
		char holdshortString[3];
		char holdlongString[3];
		pressedString[0] = command[5]; pressedString[1] = command[6];
		releasedString[0] = command[7]; releasedString[1] = command[8];
		holdshortString[0] = command[9]; holdshortString[1] = command[10];
		holdlongString[0] = command[11]; holdlongString[1] = command[12];
		long pressed = strtol(pressedString, NULL, 16);
		long released = strtol(releasedString, NULL, 16);
		long holdshort = strtol(holdshortString, NULL, 16);
		long holdlong = strtol(holdlongString, NULL, 16);
		app_model_model.footswitches[fNr].press.presetNr = pressed;
		app_model_model.footswitches[fNr].release.presetNr = released;
		app_model_model.footswitches[fNr].holdShort.presetNr = holdshort;
		app_model_model.footswitches[fNr].holdLong.presetNr = holdlong;
		app_model_model.dirty = true;
	}
	return true;
}
static bool sendFootswitch(eal_task_Task *self, uint8_t index){
	char cmdString[15] = "";
	uint8_t pressed = app_model_model.footswitches[index].press.presetNr;
	uint8_t released = app_model_model.footswitches[index].release.presetNr;
	uint8_t holdshort = app_model_model.footswitches[index].holdShort.presetNr;
	uint8_t holdlong = app_model_model.footswitches[index].holdLong.presetNr;
	snprintf(cmdString, sizeof(cmdString),"#F0%02x%02x%02x%02x%02x0\r", index, pressed, released, holdshort, holdlong);
	cmdString[14] = '\r';
	sendToInterface(self, activeInterface, cmdString);
	return true;
}

static bool parsePreset(eal_task_Task *self, char *command){
	char cmdIdx = command[2];
	char prNrString[3];
	prNrString[0] = command[3]; prNrString[1] = command[4];
	long prNr = strtol(prNrString, NULL, 16);
	if(cmdIdx == '0'){
		strncpy (&app_model_model.presets[prNr].name[0], &command[5], 9);
	}else if(cmdIdx == '1'){
		strncpy (&app_model_model.presets[prNr].name[9], &command[5], 9);
		app_model_model.dirty = true;
	}else if(cmdIdx == 'T'){
		sendDmxPreset(self, prNr);
	}
	return true;
}
static bool sendPresetName(eal_task_Task *self, uint8_t index){
	char cmdString[15] = {' '};
	memset(cmdString,' ', sizeof(cmdString));
	snprintf(cmdString, 6,"#P0%02x", index);
	cmdString[5] = ' ';
	uint8_t length = strlen(&app_model_model.presets[index].name[0]);
	if(length > 9) length = 9;
	strncpy(&cmdString[5], &app_model_model.presets[index].name[0], length);
	cmdString[14] = '\r';
	sendToInterface(self, activeInterface, cmdString);
	memset(cmdString,' ', sizeof(cmdString));
	snprintf(cmdString, 6,"#P1%02x", index);
	cmdString[5] = ' ';
	length = strlen(&app_model_model.presets[index].name[9]);
	if(length > 9) length = 9;
	strncpy(&cmdString[5], &app_model_model.presets[index].name[9], length);
	cmdString[14] = '\r';
	sendToInterface(self, activeInterface, cmdString);
	return true;
}

static bool parseLoadConfig(eal_task_Task *self, char *command){
	char cmdIdx = command[2];
	if(cmdIdx == '0'){
		uploadCmdCounter = 0;
		uploadState = upload_PRESET_NAME;
	}
	return true;
}

static void uploadConfig(eal_task_Task *self){
	if(uploadState == upload_NONE) return;
	else if(uploadState == upload_PRESET_NAME){
		sendPresetName(self, uploadCmdCounter);
		if(++uploadCmdCounter >= app_cfg_NMBR_PRESETS){
			uploadCmdCounter = 0;
			uploadState = upload_PRESET_TRANSITION;
		}
	}else if(uploadState == upload_PRESET_TRANSITION){
		sendTransition(self, uploadCmdCounter);
		if(++uploadCmdCounter >= app_cfg_NMBR_PRESETS*app_cfg_NMBR_TRANSITIONS){
			uploadCmdCounter = 0;
			uploadState = upload_FOOTSWITCH;
		}
	}else if(uploadState == upload_FOOTSWITCH){
		sendFootswitch(self, uploadCmdCounter);
		if(++uploadCmdCounter >= app_cfg_NMBR_BTNS-1){
			uploadCmdCounter = 0;
			uploadState = upload_NONE;
		}
	}
}

/* Helper ----------------------------------------------------------------------*/
static void sendToInterface(eal_task_Task *self, app_interface_CmdInterface *interface, char *data){
	if(interface == &usbInterface) {
		app_usb_writeStringToBuffer(data);
		//TODO: Write new line
	}
	if(interface == &btInterface) {
		app_uart_writeStringToBuffer(data, &uart4TxBuf, huart4.Instance);
		//TODO: Write new line
	}
}

static void sendDmxPreset(eal_task_Task *self, uint8_t preset){
	if(self->sendMsg == NULL) return;
	msg_Message newMessage = msg_LITERAL(app_cfg_DMX_PRESET_MSG, msg_ACTION(preset), app_cfg_dmx_preset_TRIGGER_ENABLE, 0, 0);
	self->sendMsg(self, &newMessage);
}

/*IRQs ----------------------------------------------------------------------*/
void UART4_IRQHandler(void){
	app_uart_irqHandler(huart4.Instance, &uart4RxBuf, &uart4TxBuf);
}


