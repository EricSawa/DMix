/*
 * app_footcontrol.h
 *
 *  Created on: 07.01.2020
 *      Author: erics
 */

#ifndef FOOTCONTROL_APP_FOOTCONTROL_H_
#define FOOTCONTROL_APP_FOOTCONTROL_H_
/******************************************************************************
* Includes
*******************************************************************************/
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "gpio.h"
#include "app_cfg.h"
#include "configs.h"
#include "eal_task.h"
#include "main.h"
/******************************************************************************
* Constants
*******************************************************************************/

/******************************************************************************
* Typedefs
*******************************************************************************/

/******************************************************************************
* Callback's
*******************************************************************************/

/******************************************************************************
* Function Prototypes
*******************************************************************************/
void app_footcontrol_init(eal_task_Task *self);
void app_footcontrol_process(eal_task_Task *self);
void app_footcontrol_cyclic1ms(eal_task_Task *self, bool stat);
void app_footcontrol_receiveMsg(eal_task_Task *self, msg_Message *message);



#endif /* FOOTCONTROL_APP_FOOTCONTROL_H_ */
