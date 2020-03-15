/*
 * app_model.h
 *
 *  Created on: 11.03.2020
 *      Author: erics
 */

#ifndef MODEL_APP_MODEL_H_
#define MODEL_APP_MODEL_H_
/******************************************************************************
* Includes
*******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include "app_cfg.h"
#include "app_dmx_preset_cfg.h"
#include "app_footcontrol_cfg.h"
/******************************************************************************
* Constants
*******************************************************************************/
typedef struct{
	uint16_t presetSize;
	app_dmx_preset_cfg_Preset *presets;
	uint16_t footswitchSize;
	app_footcontrol_cfg_GpioCfg *footswitches;
	bool dirty;
}app_model_Model;

extern app_model_Model app_model_model;

#endif /* MODEL_APP_MODEL_H_ */
