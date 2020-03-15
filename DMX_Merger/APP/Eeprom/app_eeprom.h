/*
 * app_eeprom.h
 *
 *  Created on: 15.03.2020
 *      Author: erics
 */

#ifndef EEPROM_APP_EEPROM_H_
#define EEPROM_APP_EEPROM_H_
/******************************************************************************
* Includes
*******************************************************************************/
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "app_cfg.h"
#include "app_model.h"
#include "configs.h"
#include "i2c.h"
#include "eeprom.h"
#include "eal_task.h"
#include "main.h"
/******************************************************************************
* Constants
*******************************************************************************/
#define app_eeprom_PAGE_SIZE	256
#define app_eeprom_PAGE_NMBR	500

#define app_eeprom_PAGES_PER_VERSION	1
#define app_eeprom_TOTAL_VERSION_PAGES	app_eeprom_PAGES_PER_VERSION

#define app_eeprom_PAGES_PER_PRESET		2
#define app_eeprom_TOTAL_PRESET_PAGES	(app_eeprom_PAGES_PER_PRESET*app_cfg_NMBR_PRESETS)

#define app_eeprom_VERSION_PAGE				0
#define app_eeprom_PRESET_START_PAGE 		(app_eeprom_VERSION_PAGE+ app_eeprom_TOTAL_VERSION_PAGES)
#define app_eeprom_FOOTSWITCH_START_PAGE 	(app_eeprom_PRESET_START_PAGE + app_eeprom_TOTAL_PRESET_PAGES)
/******************************************************************************
* Typedefs
*******************************************************************************/
typedef struct{
	uint8_t data[app_eeprom_PAGE_SIZE];
	uint16_t addr;
}app_eeprom_Page;
/******************************************************************************
* Callback's
*******************************************************************************/

/******************************************************************************
* Function Prototypes
*******************************************************************************/
void app_eeprom_init(eal_task_Task *self);
void app_eeprom_process(eal_task_Task *self);
void app_eeprom_cyclic1ms(eal_task_Task *self, bool stat);
void app_eeprom_receiveMsg(eal_task_Task *self, msg_Message *message);


#endif /* EEPROM_APP_EEPROM_H_ */
