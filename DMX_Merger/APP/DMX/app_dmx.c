/*
 * app_dmx.c
 *
 *  Created on: 31.12.2019
 *      Author: erics
 */
/******************************************************************************
* Includes
*******************************************************************************/
#include "app_dmx.h"
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
bool dmx1TransmitterCallback(dmx_cfg_Instance *dmx);
bool dmx2ReceiverCallback(dmx_cfg_Instance *dmx);
/******************************************************************************
* Variable Declarations
*******************************************************************************/
static volatile bool cyclicFlag = false;
static volatile int16_t internalTimer = 0;
/******************************************************************************
* Function Definitions
*******************************************************************************/
void app_dmx_init(eal_task_Task *self){
	dmx_registerCallback(&dmx1, dmx1TransmitterCallback);
	dmx_init(&dmx1);
	dmx_txStart(&dmx1);

	dmx_registerCallback(&dmx2, dmx2ReceiverCallback);
	dmx_init(&dmx2);
	dmx_rxStart(&dmx2);

	HAL_GPIO_WritePin(O_USART1_DIR_GPIO_Port, O_USART1_DIR_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(O_USART2_DIR_GPIO_Port, O_USART2_DIR_Pin, GPIO_PIN_SET);
}

void app_dmx_process(eal_task_Task *self){
	if(!self->enable) return;
	if(!cyclicFlag) return;
	/*Do everything what you want to do*/
	dmx_loop(&dmx1);
	dmx_loop(&dmx2);
	cyclicFlag = false;
}

void app_dmx_cyclic1ms(eal_task_Task *self, bool stat){
	cyclicFlag = true;
}

void app_dmx_receiveMsg(eal_task_Task *self, msg_Message *message){
	if(message->type == app_cfg_DMX_MSG){
		/*Action parsing*/
		if(msg_isAction(message)){
			uint16_t idx = msg_KILL_ACTION_MSK(message->index);
			//TODO: Add real behavior
			dmx1.universe[idx] = (uint8_t)message->value;
		}
		/*Event parsing*/
		}else{

		}
}

/* Callback's ---------------------------------------------------------------------*/
bool dmx1TransmitterCallback(dmx_cfg_Instance *dmx){
	bool state = true;
	return state;
}

bool dmx2ReceiverCallback(dmx_cfg_Instance *dmx){
	bool state = true;
	return state;
}
/* IRQs ---------------------------------------------------------------------*/
void USART1_IRQHandler(void){
	dmx_irqRxInterfaceHandler(&dmx2);
}

void USART2_IRQHandler(void){
	dmx_irqTxInterfaceHandler(&dmx1);
}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	if(htim->Instance == dmx1.txTimer) dmx_irqTxTimerHandler(&dmx1);
}

