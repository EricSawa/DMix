/*
 * app_footcontrol_cfg.h
 *
 *  Created on: 11.03.2020
 *      Author: erics
 */

#ifndef FOOTCONTROL_APP_FOOTCONTROL_CFG_H_
#define FOOTCONTROL_APP_FOOTCONTROL_CFG_H_
/******************************************************************************
* Includes
*******************************************************************************/
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
/******************************************************************************
* Constants
*******************************************************************************/

/******************************************************************************
* Typedefs
*******************************************************************************/
typedef enum{
	app_footcontrol_ACTIVE,
	app_footcontrol_INACTIVE
}app_footcontrol_cfg_Status;

typedef enum{
	app_footcontrol_GPIO_INPUT,
	app_footcontrol_GPIO_OUTPUT
}app_footcontrol_cfg_GpioMode;

typedef struct{
	uint8_t presetNr;
	int16_t triggerValue;
}app_footcontrol_cfg_PresetCfg;

typedef struct{
	app_footcontrol_cfg_GpioMode mode;
	app_footcontrol_cfg_PresetCfg press;
	app_footcontrol_cfg_PresetCfg release;
	app_footcontrol_cfg_PresetCfg holdShort;
	app_footcontrol_cfg_PresetCfg holdLong;
}app_footcontrol_cfg_GpioCfg;

#define app_dmx_footswitch_cfg_FOOTSWITCH_SIZE sizeof(app_footcontrol_cfg_GpioCfg)

#endif /* FOOTCONTROL_APP_FOOTCONTROL_CFG_H_ */
