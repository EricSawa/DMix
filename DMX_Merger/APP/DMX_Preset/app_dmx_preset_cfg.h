/*
 * app_dmx_preset_cfg.h
 *
 *  Created on: 31.12.2019
 *      Author: erics
 */

#ifndef DMX_PRESET_APP_DMX_PRESET_CFG_H_
#define DMX_PRESET_APP_DMX_PRESET_CFG_H_
/******************************************************************************
* Includes
*******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
/******************************************************************************
* Constants
*******************************************************************************/
#define app_dmx_preset_cfg_NMBR_TRANSITIONS 10
/******************************************************************************
* Typedefs
*******************************************************************************/
typedef enum{
	app_dmx_preset_cfg_RAMP_LINEAR
}app_dmx_preset_cfg_Ramp;

typedef enum{
	app_dmx_preset_cfg_TRANS_STEP_NONE,
	app_dmx_preset_cfg_TRANS_STEP_EXE,
	app_dmx_preset_cfg_TRANS_STEP_DONE,
}app_dmx_preset_cfg_TransitionStep;

typedef struct{
	uint16_t dmxCh;
	uint8_t dmxStartVal;
	uint8_t dmxEndVal;
	uint32_t transTime_ms;
	uint32_t transDelay_ms;
	app_dmx_preset_cfg_Ramp transRamp;
	bool enable;
}app_dmx_preset_cfg_TransitionSettings;

#define app_dmx_preset_cfg_TRANSITION_SETTINGS_LITERAL(CH, START_VAL, END_VAL, TRANS_TIME, TRANS_DELAY, TRANS_RAMP, ENABLE) {\
	.dmxCh = (CH), .dmxStartVal = (START_VAL), .dmxEndVal = (END_VAL), \
	.transTime_ms = (TRANS_TIME), .transDelay_ms = (TRANS_DELAY), .transRamp = (TRANS_RAMP), .enable = (ENABLE)}

typedef struct{
	uint8_t actualVal;
	app_dmx_preset_cfg_TransitionStep transStep;
}app_dmx_preset_cfg_TransitionStatus;

typedef struct{
	bool trigger;
	uint32_t relTime_ms;		/*Starts to count when preset is triggered. Can be used to check transition delays*/
	uint32_t transitionDoneCnt;	/*Will be used to check how many transitions are already done*/
	uint32_t transitionEnableSize;
	app_dmx_preset_cfg_TransitionSettings presetTransitionSettings[app_dmx_preset_cfg_NMBR_TRANSITIONS];
	app_dmx_preset_cfg_TransitionStatus presetTransitionStatus[app_dmx_preset_cfg_NMBR_TRANSITIONS];
}app_dmx_preset_cfg_Preset;



#endif /* DMX_PRESET_APP_DMX_PRESET_CFG_H_ */
