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
		myPresets[i].relTime_ms = 0;
		myPresets[i].trigger = false;
		myPresets[i].transitionDoneCnt = 0;
		myPresets[i].transitionEnableSize = 0;
		memset(myPresets[i].presetTransitionStatus, 0, sizeof(myPresets[i].presetTransitionStatus));
		for(int j = 0; j < app_dmx_preset_cfg_NMBR_TRANSITIONS; ++j){
			if(myPresets[i].presetTransitionSettings[j].enable) ++myPresets[i].transitionEnableSize;
		}
	}
}

void app_dmx_preset_process(eal_task_Task *self){
	if(!self->enable) return;
	if(!cyclicFlag) return;
	/*Do everything what you want to do*/
	for(int i = 0; i < app_cfg_NMBR_PRESETS; ++i){
		if(myPresets[i].trigger){	/*Preset is triggered and can be executed*/
			++myPresets[i].relTime_ms;
			for(int j = 0; j < app_dmx_preset_cfg_NMBR_TRANSITIONS; ++j){
				if(myPresets[i].presetTransitionSettings[j].enable){
					if(myPresets[i].presetTransitionStatus[j].transStep == app_dmx_preset_cfg_TRANS_STEP_EXE) performNextTransitionStep(self, i, j);
					else if((myPresets[i].relTime_ms >= myPresets[i].presetTransitionSettings[j].transDelay_ms) && (myPresets[i].presetTransitionStatus[j].transStep == app_dmx_preset_cfg_TRANS_STEP_NONE)){
						myPresets[i].presetTransitionStatus[j].transStep = app_dmx_preset_cfg_TRANS_STEP_EXE;
						performNextTransitionStep(self, i, j);
					}
				}
			}
			if(myPresets[i].transitionDoneCnt >= myPresets[i].transitionEnableSize){	/*All transitions are performed*/
				myPresets[i].transitionDoneCnt = 0;
				myPresets[i].trigger = false;
				myPresets[i].relTime_ms = 0;
				memset(myPresets[i].presetTransitionStatus, 0, sizeof(myPresets[i].presetTransitionStatus));
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
			if(APP_CFG_IS_INDEX_IN_RANGE(idx, app_cfg_dmx_preset_TRIGGER_PRESET_OFFSET_IDX, app_cfg_dmx_preset_TRIGGER_PRESET4_IDX+1)){
				if((message->value == app_cfg_dmx_preset_TRIGGER_ENABLE) && (myPresets[idx].trigger == false)) {	/*Can be triggered only once*/
					myPresets[idx].trigger = true;
					myPresets[idx].relTime_ms = 0;
					myPresets[idx].transitionDoneCnt = 0;
					memset(myPresets[idx].presetTransitionStatus, 0, sizeof(myPresets[idx].presetTransitionStatus));
					if(self->sendMsg != NULL){
						msg_Message newMessage = msg_LITERAL(app_cfg_DMX_PRESET_MSG, msg_EVENT(idx), app_cfg_dmx_preset_TRIGGER_ENABLE, 0, 0);
						self->sendMsg(self, &newMessage);
					}
				}else{
					myPresets[idx].trigger = false;
					myPresets[idx].relTime_ms = 0;
					myPresets[idx].transitionDoneCnt = 0;
					memset(myPresets[idx].presetTransitionStatus, 0, sizeof(myPresets[idx].presetTransitionStatus));
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
	float actualTransitionTime = (float)(myPresets[preset].relTime_ms - myPresets[preset].presetTransitionSettings[transition].transDelay_ms);
	uint16_t newVal = 0;
	if(myPresets[preset].presetTransitionSettings[transition].transTime_ms == 0){
		newVal = myPresets[preset].presetTransitionSettings[transition].dmxEndVal;
		++myPresets[preset].transitionDoneCnt;
		myPresets[preset].presetTransitionStatus[transition].transStep = app_dmx_preset_cfg_TRANS_STEP_DONE;
	}else{
		//TODO: Perform different ramps
		float diff = (float)((float)myPresets[preset].presetTransitionSettings[transition].dmxEndVal - (float)myPresets[preset].presetTransitionSettings[transition].dmxStartVal);
		float actualVal = (diff/((float)myPresets[preset].presetTransitionSettings[transition].transTime_ms)*actualTransitionTime) + (float)myPresets[preset].presetTransitionSettings[transition].dmxStartVal;
		newVal = (uint16_t)actualVal;
		if(newVal >= myPresets[preset].presetTransitionSettings[transition].dmxEndVal){
			newVal = myPresets[preset].presetTransitionSettings[transition].dmxEndVal;
			++myPresets[preset].transitionDoneCnt;
			myPresets[preset].presetTransitionStatus[transition].transStep = app_dmx_preset_cfg_TRANS_STEP_DONE;
		}
	}
	//If the if request is enabled 0 values can't be send because the comparison will be never true
//	if(newVal != myPresets[preset].presetTransitionStatus[transition].actualVal) {
		myPresets[preset].presetTransitionStatus[transition].actualVal = newVal;
		if(self->sendMsg != NULL){
			msg_Message newMessage = msg_LITERAL(app_cfg_DMX_MSG, msg_ACTION(myPresets[preset].presetTransitionSettings[transition].dmxCh), myPresets[preset].presetTransitionStatus[transition].actualVal, 0, 0);
			self->sendMsg(self, &newMessage);
		}
		if(myPresets[preset].presetTransitionStatus[transition].transStep == app_dmx_preset_cfg_TRANS_STEP_DONE) myPresets[preset].presetTransitionStatus[transition].actualVal = 0;
//	}
}
