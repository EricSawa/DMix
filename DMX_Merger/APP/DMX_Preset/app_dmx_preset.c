/*
 * app_dmx_preset.c
 *
 *  Created on: 31.12.2019
 *      Author: erics
 */
/******************************************************************************
* Includes
*******************************************************************************/
#include "app_dmx_preset.h"
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
static void performNextTransitionStep(eal_task_Task *self, uint8_t preset, uint8_t transition);
/******************************************************************************
* Variable Declarations
*******************************************************************************/
static volatile bool cyclicFlag = false;
static volatile int16_t internalTimer = 0;
/******************************************************************************
* Function Definitions
*******************************************************************************/
void app_dmx_preset_init(eal_task_Task *self){
	for(int i = 0; i < app_cfg_NMBR_PRESETS; ++i){
		app_model_model.presets[i].relTime_ms = 0;
		app_model_model.presets[i].trigger = false;
		app_model_model.presets[i].transitionDoneCnt = 0;
	}
}

void app_dmx_preset_process(eal_task_Task *self){
	if(!self->enable) return;
	if(!cyclicFlag) return;
	/*Do everything what you want to do*/
	for(int i = 0; i < app_cfg_NMBR_PRESETS; ++i){
		if(app_model_model.presets[i].trigger){	/*Preset is triggered and can be executed*/
			++app_model_model.presets[i].relTime_ms;
			for(int j = 0; j < app_dmx_preset_cfg_NMBR_TRANSITIONS; ++j){
				if(app_model_model.presets[i].presetTransitionSettings[j].enable){
					if(app_model_model.presets[i].presetTransitionStatus[j].transStep == app_dmx_preset_cfg_TRANS_STEP_EXE) performNextTransitionStep(self, i, j);
					else if((app_model_model.presets[i].relTime_ms >= app_model_model.presets[i].presetTransitionSettings[j].transDelay_ms) && (app_model_model.presets[i].presetTransitionStatus[j].transStep == app_dmx_preset_cfg_TRANS_STEP_NONE)){
						app_model_model.presets[i].presetTransitionStatus[j].transStep = app_dmx_preset_cfg_TRANS_STEP_EXE;
						performNextTransitionStep(self, i, j);
					}
				}
			}
			if(app_model_model.presets[i].transitionDoneCnt >= app_model_model.presets[i].transitionEnableSize){	/*All transitions are performed*/
				app_model_model.presets[i].transitionDoneCnt = 0;
				app_model_model.presets[i].trigger = false;
				app_model_model.presets[i].relTime_ms = 0;
				memset(app_model_model.presets[i].presetTransitionStatus, 0, sizeof(app_model_model.presets[i].presetTransitionStatus));
			}
		}
	}
	/*End of everything you want to do*/
	cyclicFlag = false;
}

void app_dmx_preset_cyclic1ms(eal_task_Task *self, bool stat){
	cyclicFlag = true;
}

void app_dmx_preset_receiveMsg(eal_task_Task *self, msg_Message *message){
	if(message->type == app_cfg_DMX_PRESET_MSG){
		/*Action parsing*/
		if(msg_isAction(message)){
			uint16_t idx = msg_KILL_ACTION_MSK(message->index);
			if(APP_CFG_IS_INDEX_IN_RANGE(idx, app_cfg_dmx_preset_TRIGGER_PRESET_OFFSET_IDX, app_cfg_dmx_preset_TRIGGER_PRESET_OFFSET_IDX+app_cfg_NMBR_PRESETS)){
				if((message->value == app_cfg_dmx_preset_TRIGGER_ENABLE) && (app_model_model.presets[idx].trigger == false)) {	/*Can be triggered only once*/
					app_model_model.presets[idx].trigger = true;
					app_model_model.presets[idx].relTime_ms = 0;
					app_model_model.presets[idx].transitionDoneCnt = 0;
					memset(app_model_model.presets[idx].presetTransitionStatus, 0, sizeof(app_model_model.presets[idx].presetTransitionStatus));
					if(self->sendMsg != NULL){
						msg_Message newMessage = msg_LITERAL(app_cfg_DMX_PRESET_MSG, msg_EVENT(idx), app_cfg_dmx_preset_TRIGGER_ENABLE, 0, 0);
						self->sendMsg(self, &newMessage);
					}
				}else{
					app_model_model.presets[idx].trigger = false;
					app_model_model.presets[idx].relTime_ms = 0;
					app_model_model.presets[idx].transitionDoneCnt = 0;
					memset(app_model_model.presets[idx].presetTransitionStatus, 0, sizeof(app_model_model.presets[idx].presetTransitionStatus));
					if(self->sendMsg != NULL){
						msg_Message newMessage = msg_LITERAL(app_cfg_DMX_PRESET_MSG, msg_EVENT(idx), app_cfg_dmx_preset_TRIGGER_DISABLE, 0, 0);
						self->sendMsg(self, &newMessage);
					}
				}
			}
		/*Event parsing*/
		}else{

		}
	}
}

/*Helper*/
static void performNextTransitionStep(eal_task_Task *self, uint8_t preset, uint8_t transition){
	float actualTransitionTime = (float)(app_model_model.presets[preset].relTime_ms - app_model_model.presets[preset].presetTransitionSettings[transition].transDelay_ms);
	uint16_t newVal = 0;
	if(app_model_model.presets[preset].presetTransitionSettings[transition].transTime_ms == 0){
		newVal = app_model_model.presets[preset].presetTransitionSettings[transition].dmxEndVal;
		++app_model_model.presets[preset].transitionDoneCnt;
		app_model_model.presets[preset].presetTransitionStatus[transition].transStep = app_dmx_preset_cfg_TRANS_STEP_DONE;
	}else{
		//TODO: Perform different ramps
		float diff = (float)((float)app_model_model.presets[preset].presetTransitionSettings[transition].dmxEndVal - (float)app_model_model.presets[preset].presetTransitionSettings[transition].dmxStartVal);
		float actualVal = (diff/((float)app_model_model.presets[preset].presetTransitionSettings[transition].transTime_ms)*actualTransitionTime) + (float)app_model_model.presets[preset].presetTransitionSettings[transition].dmxStartVal;
		newVal = (uint16_t)actualVal;
		if(newVal >= app_model_model.presets[preset].presetTransitionSettings[transition].dmxEndVal){
			newVal = app_model_model.presets[preset].presetTransitionSettings[transition].dmxEndVal;
			++app_model_model.presets[preset].transitionDoneCnt;
			app_model_model.presets[preset].presetTransitionStatus[transition].transStep = app_dmx_preset_cfg_TRANS_STEP_DONE;
		}
	}
	//If the if request is enabled 0 values can't be send because the comparison will be never true
//	if(newVal != app_model_model.presets[preset].presetTransitionStatus[transition].actualVal) {
		app_model_model.presets[preset].presetTransitionStatus[transition].actualVal = newVal;
		if(self->sendMsg != NULL){
			msg_Message newMessage = msg_LITERAL(app_cfg_DMX_MSG, msg_ACTION(app_model_model.presets[preset].presetTransitionSettings[transition].dmxCh), app_model_model.presets[preset].presetTransitionStatus[transition].actualVal, 0, 0);
			self->sendMsg(self, &newMessage);
		}
		if(app_model_model.presets[preset].presetTransitionStatus[transition].transStep == app_dmx_preset_cfg_TRANS_STEP_DONE) app_model_model.presets[preset].presetTransitionStatus[transition].actualVal = 0;
//	}
}
