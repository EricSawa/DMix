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
#define JSMN_HEADER
#include "jsmn.h"

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
eal_task_Task btnTask = {
		.id = 3, .enable = false,
		.init = app_btn_init, .mainLoop = app_btn_process, .timer = app_btn_cyclic1ms,
		.sendMsg = sendMessage, .receiveMsg = app_btn_receiveMsg
};
eal_task_Task footcontrolTask = {
		.id = 4, .enable = false,
		.init = app_footcontrol_init, .mainLoop = app_footcontrol_process, .timer = NULL,
		.sendMsg = sendMessage, .receiveMsg = app_footcontrol_receiveMsg
};
eal_task_Task interfaceTask = {
		.id = 5, .enable = false,
		.init = app_interface_init, .mainLoop = app_interface_process, .timer = NULL,
		.sendMsg = sendMessage, .receiveMsg = app_interface_receiveMsg
};
/******************************************************************************
* Function Definitions
*******************************************************************************/
#define SOFT_I2C	1
void app_main(){
	msgTask.enable = true;
	msgTask.init(&msgTask);
	while (1){
		msgTask.mainLoop(&msgTask);
		btnTask.mainLoop(&btnTask);
		footcontrolTask.mainLoop(&footcontrolTask);

		dmxTask.mainLoop(&dmxTask);
		dmxPresetTask.mainLoop(&dmxPresetTask);
		interfaceTask.mainLoop(&interfaceTask);
	 }
}

void app_1ms(){
	static uint16_t timer100ms = 0;
	static uint16_t timer500ms = 0;
	app_usb_1ms();
	dmx_1ms(&dmx1Out);
	if(dmxTask.timer) dmxTask.timer(&dmxTask, true);
	if(dmxPresetTask.timer) dmxPresetTask.timer(&dmxPresetTask, true);
	if(btnTask.timer) btnTask.timer(&btnTask, true);
	if(++timer100ms >= 100){
		timer100ms = 0;
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
	btnTask.receiveMsg(&btnTask, message);
	footcontrolTask.receiveMsg(&footcontrolTask, message);
	interfaceTask.receiveMsg(&interfaceTask, message);
}

static void app_init(eal_task_Task *self){
	i2c_soft_init(&softI2C2);
	eeprom_registerCallback(&eepromDev1, eepromCallback);
	eeprom_init(&eepromDev1);
	dmxTask.enable = true;
	if(dmxTask.init) dmxTask.init(&dmxTask);
	dmxPresetTask.enable = true;
	if(dmxPresetTask.init) dmxPresetTask.init(&dmxPresetTask);
	btnTask.enable = true;
	if(btnTask.init) btnTask.init(&btnTask);
	footcontrolTask.enable = true;
	if(footcontrolTask.init) footcontrolTask.init(&footcontrolTask);
	interfaceTask.enable = true;
	if(interfaceTask.init) interfaceTask.init(&interfaceTask);

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
		if(i2c_soft_mem_write(&softI2C2, config->devAddr, config->memAddr, I2C_MEMADD_SIZE_16BIT, config->txBuffer, config->dataSize) != soft_i2c_RETURN_OK) state = 0;
	}
	if(config->callbackType == EEPROM_CFG_CALL_RX_START){
		if(i2c_soft_mem_read(&softI2C2, config->devAddr, config->memAddr, I2C_MEMADD_SIZE_16BIT, config->rxBuffer, config->dataSize) != soft_i2c_RETURN_OK) state = 0;
	}
	if(config->callbackType == EEPROM_CFG_CALL_CHECK_TX_STATE){
		if(i2c_soft_write(&softI2C2, config->devAddr, config->rxBuffer, config->dataSize) != soft_i2c_RETURN_OK) state = 0;
	}
	return state;
}

/* IRQs ---------------------------------------------------------------------*/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	if(htim->Instance == dmx1Out.txTimer) dmx_irqTxTimerHandler(&dmx1Out);
}

void TIM1_UP_TIM16_IRQHandler(void){
  if (__HAL_TIM_GET_FLAG(&htim16, TIM_FLAG_UPDATE) != RESET){
	  __HAL_TIM_CLEAR_IT(&htim16, TIM_IT_UPDATE);
	  i2c_soft_timer_us(&softI2C2);
  }
}

bool usbConnected = false;
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	if(GPIO_Pin == I_USB_VBUS_Pin) {
		if(HAL_GPIO_ReadPin(I_USB_VBUS_GPIO_Port, I_USB_VBUS_Pin)) usbConnected = true;
		else usbConnected = false;
	}
}


