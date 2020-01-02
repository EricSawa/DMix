/*
 * dmx.h
 *
 *  Created on: 24.11.2019
 *      Author: erics
 */

#ifndef DMX_DMX_H_
#define DMX_DMX_H_
/******************************************************************************
* Includes
*******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include "dmx_cfg.h"
/******************************************************************************
* Constants
*******************************************************************************/
/******************************************************************************
* Macros
*******************************************************************************/
/******************************************************************************
* Configuration
******************************************************************************/
/******************************************************************************
* Typedefs
*******************************************************************************/
/******************************************************************************
* Variable Declarations
*******************************************************************************/
/******************************************************************************
* Function Prototypes
*******************************************************************************/
void dmx_registerCallback(dmx_cfg_Instance *dmx, dmx_cfg_callback callback);
void dmx_init(dmx_cfg_Instance *dmx);
void dmx_loop(dmx_cfg_Instance *dmx);
void dmx_1ms(dmx_cfg_Instance *dmx);
void dmx_sendStart(dmx_cfg_Instance *dmx);
void dmx_txStart(dmx_cfg_Instance *dmx);
void dmx_txStop(dmx_cfg_Instance *dmx);
void dmx_rxStart(dmx_cfg_Instance *dmx);
void dmx_rxStop(dmx_cfg_Instance *dmx);
void dmx_setTxTime(dmx_cfg_Instance *dmx, dmx_cfg_Time timeIdx, uint32_t time_us);
void dmx_incrementRxUpdateCnt(dmx_cfg_Instance *dmx);

void dmx_irqTxTimerHandler(dmx_cfg_Instance *dmx);
void dmx_irqTxInterfaceHandler(dmx_cfg_Instance *dmx);

void dmx_irqRxTimerHandler(dmx_cfg_Instance *dmx);
void dmx_irqRxInterfaceHandler(dmx_cfg_Instance *dmx);

#endif /* DMX_DMX_H_ */
