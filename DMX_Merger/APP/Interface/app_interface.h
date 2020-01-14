/*
 * app_interface.h
 *
 *  Created on: 12.01.2020
 *      Author: erics
 */

#ifndef INTERFACE_APP_INTERFACE_H_
#define INTERFACE_APP_INTERFACE_H_
/******************************************************************************
* Includes
*******************************************************************************/
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "gpio.h"
#include "usart.h"
#include "usbd_cdc_if.h"
#include "app_cfg.h"
#include "app_uart.h"
#include "app_usb.h"
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
void app_interface_init(eal_task_Task *self);
void app_interface_process(eal_task_Task *self);
void app_interface_cyclic1ms(eal_task_Task *self, bool stat);
void app_interface_receiveMsg(eal_task_Task *self, msg_Message *message);



#endif /* INTERFACE_APP_INTERFACE_H_ */
