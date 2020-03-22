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
#include "app_interface_cmd.h"
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

#define USB_TX_SIZE 1024
#define USB_RX_SIZE 1024
char usbTxData[USB_TX_SIZE] = {0};
char usbRxData[USB_RX_SIZE] = {0};

#define CMD_TX_SIZE 10240
char cmdTxData[CMD_TX_SIZE] = {0};
#define CMD_RX_SIZE 10240
char cmdRxData[CMD_RX_SIZE] = {0};
app_interface_cmd_Buffer cmdTxBuf = {.buffer.data = cmdTxData, .buffer.size = CMD_TX_SIZE, .buffer.in = 0, .buffer.out = 0};
app_interface_cmd_Buffer cmdRxBuf = {.buffer.data = cmdRxData, .buffer.size = CMD_RX_SIZE, .buffer.in = 0, .buffer.out = 0};

static app_interface_cmd_Interface btInterface;
static app_interface_cmd_Interface usbInterface;
static app_interface_cmd_Interface *activeInterface;
/******************************************************************************
* Protocoll
*******************************************************************************/
#define UPLOAD_TIMER 1
#define SEND_TIMER 1
/******************************************************************************
* Variable Declarations
*******************************************************************************/
static volatile bool cyclicFlag = false;
static volatile int16_t internalTimer = 0;
static int16_t uploadTimer = 0;
static int16_t sendTimer = 0;

enum{
	upload_NONE, upload_PRESET_NAME, upload_PRESET_TRANSITION, upload_FOOTSWITCH
};

static bool waitForAck = false;
static uint8_t uploadState = upload_NONE;
static uint16_t uploadCmdCounter = 0;
/******************************************************************************
* Callback's
*******************************************************************************/

/******************************************************************************
* Function Prototypes
*******************************************************************************/
static void sendToInterface();
static void checkInterfaces(eal_task_Task *self);
static void usbToUartBridge(eal_task_Task *self);
static bool commandFound(eal_task_Task *self, app_interface_cmd_Cmd *command);
static bool parseTransition(eal_task_Task *self, app_interface_cmd_Cmd *command);
static bool sendTransition(eal_task_Task *self, uint8_t index);
static bool parseFootswitch(eal_task_Task *self, app_interface_cmd_Cmd *command);
static bool sendFootswitch(eal_task_Task *self, uint8_t index);
static bool parsePreset(eal_task_Task *self, app_interface_cmd_Cmd *command);
static bool sendPresetName(eal_task_Task *self, uint8_t index);
static bool parseLoadConfig(eal_task_Task *self, app_interface_cmd_Cmd *command);

static void signalizeCommuniction();
static void signalizeError();
static void uploadConfig();
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

	cyclicFlag = false;
	internalTimer = 0;
	uploadTimer = 0;
	waitForAck = false;
	uploadState = upload_NONE;
	uploadCmdCounter = 0;
}

void app_interface_process(eal_task_Task *self){
	if(!self->enable) return;
	app_usb_txLoop();
//	sendToInterface();
	checkInterfaces(self);
	//usbToUartBridge(self);
	if(!cyclicFlag) return;
	if(++sendTimer >= SEND_TIMER){
		sendToInterface();
		sendTimer = 0;
	}
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

/******************************************************************************
* USB BT Bridge
*******************************************************************************/
static void usbToUartBridge(eal_task_Task *self){
	app_usb_txLoop();
	char intData;
	if(app_usb_getCharFromRxBuffer(&intData) == ringbuffer_OK) app_uart_writeCharToBuffer(intData, &uart4TxBuf, huart4.Instance);
	if(app_uart_getCharFromBuffer(&intData, &uart4RxBuf) == ringbuffer_OK) app_usb_writeCharToBuffer(intData);
}

/******************************************************************************
* Normal receiver
*******************************************************************************/
static void sendToInterface(){
	app_interface_cmd_Cmd newCmd;
	if(app_interface_cmd_getNext(&newCmd, &cmdTxBuf)){
		signalizeCommuniction();
		if(!app_interface_cmd_isAck(&newCmd)) waitForAck = true;
		if(activeInterface == &usbInterface){
			if(app_usb_writeStringToBuffer(newCmd.data) != ringbuffer_OK){
				signalizeError();
			}
		}else if(activeInterface == &btInterface){
			if(app_uart_writeStringToBuffer(newCmd.data, &uart4TxBuf, huart4.Instance) != ringbuffer_OK){
				signalizeError();
			}
		}
	}
}

static void checkInterfaces(eal_task_Task *self){
	char intData;
	/*Receive char and check if command was found*/
	if(app_usb_getCharFromRxBuffer(&intData) == ringbuffer_OK) {
		if(app_interface_cmd_parseCmd(intData, &cmdRxBuf)){
			activeInterface = &usbInterface;
		}
	}
	if(app_uart_getCharFromBuffer(&intData, &uart4RxBuf) == ringbuffer_OK) {
		if(app_interface_cmd_parseCmd(intData, &cmdRxBuf)){
			activeInterface = &btInterface;
		}
	}
	/*Check if a full command was received*/
	app_interface_cmd_Cmd newCmd;
	if(app_interface_cmd_getNext(&newCmd, &cmdRxBuf)){
		signalizeCommuniction();
		commandFound(self, &newCmd);
		/*Send acknowledge*/
		if(!app_interface_cmd_isAck(&newCmd)){
			app_interface_cmd_Cmd ack = {.data = "#A000000000000\r\n"};
			if(!app_interface_cmd_writeCmd(&ack, &cmdTxBuf)) signalizeError();
		}
	}
}


static bool commandFound(eal_task_Task *self, app_interface_cmd_Cmd *command){
	char type = command->data[app_interface_cmd_TYPE_POS];
	if(type == 'T') return parseTransition(self, command);
	else if(type == 'F') return parseFootswitch(self,command);
	else if(type == 'P') return parsePreset(self,command);
	else if(type == 'L') {
		app_interface_cmd_killBuffer(&cmdTxBuf);
		waitForAck = false;
		return parseLoadConfig(self,command);
	}
	else if(type == 'A'){
		waitForAck = false;
		return true;
	}
	return true;
}

static bool parseTransition(eal_task_Task *self, app_interface_cmd_Cmd *command){
	char cmdIdx = command->data[2];
	char prNrString[3];
	char trNrString[3];
	prNrString[0] = command->data[3]; prNrString[1] = command->data[4];
	trNrString[0] = command->data[5]; trNrString[1] = command->data[6];
	long prNr = strtol(prNrString, NULL, 16);
	long trNr = strtol(trNrString, NULL, 16);
	if(cmdIdx == '0'){
		char chString[3] = "";
		char startString[3] = "";
		chString[0] = command->data[7]; chString[1] = command->data[8];
		startString[0] = command->data[9]; startString[1] = command->data[10];
		long ch = strtol(chString, NULL, 16);
		long start = strtol(startString, NULL, 16);
		app_model_model.presets[prNr].presetTransitionSettings[trNr].dmxCh = ch;
		app_model_model.presets[prNr].presetTransitionSettings[trNr].dmxStartVal = start;
	}else if(cmdIdx == '1'){
		char endString[3] = "";
		char timeString[5] = "";
		endString[0] = command->data[7]; endString[1] = command->data[8];
		timeString[0] = command->data[9]; timeString[1] = command->data[10]; timeString[2] = command->data[11]; timeString[3] = command->data[12];
		long end = strtol(endString, NULL, 16);
		long time = strtol(timeString, NULL, 16);
		app_model_model.presets[prNr].presetTransitionSettings[trNr].dmxEndVal = end;
		app_model_model.presets[prNr].presetTransitionSettings[trNr].transTime_ms = time;
	}else if(cmdIdx == '2'){
		char delayString[5] = "";
		char rampString[3] = "";
		char activeString;
		delayString[0] = command->data[7]; delayString[1] = command->data[8]; delayString[2] = command->data[9]; delayString[3] = command->data[10];
		rampString[0] = command->data[11]; rampString[1] = command->data[12];
		activeString = command->data[13];
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
	app_interface_cmd_Cmd newCmd;
	snprintf(newCmd.data, sizeof(newCmd.data),"#T0%02x%02x%02x%02x000\r", prNr, trNr, ch, start);
	newCmd.data[14] = '\r';
	newCmd.data[15] = '\n';
	newCmd.data[16] = '\0';
	if(!app_interface_cmd_writeCmd(&newCmd, &cmdTxBuf)) signalizeError();
	snprintf(newCmd.data, sizeof(newCmd.data),"#T1%02x%02x%02x%04x0\r", prNr, trNr, end, time);
	newCmd.data[14] = '\r';
	newCmd.data[15] = '\n';
	newCmd.data[16] = '\0';
	if(!app_interface_cmd_writeCmd(&newCmd, &cmdTxBuf)) signalizeError();
	snprintf(newCmd.data, sizeof(newCmd.data),"#T2%02x%02x%04x%02x%01x\r", prNr, trNr, delay, ramp, active);
	newCmd.data[14] = '\r';
	newCmd.data[15] = '\n';
	newCmd.data[16] = '\0';
	if(!app_interface_cmd_writeCmd(&newCmd, &cmdTxBuf)) signalizeError();
	return true;
}

static bool parseFootswitch(eal_task_Task *self, app_interface_cmd_Cmd *command){
	char cmdIdx = command->data[2];
	char fNrString[3];
	fNrString[0] = command->data[3]; fNrString[1] = command->data[4];
	long fNr = strtol(fNrString, NULL, 16);
	if(cmdIdx == '0'){
		char pressedString[3];
		char releasedString[3];
		char holdshortString[3];
		char holdlongString[3];
		pressedString[0] = command->data[5]; pressedString[1] = command->data[6];
		releasedString[0] = command->data[7]; releasedString[1] = command->data[8];
		holdshortString[0] = command->data[9]; holdshortString[1] = command->data[10];
		holdlongString[0] = command->data[11]; holdlongString[1] = command->data[12];
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
	app_interface_cmd_Cmd newCmd;
	uint8_t pressed = app_model_model.footswitches[index].press.presetNr;
	uint8_t released = app_model_model.footswitches[index].release.presetNr;
	uint8_t holdshort = app_model_model.footswitches[index].holdShort.presetNr;
	uint8_t holdlong = app_model_model.footswitches[index].holdLong.presetNr;
	snprintf(newCmd.data, sizeof(newCmd.data),"#F0%02x%02x%02x%02x%02x0\r", index, pressed, released, holdshort, holdlong);
	newCmd.data[14] = '\r';
	newCmd.data[15] = '\n';
	newCmd.data[16] = '\0';
	if(!app_interface_cmd_writeCmd(&newCmd, &cmdTxBuf)) signalizeError();
	return true;
}

static bool parsePreset(eal_task_Task *self, app_interface_cmd_Cmd *command){
	char cmdIdx = command->data[2];
	char prNrString[3];
	prNrString[0] = command->data[3]; prNrString[1] = command->data[4];
	long prNr = strtol(prNrString, NULL, 16);
	if(cmdIdx == '0'){
		strncpy (&app_model_model.presets[prNr].name[0], &command->data[5], 9);
	}else if(cmdIdx == '1'){
		strncpy (&app_model_model.presets[prNr].name[9], &command->data[5], 9);
		app_model_model.dirty = true;
	}else if(cmdIdx == 'T'){
		sendDmxPreset(self, prNr);
	}
	return true;
}
static bool sendPresetName(eal_task_Task *self, uint8_t index){
	app_interface_cmd_Cmd newCmd;
	memset(newCmd.data,' ', sizeof(newCmd.data));
	snprintf(newCmd.data, 6,"#P0%02x", index);
	newCmd.data[5] = ' ';
	uint8_t length = strlen(&app_model_model.presets[index].name[0]);
	if(length > 9) length = 9;
	strncpy(&newCmd.data[5], &app_model_model.presets[index].name[0], length);
	newCmd.data[14] = '\r';
	newCmd.data[15] = '\n';
	newCmd.data[16] = '\0';
	app_interface_cmd_writeCmd(&newCmd, &cmdTxBuf);
	memset(newCmd.data,' ', sizeof(newCmd.data));
	snprintf(newCmd.data, 6,"#P1%02x", index);
	newCmd.data[5] = ' ';
	length = strlen(&app_model_model.presets[index].name[9]);
	if(length > 9) length = 9;
	strncpy(&newCmd.data[5], &app_model_model.presets[index].name[9], length);
	newCmd.data[14] = '\r';
	newCmd.data[15] = '\n';
	newCmd.data[16] = '\0';
	if(!app_interface_cmd_writeCmd(&newCmd, &cmdTxBuf)) signalizeError();
	return true;
}

static bool parseLoadConfig(eal_task_Task *self, app_interface_cmd_Cmd *command){
	char cmdIdx = command->data[2];
	if(cmdIdx == '0'){
		uploadCmdCounter = 0;
		uploadState = upload_PRESET_NAME;
	}
	return true;
}

static void uploadConfig(eal_task_Task *self){
	if(uploadState == upload_NONE) return;
	else if(uploadState == upload_PRESET_NAME){
		sendPresetName(self,uploadCmdCounter);
		if(++uploadCmdCounter >= app_cfg_NMBR_PRESETS){
			uploadCmdCounter = 0;
			uploadState = upload_PRESET_TRANSITION;
		}
	}else if(uploadState == upload_PRESET_TRANSITION){
		sendTransition(self,uploadCmdCounter);
		if(++uploadCmdCounter >= app_cfg_NMBR_PRESETS*app_cfg_NMBR_TRANSITIONS){
			uploadCmdCounter = 0;
			uploadState = upload_FOOTSWITCH;
		}
	}else if(uploadState == upload_FOOTSWITCH){
		sendFootswitch(self,uploadCmdCounter);
		if(++uploadCmdCounter >= app_cfg_NMBR_BTNS-1){
			uploadCmdCounter = 0;
			uploadState = upload_NONE;
		}
	}
}

/* Helper ----------------------------------------------------------------------*/
static void sendDmxPreset(eal_task_Task *self, uint8_t preset){
	if(self->sendMsg == NULL) return;
	msg_Message newMessage = msg_LITERAL(app_cfg_DMX_PRESET_MSG, msg_ACTION(preset), app_cfg_dmx_preset_TRIGGER_ENABLE, 0, 0);
	self->sendMsg(self, &newMessage);
}

static void signalizeCommuniction(){
	 HAL_GPIO_TogglePin(O_PWM_LED1_GPIO_Port, O_PWM_LED1_Pin);
}
static void signalizeError(){
	 HAL_GPIO_WritePin(O_PWM_LED2_GPIO_Port, O_PWM_LED2_Pin, GPIO_PIN_SET);
}
/*IRQs ----------------------------------------------------------------------*/
void UART4_IRQHandler(void){
	app_uart_irqHandler(huart4.Instance, &uart4RxBuf, &uart4TxBuf);
}


