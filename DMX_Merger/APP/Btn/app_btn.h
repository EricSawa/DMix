/*
 * app_btn.h
 *
 *  Created on: 26.01.2019
 *      Author: erics
 */

#ifndef BTN_APP_BTN_H_
#define BTN_APP_BTN_H_
/******************************************************************************
* Includes
*******************************************************************************/
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "app_cfg.h"
#include "configs.h"
#include "eal_task.h"
#include "main.h"
/******************************************************************************
* Constants
*******************************************************************************/
#define app_btn_NONE		0xFF
#define app_btn_NMBR_REG	0
#define app_btn_NMBR_OTH	app_cfg_NMBR_BTNS
/******************************************************************************
* Typedefs
*******************************************************************************/

/******************************************************************************
* Callback's
*******************************************************************************/

/******************************************************************************
* Function Prototypes
*******************************************************************************/
void app_btn_init(eal_task_Task *self);
void app_btn_process(eal_task_Task *self);
void app_btn_cyclic1ms(eal_task_Task *self, bool stat);
void app_btn_receiveMsg(eal_task_Task *self, msg_Message *message);


#endif /* BTN_APP_BTN_H_ */
