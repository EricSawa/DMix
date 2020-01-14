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

static void updateDmxOut();
/******************************************************************************
* Variable Declarations
*******************************************************************************/
static volatile bool cyclicFlag = false;
static volatile int16_t internalTimer = 0;

typedef enum{
	app_dmx_MERGE_ACTIVE,
	app_dmx_MERGE_INACTIVE,
}app_dmx_Merge;

typedef enum{
	app_dmx_MERGE_MODE_HTP,
	app_dmx_MERGE_MODE_LOTP,
}app_dmx_MergeMode;

typedef enum{
	app_dmx_INTERFACE_MODE_FOOTCONTROL,
	app_dmx_INTERFACE_MODE_INTERFACE,
}app_dmx_InterfaceMode;

uint8_t dmxModified[512] = {0};
bool dmxTxComplete = false;

/*User access*/
app_dmx_Merge dmxMerge = app_dmx_MERGE_ACTIVE;
app_dmx_MergeMode dmxMergeMode = app_dmx_MERGE_MODE_HTP;
/******************************************************************************
* Function Definitions
*******************************************************************************/
void app_dmx_init(eal_task_Task *self){
	dmxMerge = app_dmx_MERGE_ACTIVE;
	dmxMergeMode = app_dmx_MERGE_MODE_HTP;

	dmx_registerCallback(&dmx1Out, dmx1TransmitterCallback);
	dmx_init(&dmx1Out);
	dmx_txStart(&dmx1Out);

	dmx_registerCallback(&dmx2In, dmx2ReceiverCallback);
	if(dmxMerge == app_dmx_MERGE_ACTIVE){
		dmx_init(&dmx2In);
		dmx_rxStart(&dmx2In);
	}

	HAL_GPIO_WritePin(O_USART1_DIR_GPIO_Port, O_USART1_DIR_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(O_USART2_DIR_GPIO_Port, O_USART2_DIR_Pin, GPIO_PIN_SET);

	memset(dmxModified, 0, sizeof(dmxModified));
	dmxTxComplete = false;
}

void app_dmx_process(eal_task_Task *self){
	if(!self->enable) return;
	if(!cyclicFlag) return;
	/*Do everything what you want to do*/
//	dmx1Out.universe[1] = 255;
//	dmx1Out.universe[3] = 10;
	dmx_loop(&dmx1Out);
	/*Receive makes only sense if merge is enabled*/
	if(dmxMerge == app_dmx_MERGE_ACTIVE) dmx_loop(&dmx2In);

	if(dmxTxComplete){updateDmxOut(); dmxTxComplete = false;}

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
			dmxModified[idx] = (uint8_t)message->value;
		}
		/*Event parsing*/
		}else{

		}
}

/* Helper ---------------------------------------------------------------------*/
static void updateDmxOut(){
	if(dmxMerge == app_dmx_MERGE_ACTIVE){
		for(int i = 0; i < dmx1Out.universeSize; ++i){
			uint8_t dmxRxValue = dmx2In.universe[i]; /*Atomic operation*/
			uint8_t dmxModValue = dmxModified[i]; /*Atomic operation*/
			if(dmxMergeMode == app_dmx_MERGE_MODE_HTP) {	/*Highest takes presence*/
				if(dmxRxValue > dmxModValue) dmx1Out.universe[i] = dmxRxValue;
				else dmx1Out.universe[i] = dmxModValue;
			}else{								/*Lowest takes presence*/
				if(dmxRxValue < dmxModValue) dmx1Out.universe[i] = dmxRxValue;
				else dmx1Out.universe[i] = dmxModValue;
			}
		}
	}else{ /*If merge is disabled just copy the dmxModified data to output*/
		memcpy(dmx1Out.universe, dmxModified, dmx1Out.universeSize);
	}
}

/* Callback's ---------------------------------------------------------------------*/
bool dmx1TransmitterCallback(dmx_cfg_Instance *dmx){
	bool state = true;
	dmxTxComplete = true;
	return state;
}

bool dmx2ReceiverCallback(dmx_cfg_Instance *dmx){
	bool state = true;
	return state;
}
/* IRQs ---------------------------------------------------------------------*/
void USART1_IRQHandler(void){
	dmx_irqRxInterfaceHandler(&dmx2In);
}

void USART2_IRQHandler(void){
	dmx_irqTxInterfaceHandler(&dmx1Out);
}

