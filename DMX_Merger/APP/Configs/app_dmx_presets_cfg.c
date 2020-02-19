/*
 * app_dmx_presets_cfg.c
 *
 *  Created on: 31.12.2019
 *      Author: erics
 */
/******************************************************************************
* Includes
*******************************************************************************/
#include "configs.h"
#define FOG_GUN_OFFSET	99
/******************************************************************************
* Instances definitions
*******************************************************************************/
app_dmx_preset_cfg_Preset myPresets[app_cfg_NMBR_PRESETS] = {
	{.presetTransitionSettings =
		{
		app_dmx_preset_cfg_TRANSITION_SETTINGS_LITERAL(FOG_GUN_OFFSET + 3, 0, 110, 0, 0, 0, true), /*Color*/
		app_dmx_preset_cfg_TRANSITION_SETTINGS_LITERAL(FOG_GUN_OFFSET + 1, 0, 255, 0, 0, 0, true), /*Dimmer*/
		app_dmx_preset_cfg_TRANSITION_SETTINGS_LITERAL(0, 0, 0, 0, 0, 0, false),
		app_dmx_preset_cfg_TRANSITION_SETTINGS_LITERAL(0, 0, 0, 0, 0, 0, false),
		app_dmx_preset_cfg_TRANSITION_SETTINGS_LITERAL(0, 0, 0, 0, 0, 0, false),
		app_dmx_preset_cfg_TRANSITION_SETTINGS_LITERAL(0, 0, 0, 0, 0, 0, false),
		app_dmx_preset_cfg_TRANSITION_SETTINGS_LITERAL(0, 0, 0, 0, 0, 0, false),
		app_dmx_preset_cfg_TRANSITION_SETTINGS_LITERAL(0, 0, 0, 0, 0, 0, false),
		app_dmx_preset_cfg_TRANSITION_SETTINGS_LITERAL(0, 0, 0, 0, 0, 0, false),
		app_dmx_preset_cfg_TRANSITION_SETTINGS_LITERAL(0, 0, 0, 0, 0, 0, false),
		app_dmx_preset_cfg_TRANSITION_SETTINGS_LITERAL(0, 0, 0, 0, 0, 0, false),
		app_dmx_preset_cfg_TRANSITION_SETTINGS_LITERAL(0, 0, 0, 0, 0, 0, false),
		app_dmx_preset_cfg_TRANSITION_SETTINGS_LITERAL(0, 0, 0, 0, 0, 0, false),
		app_dmx_preset_cfg_TRANSITION_SETTINGS_LITERAL(0, 0, 0, 0, 0, 0, false),
		app_dmx_preset_cfg_TRANSITION_SETTINGS_LITERAL(0, 0, 0, 0, 0, 0, false),
		app_dmx_preset_cfg_TRANSITION_SETTINGS_LITERAL(0, 0, 0, 0, 0, 0, false),
		app_dmx_preset_cfg_TRANSITION_SETTINGS_LITERAL(0, 0, 0, 0, 0, 0, false),
		app_dmx_preset_cfg_TRANSITION_SETTINGS_LITERAL(0, 0, 0, 0, 0, 0, false),
		app_dmx_preset_cfg_TRANSITION_SETTINGS_LITERAL(0, 0, 0, 0, 0, 0, false),
		app_dmx_preset_cfg_TRANSITION_SETTINGS_LITERAL(0, 0, 0, 0, 0, 0, false),
		}
	},
	{.presetTransitionSettings =
		{
		app_dmx_preset_cfg_TRANSITION_SETTINGS_LITERAL(FOG_GUN_OFFSET + 3, 0, 0, 0, 0, 0, true),	/*Color*/
		app_dmx_preset_cfg_TRANSITION_SETTINGS_LITERAL(FOG_GUN_OFFSET + 1, 0, 0, 0, 0, 0, true),	/*Dimmer*/
		app_dmx_preset_cfg_TRANSITION_SETTINGS_LITERAL(FOG_GUN_OFFSET + 2, 0, 0, 0, 0, 0, true),	/*Strobe*/
		app_dmx_preset_cfg_TRANSITION_SETTINGS_LITERAL(0, 0, 0, 0, 0, 0, false),
		app_dmx_preset_cfg_TRANSITION_SETTINGS_LITERAL(0, 0, 0, 0, 0, 0, false),
		app_dmx_preset_cfg_TRANSITION_SETTINGS_LITERAL(0, 0, 0, 0, 0, 0, false),
		app_dmx_preset_cfg_TRANSITION_SETTINGS_LITERAL(0, 0, 0, 0, 0, 0, false),
		app_dmx_preset_cfg_TRANSITION_SETTINGS_LITERAL(0, 0, 0, 0, 0, 0, false),
		app_dmx_preset_cfg_TRANSITION_SETTINGS_LITERAL(0, 0, 0, 0, 0, 0, false),
		app_dmx_preset_cfg_TRANSITION_SETTINGS_LITERAL(0, 0, 0, 0, 0, 0, false),
		app_dmx_preset_cfg_TRANSITION_SETTINGS_LITERAL(0, 0, 0, 0, 0, 0, false),
		app_dmx_preset_cfg_TRANSITION_SETTINGS_LITERAL(0, 0, 0, 0, 0, 0, false),
		app_dmx_preset_cfg_TRANSITION_SETTINGS_LITERAL(0, 0, 0, 0, 0, 0, false),
		app_dmx_preset_cfg_TRANSITION_SETTINGS_LITERAL(0, 0, 0, 0, 0, 0, false),
		app_dmx_preset_cfg_TRANSITION_SETTINGS_LITERAL(0, 0, 0, 0, 0, 0, false),
		app_dmx_preset_cfg_TRANSITION_SETTINGS_LITERAL(0, 0, 0, 0, 0, 0, false),
		app_dmx_preset_cfg_TRANSITION_SETTINGS_LITERAL(0, 0, 0, 0, 0, 0, false),
		app_dmx_preset_cfg_TRANSITION_SETTINGS_LITERAL(0, 0, 0, 0, 0, 0, false),
		app_dmx_preset_cfg_TRANSITION_SETTINGS_LITERAL(0, 0, 0, 0, 0, 0, false),
		app_dmx_preset_cfg_TRANSITION_SETTINGS_LITERAL(0, 0, 0, 0, 0, 0, false),
		}
	},
	{.presetTransitionSettings =
		{
		app_dmx_preset_cfg_TRANSITION_SETTINGS_LITERAL(FOG_GUN_OFFSET + 3, 0, 14, 0, 0, 0, true),
		app_dmx_preset_cfg_TRANSITION_SETTINGS_LITERAL(FOG_GUN_OFFSET + 3, 0, 0, 0, 2000, 0, true),
		app_dmx_preset_cfg_TRANSITION_SETTINGS_LITERAL(FOG_GUN_OFFSET + 1, 0, 255, 0, 0, 0, true),
		app_dmx_preset_cfg_TRANSITION_SETTINGS_LITERAL(FOG_GUN_OFFSET + 1, 0, 0, 0, 2000, 0, true),
		app_dmx_preset_cfg_TRANSITION_SETTINGS_LITERAL(FOG_GUN_OFFSET + 0, 0, 255, 0, 0, 0, true), 	/*FOG*/
		app_dmx_preset_cfg_TRANSITION_SETTINGS_LITERAL(FOG_GUN_OFFSET + 0, 0, 0, 0, 2000, 0, true),	/*FOG*/
		app_dmx_preset_cfg_TRANSITION_SETTINGS_LITERAL(0, 0, 0, 0, 0, 0, false),
		app_dmx_preset_cfg_TRANSITION_SETTINGS_LITERAL(0, 0, 0, 0, 0, 0, false),
		app_dmx_preset_cfg_TRANSITION_SETTINGS_LITERAL(0, 0, 0, 0, 0, 0, false),
		app_dmx_preset_cfg_TRANSITION_SETTINGS_LITERAL(0, 0, 0, 0, 0, 0, false),
		app_dmx_preset_cfg_TRANSITION_SETTINGS_LITERAL(0, 0, 0, 0, 0, 0, false),
		app_dmx_preset_cfg_TRANSITION_SETTINGS_LITERAL(0, 0, 0, 0, 0, 0, false),
		app_dmx_preset_cfg_TRANSITION_SETTINGS_LITERAL(0, 0, 0, 0, 0, 0, false),
		app_dmx_preset_cfg_TRANSITION_SETTINGS_LITERAL(0, 0, 0, 0, 0, 0, false),
		app_dmx_preset_cfg_TRANSITION_SETTINGS_LITERAL(0, 0, 0, 0, 0, 0, false),
		app_dmx_preset_cfg_TRANSITION_SETTINGS_LITERAL(0, 0, 0, 0, 0, 0, false),
		app_dmx_preset_cfg_TRANSITION_SETTINGS_LITERAL(0, 0, 0, 0, 0, 0, false),
		app_dmx_preset_cfg_TRANSITION_SETTINGS_LITERAL(0, 0, 0, 0, 0, 0, false),
		app_dmx_preset_cfg_TRANSITION_SETTINGS_LITERAL(0, 0, 0, 0, 0, 0, false),
		app_dmx_preset_cfg_TRANSITION_SETTINGS_LITERAL(0, 0, 0, 0, 0, 0, false),

		}
	},
	{.presetTransitionSettings =
		{
		app_dmx_preset_cfg_TRANSITION_SETTINGS_LITERAL(FOG_GUN_OFFSET + 2, 0, 255, 0, 0, 0, true), /*Strobe*/
		app_dmx_preset_cfg_TRANSITION_SETTINGS_LITERAL(0, 0, 0, 0, 0, 0, false),
		app_dmx_preset_cfg_TRANSITION_SETTINGS_LITERAL(0, 0, 0, 0, 0, 0, false),
		app_dmx_preset_cfg_TRANSITION_SETTINGS_LITERAL(0, 0, 0, 0, 0, 0, false),
		app_dmx_preset_cfg_TRANSITION_SETTINGS_LITERAL(0, 0, 0, 0, 0, 0, false),
		app_dmx_preset_cfg_TRANSITION_SETTINGS_LITERAL(0, 0, 0, 0, 0, 0, false),
		app_dmx_preset_cfg_TRANSITION_SETTINGS_LITERAL(0, 0, 0, 0, 0, 0, false),
		app_dmx_preset_cfg_TRANSITION_SETTINGS_LITERAL(0, 0, 0, 0, 0, 0, false),
		app_dmx_preset_cfg_TRANSITION_SETTINGS_LITERAL(0, 0, 0, 0, 0, 0, false),
		app_dmx_preset_cfg_TRANSITION_SETTINGS_LITERAL(0, 0, 0, 0, 0, 0, false),
		app_dmx_preset_cfg_TRANSITION_SETTINGS_LITERAL(0, 0, 0, 0, 0, 0, false),
		app_dmx_preset_cfg_TRANSITION_SETTINGS_LITERAL(0, 0, 0, 0, 0, 0, false),
		app_dmx_preset_cfg_TRANSITION_SETTINGS_LITERAL(0, 0, 0, 0, 0, 0, false),
		app_dmx_preset_cfg_TRANSITION_SETTINGS_LITERAL(0, 0, 0, 0, 0, 0, false),
		app_dmx_preset_cfg_TRANSITION_SETTINGS_LITERAL(0, 0, 0, 0, 0, 0, false),
		app_dmx_preset_cfg_TRANSITION_SETTINGS_LITERAL(0, 0, 0, 0, 0, 0, false),
		app_dmx_preset_cfg_TRANSITION_SETTINGS_LITERAL(0, 0, 0, 0, 0, 0, false),
		app_dmx_preset_cfg_TRANSITION_SETTINGS_LITERAL(0, 0, 0, 0, 0, 0, false),
		app_dmx_preset_cfg_TRANSITION_SETTINGS_LITERAL(0, 0, 0, 0, 0, 0, false),
		app_dmx_preset_cfg_TRANSITION_SETTINGS_LITERAL(0, 0, 0, 0, 0, 0, false),
		}
	},
};


