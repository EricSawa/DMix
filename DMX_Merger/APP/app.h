/*
 * app.h
 *
 *  Created on: 22.11.2019
 *      Author: erics
 */

#ifndef APP_H_
#define APP_H_

/******************************************************************************
* Includes
*******************************************************************************/

#include <stdint.h>
#include "gpio.h"
#include "usart.h"
#include "configs.h"
#include "dmx.h"
#include "app_dmx.h"
#include "app_dmx_preset.h"
#include "app_btn.h"
#include "app_footcontrol.h"
#include "app_interface.h"
#include "app_eeprom.h"
#include "eal_task.h"
#include "app_cfg.h"
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
* Variable Declarations
*******************************************************************************/

/******************************************************************************
* Function Prototypes
*******************************************************************************/
void app_main();
void app_1ms();

#endif /* APP_H_ */
