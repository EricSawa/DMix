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
/******************************************************************************
* Instances definitions
*******************************************************************************/
app_dmx_preset_cfg_Preset myPresets[app_cfg_NMBR_PRESETS] = {
	{.presetTransitionSettings =
		{
		app_dmx_preset_cfg_TRANSITION_SETTINGS_LITERAL(0, 0, 255, 4000, 0, 0, true),
		app_dmx_preset_cfg_TRANSITION_SETTINGS_LITERAL(0, 0, 0, 0, 5000, 0, true),

		app_dmx_preset_cfg_TRANSITION_SETTINGS_LITERAL(2, 0, 255, 0, 0, 0, true),
		app_dmx_preset_cfg_TRANSITION_SETTINGS_LITERAL(1, 0, 0, 0, 5000, 0, true),

		app_dmx_preset_cfg_TRANSITION_SETTINGS_LITERAL(4, 0, 255, 4000, 1000, 0, true),
		app_dmx_preset_cfg_TRANSITION_SETTINGS_LITERAL(4, 0, 0, 0, 5000, 0, true),

		app_dmx_preset_cfg_TRANSITION_SETTINGS_LITERAL(1, 0, 255, 4000, 0, 0, false),
		app_dmx_preset_cfg_TRANSITION_SETTINGS_LITERAL(1, 0, 0, 0, 0, 0, false),

		app_dmx_preset_cfg_TRANSITION_SETTINGS_LITERAL(0, 0, 0, 0, 0, 0, false),
		app_dmx_preset_cfg_TRANSITION_SETTINGS_LITERAL(0, 0, 0, 0, 0, 0, false),
		}
	},
	{.presetTransitionSettings =
		{
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


