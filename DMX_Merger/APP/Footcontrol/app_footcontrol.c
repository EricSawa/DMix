/*
 * app_footcontrol.c
 *
 *  Created on: 07.01.2020
 *      Author: erics
 */
/******************************************************************************
* Includes
*******************************************************************************/
#include "app_footcontrol.h"
/******************************************************************************
* Configuration
*******************************************************************************/



/*User access*/
app_footcontrol_cfg_Status status = app_footcontrol_ACTIVE;
/******************************************************************************
* Variable Declarations
*******************************************************************************/
static volatile bool cyclicFlag = false;
static volatile int16_t internalTimer = 0;
/******************************************************************************
* Callback's
*******************************************************************************/

/******************************************************************************
* Function Prototypes
*******************************************************************************/
static void sendDmxPreset(eal_task_Task *self, app_footcontrol_cfg_PresetCfg *preset);
/******************************************************************************
* Function Definitions
*******************************************************************************/
void app_footcontrol_init(eal_task_Task *self){
	status = app_footcontrol_ACTIVE;
}

void app_footcontrol_process(eal_task_Task *self){
	if(!self->enable) return;
	if(!cyclicFlag) return;
	//TODO: logic
	cyclicFlag = false;
}

void app_footcontrol_cyclic1ms(eal_task_Task *self, bool stat){
	cyclicFlag = true;
	if(internalTimer > 0)--internalTimer;
}

void app_footcontrol_receiveMsg(eal_task_Task *self, msg_Message *message){
	if(status != app_footcontrol_ACTIVE) return;
	/*Button message parsing*/
	if(message->type == app_cfg_BTN_MSG){
		/*Action parsing*/
		if(msg_isAction(message)){
//			uint16_t idx = msg_KILL_ACTION_MSK(message->index);
		}
		/*Event parsing*/
		else{
			uint16_t idx = message->index;
			if(app_model_model.footswitches[idx].mode == app_footcontrol_GPIO_INPUT){
				if((message->value == app_cfg_btn_STAT_RELEASED) && (app_model_model.footswitches[idx].release.presetNr < app_cfg_NMBR_PRESETS)) sendDmxPreset(self, &app_model_model.footswitches[idx].release);
				else if((message->value == app_cfg_btn_STAT_PRESSED) && (app_model_model.footswitches[idx].press.presetNr < app_cfg_NMBR_PRESETS)) sendDmxPreset(self, &app_model_model.footswitches[idx].press);
				else if((message->value == app_cfg_btn_STAT_HOLD_SHORT) && (app_model_model.footswitches[idx].holdShort.presetNr < app_cfg_NMBR_PRESETS)) sendDmxPreset(self, &app_model_model.footswitches[idx].holdShort);
				else if((message->value == app_cfg_btn_STAT_HOLD_LONG) && (app_model_model.footswitches[idx].holdLong.presetNr < app_cfg_NMBR_PRESETS)) sendDmxPreset(self, &app_model_model.footswitches[idx].holdLong);
			}
		}
	}
}

/* Helper ----------------------------------------------------------------------*/
static void sendDmxPreset(eal_task_Task *self, app_footcontrol_cfg_PresetCfg *preset){
	if(self->sendMsg == NULL) return;
	msg_Message newMessage = msg_LITERAL(app_cfg_DMX_PRESET_MSG, msg_ACTION(preset->presetNr), app_cfg_dmx_preset_TRIGGER_ENABLE, 0, 0);
	self->sendMsg(self, &newMessage);
}
