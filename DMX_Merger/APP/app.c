/*
 * app.c
 *
 *  Created on: 22.11.2019
 *      Author: erics
 */
/******************************************************************************
* Includes
*******************************************************************************/
#include <stdint.h>
#include <stdlib.h>
#include "main.h"
#include "app.h"
/******************************************************************************
* Constants
*******************************************************************************/

/******************************************************************************
* Macros
*******************************************************************************/

/******************************************************************************
* Configuration
*******************************************************************************/

/******************************************************************************
* Typedefs
*******************************************************************************/
/******************************************************************************
* Function Prototypes
*******************************************************************************/
/* Callback's ----------------------------------------------------------------*/
uint8_t eepromCallback(eeprom_cfg_Config *config);
/* Helper --------------------------------------------------------------------*/
static void sendMessage(eal_task_Task *self, msg_Message *message);
static void messageBufferError(uint8_t state);

static void app_init(eal_task_Task *self);
static void app_process(eal_task_Task *self);
static void app_receiveMsg(eal_task_Task *self, msg_Message *message);
/******************************************************************************
* Variable Declarations
*******************************************************************************/
/* Buffer --------------------------------------------------------------------*/
#define MSG_BUF_SIZE 512
char MSG_Data[MSG_BUF_SIZE] = {0};
msg_Buffer msgBuf = msg_BUF(MSG_Data, MSG_BUF_SIZE);

#define UART_TX_SIZE 1024
#define UART_RX_SIZE 1024
char uart4TxData[UART_TX_SIZE] = {0};
char uart4RxData[UART_RX_SIZE] = {0};

stRingBuf uart4TxBuf = {.data = uart4TxData, .size = UART_TX_SIZE, .in = 0, .out = 0};
stRingBuf uart4RxBuf = {.data = uart4RxData, .size = UART_RX_SIZE, .in = 0, .out = 0};

/* Tasks ---------------------------------------------------------------------*/
eal_task_Task msgTask = {
		.id = 0, .enable = false,
		.init = app_init, .mainLoop = app_process, .timer = NULL,
		.sendMsg = sendMessage, .receiveMsg = app_receiveMsg
};
eal_task_Task dmxPresetTask = {
		.id = 1, .enable = false,
		.init = app_dmx_preset_init, .mainLoop = app_dmx_preset_process, .timer = app_dmx_preset_cyclic1ms,
		.sendMsg = sendMessage, .receiveMsg = app_dmx_preset_receiveMsg
};
eal_task_Task dmxTask = {
		.id = 2, .enable = false,
		.init = app_dmx_init, .mainLoop = app_dmx_process, .timer = app_dmx_cyclic1ms,
		.sendMsg = sendMessage, .receiveMsg = app_dmx_receiveMsg
};

/******************************************************************************
* Function Definitions
*******************************************************************************/
void app_main(){
	msgTask.enable = true;
	msgTask.init(&msgTask);

	while (1){
		msgTask.mainLoop(&msgTask);
		dmxTask.mainLoop(&dmxTask);
		dmxPresetTask.mainLoop(&dmxPresetTask);
		if((HAL_GPIO_ReadPin(I_SW2_GPIO_Port, I_SW2_Pin) == GPIO_PIN_RESET) && (myPresets[0].trigger != true)){
			msg_Message newMessage = msg_LITERAL(app_cfg_DMX_PRESET_MSG, msg_ACTION(0), app_cfg_dmx_preset_TRIGGER_ENABLE, 0, 0);
			sendMessage(NULL, &newMessage);
		}

	 }
}

void app_1ms(){
	static uint16_t timer100ms = 0;
	static uint16_t timer500ms = 0;
	dmx_1ms(&dmx1);
	if(dmxTask.timer) dmxTask.timer(&dmxTask, true);
	if(dmxPresetTask.timer) dmxPresetTask.timer(&dmxPresetTask, true);
	if(++timer100ms >= 100){
		timer100ms = 0;
//		uart_writeStringToBuffer("AT+NAMEERIC12  ", &uart4TxBuf, huart4.Instance);
//		uart_writeStringToBuffer("AT+NAMEERIC12  ", &uart2TxBuf, huart2.Instance);
	}
	if(++timer500ms >= 500){
		timer500ms = 0;
	}

}
/* Application Task ------------------------------------------------------------*/
static void app_process(eal_task_Task *self){
	msg_Message buffer = {0};
	msg_Message *newMessage = msg_getNext(&buffer, &msgBuf);
	if(newMessage != NULL){
		self->receiveMsg(self, newMessage);
	}
}
static void app_receiveMsg(eal_task_Task *self, msg_Message *message){
	dmxTask.receiveMsg(&dmxTask, message);
	dmxPresetTask.receiveMsg(&dmxPresetTask, message);
}

static void app_init(eal_task_Task *self){


	eeprom_registerCallback(&eepromDev1, eepromCallback);
	eeprom_init(&eepromDev1);


	dmxTask.enable = true;
	if(dmxTask.init) dmxTask.init(&dmxTask);
	dmxPresetTask.enable = true;
	if(dmxPresetTask.init) dmxPresetTask.init(&dmxPresetTask);
}

/* Messages --------------------------------------------------------------------*/
static void sendMessage(eal_task_Task *self, msg_Message *message){
	uint8_t msgBufState = 1;
	if(msg_isAction(message))msgBufState = msg_writeAction(message, &msgBuf);
	else msgBufState = msg_writeEvent(message, &msgBuf);
	if(msgBufState != 1) messageBufferError(msgBufState);
}
static void messageBufferError(uint8_t state){
//	if(state == NO_BUF) USART3_WriteStringToBuffer("ERROR: MSG NO BUF\r\n", 18);
//	else if(state == FULL) USART3_WriteStringToBuffer("ERROR: MSG BUF OVR\r\n", 19);
}

/* Callback's ---------------------------------------------------------------------*/
uint8_t eepromCallback(eeprom_cfg_Config *config){
	uint8_t state = 1;
	if(config->callbackType == EEPROM_CFG_CALL_TX_START){
		if(HAL_I2C_Mem_Write(&hi2c2, config->devAddr, config->memAddr, I2C_MEMADD_SIZE_16BIT, config->txBuffer, config->dataSize, 100) != HAL_OK) state = 0;
	}
	if(config->callbackType == EEPROM_CFG_CALL_RX_START){
		if(HAL_I2C_Mem_Read(&hi2c2, config->devAddr, config->memAddr, I2C_MEMADD_SIZE_16BIT, config->rxBuffer, config->dataSize, 10) != HAL_OK) state = 0;
	}
	if(config->callbackType == EEPROM_CFG_CALL_CHECK_TX_STATE){
		if(HAL_I2C_Master_Transmit(&hi2c2, config->devAddr, config->rxBuffer, config->dataSize, 1) != HAL_OK) state = 0;
	}
	return state;
}

/* IRQs ---------------------------------------------------------------------*/
void UART4_IRQHandler(void){
	app_uart_irqHandler(huart4.Instance, &uart4RxBuf, &uart4TxBuf);
}




