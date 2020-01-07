/*
 * configs.h
 *
 *  Created on: 22.11.2019
 *      Author: erics
 */

#ifndef CONFIGS_CONFIGS_H_
#define CONFIGS_CONFIGS_H_

#include "app_cfg.h"
#include "eeprom_cfg.h"
#include "dmx_cfg.h"
#include "soft_i2c_cfg.h"
#include "app_dmx_preset_cfg.h"


/*General Instances*/
extern eeprom_cfg_Config eepromDev1;
extern dmx_cfg_Instance dmx1;
extern dmx_cfg_Instance dmx2;
extern soft_i2c_cfg_Config softI2C2;

/*Application Instances*/
extern app_dmx_preset_cfg_Preset myPresets[app_cfg_NMBR_PRESETS];

#endif /* CONFIGS_CONFIGS_H_ */
