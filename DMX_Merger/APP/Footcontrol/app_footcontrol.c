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
typedef enum{
	app_footcontrol_ACTIVE,
	app_footcontrol_INACTIVE
}app_footcontrol_Status;

typedef enum{
	app_footcontrol_GPIO_INPUT,
	app_footcontrol_GPIO_OUTPUT
}app_footcontrol_GpioMode;

typedef struct{
	uint8_t presetNr;
	int16_t triggerValue;
}app_footcontrol_PresetCfg;

typedef struct{
	app_footcontrol_GpioMode mode;
	app_footcontrol_PresetCfg press;
	app_footcontrol_PresetCfg release;
	app_footcontrol_PresetCfg holdShort;
	app_footcontrol_PresetCfg holdLong;
}app_footcontrol_GpioCfg;


/*User access*/
app_footcontrol_Status status = app_footcontrol_ACTIVE;

app_footcontrol_GpioCfg myGpios[app_cfg_NMBR_BTNS] = {
	{.mode = app_footcontrol_GPIO_INPUT,
	.press = {.presetNr = 0, .triggerValue = app_cfg_dmx_preset_TRIGGER_ENABLE},
//	.release = {.presetNr = app_footcontrol_NO_PRESET, .triggerValue = app_cfg_dmx_preset_TRIGGER_ENABLE},
	.release = {.presetNr = 1, .triggerValue = app_cfg_dmx_preset_TRIGGER_ENABLE},
	.holdShort = {.presetNr = app_footcontrol_NO_PRESET, .triggerValue = app_cfg_dmx_preset_TRIGGER_ENABLE},
	.holdShort = {.presetNr = 3, .triggerValue = app_cfg_dmx_preset_TRIGGER_ENABLE},
	.holdLong = {.presetNr = app_footcontrol_NO_PRESET, .triggerValue = app_cfg_dmx_preset_TRIGGER_ENABLE},
	},
	{.mode = app_footcontrol_GPIO_INPUT,
//	.press = {.presetNr = app_footcontrol_NO_PRESET, .triggerValue = app_cfg_dmx_preset_TRIGGER_ENABLE},
	.press = {.presetNr = 0, .triggerValue = app_cfg_dmx_preset_TRIGGER_ENABLE},
//	.release = {.presetNr = app_footcontrol_NO_PRESET, .triggerValue = app_cfg_dmx_preset_TRIGGER_ENABLE},
	.release = {.presetNr = 1, .triggerValue = app_cfg_dmx_preset_TRIGGER_ENABLE},
	.holdShort = {.presetNr = 3, .triggerValue = app_cfg_dmx_preset_TRIGGER_ENABLE},
	.holdLong = {.presetNr = app_footcontrol_NO_PRESET, .triggerValue = app_cfg_dmx_preset_TRIGGER_ENABLE},
	},
	{.mode = app_footcontrol_GPIO_INPUT,
	.press = {.presetNr = app_footcontrol_NO_PRESET, .triggerValue = app_cfg_dmx_preset_TRIGGER_ENABLE},
	.release = {.presetNr = app_footcontrol_NO_PRESET, .triggerValue = app_cfg_dmx_preset_TRIGGER_ENABLE},
	.holdShort = {.presetNr = app_footcontrol_NO_PRESET, .triggerValue = app_cfg_dmx_preset_TRIGGER_ENABLE},
	.holdLong = {.presetNr = app_footcontrol_NO_PRESET, .triggerValue = app_cfg_dmx_preset_TRIGGER_ENABLE},
	},
	{.mode = app_footcontrol_GPIO_INPUT,
	.press = {.presetNr = app_footcontrol_NO_PRESET, .triggerValue = app_cfg_dmx_preset_TRIGGER_ENABLE},
	.press = {.presetNr = 2, .triggerValue = app_cfg_dmx_preset_TRIGGER_ENABLE},
//	.release = {.presetNr = app_footcontrol_NO_PRESET, .triggerValue = app_cfg_dmx_preset_TRIGGER_ENABLE},
	.holdShort = {.presetNr = app_footcontrol_NO_PRESET, .triggerValue = app_cfg_dmx_preset_TRIGGER_ENABLE},
	.holdLong = {.presetNr = app_footcontrol_NO_PRESET, .triggerValue = app_cfg_dmx_preset_TRIGGER_ENABLE},
	},
	{.mode = app_footcontrol_GPIO_INPUT,
	.press = {.presetNr = app_footcontrol_NO_PRESET, .triggerValue = app_cfg_dmx_preset_TRIGGER_ENABLE},
	.release = {.presetNr = app_footcontrol_NO_PRESET, .triggerValue = app_cfg_dmx_preset_TRIGGER_ENABLE},
	.holdShort = {.presetNr = app_footcontrol_NO_PRESET, .triggerValue = app_cfg_dmx_preset_TRIGGER_ENABLE},
	.holdLong = {.presetNr = app_footcontrol_NO_PRESET, .triggerValue = app_cfg_dmx_preset_TRIGGER_ENABLE},
	},
};
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
static void sendDmxPreset(eal_task_Task *self, app_footcontrol_PresetCfg *preset);
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
			if(myGpios[idx].mode == app_footcontrol_GPIO_INPUT){
				if((message->value == app_cfg_btn_STAT_RELEASED) && (myGpios[idx].release.presetNr != app_footcontrol_NO_PRESET)) sendDmxPreset(self, &myGpios[idx].release);
				else if((message->value == app_cfg_btn_STAT_PRESSED) && (myGpios[idx].press.presetNr != app_footcontrol_NO_PRESET)) sendDmxPreset(self, &myGpios[idx].press);
				else if((message->value == app_cfg_btn_STAT_HOLD_SHORT) && (myGpios[idx].holdShort.presetNr != app_footcontrol_NO_PRESET)) sendDmxPreset(self, &myGpios[idx].holdShort);
				else if((message->value == app_cfg_btn_STAT_HOLD_LONG) && (myGpios[idx].holdLong.presetNr != app_footcontrol_NO_PRESET)) sendDmxPreset(self, &myGpios[idx].holdLong);
			}
		}
	}
}

/* Helper ----------------------------------------------------------------------*/
static void sendDmxPreset(eal_task_Task *self, app_footcontrol_PresetCfg *preset){
	if(self->sendMsg == NULL) return;
	msg_Message newMessage = msg_LITERAL(app_cfg_DMX_PRESET_MSG, msg_ACTION(preset->presetNr), preset->triggerValue, 0, 0);
	self->sendMsg(self, &newMessage);
}
