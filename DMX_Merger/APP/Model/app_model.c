/*
 * app_model.c
 *
 *  Created on: 11.03.2020
 *      Author: erics
 */
/******************************************************************************
* Includes
*******************************************************************************/
#include "app_model.h"
/******************************************************************************
* Constants
*******************************************************************************/
//TODO: Change no preset to 0
static app_dmx_preset_cfg_Preset myPresetsModel[app_cfg_NMBR_PRESETS] = {
		{.name = "Preset0"},
		{.name = "Preset1"},
		{.name = "Preset2"},
		{.name = "Preset3"},
		{.name = "Preset4"},
		{.name = "Preset5"},
		{.name = "Preset6"},
		{.name = "Preset7"},
		{.name = "Preset8"},
		{.name = "Preset9"},
};
static app_footcontrol_cfg_GpioCfg myGpiosModel[app_cfg_NMBR_BTNS] = {
	{.press = {.presetNr = 255},.release = {.presetNr = 255},.holdShort = {.presetNr = 255},.holdLong = {.presetNr = 255},},
	{.press = {.presetNr = 255},.release = {.presetNr = 255},.holdShort = {.presetNr = 255},.holdLong = {.presetNr = 255},},
	{.press = {.presetNr = 255},.release = {.presetNr = 255},.holdShort = {.presetNr = 255},.holdLong = {.presetNr = 255},},
	{.press = {.presetNr = 255},.release = {.presetNr = 255},.holdShort = {.presetNr = 255},.holdLong = {.presetNr = 255},},
	{.press = {.presetNr = 255},.release = {.presetNr = 255},.holdShort = {.presetNr = 255},.holdLong = {.presetNr = 255},},
};

app_model_Model app_model_model = {
	.presetSize = app_cfg_NMBR_PRESETS,
	.presets = myPresetsModel,
	.footswitchSize = app_cfg_NMBR_BTNS,
	.footswitches = myGpiosModel,
	.dirty = false
};
