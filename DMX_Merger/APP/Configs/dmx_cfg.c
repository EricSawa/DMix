/*
 * dmx_cfg.c
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
uint8_t dmx1Universe[512] = {0};
uint8_t dmx2Universe[512] = {0};
/* Instances -----------------------------------------------------------------*/
dmx_cfg_Instance dmx1Out = {
		.type = dmx_cfg_TYPE_TRANSMITTER,
		/*Tx definitions*/
		.txState = dmx_cfg_TX_STATE_NONE,
		.txBreakPort = O_USART2_BREAK_GPIO_Port,
		.txBreakPin = O_USART2_BREAK_Pin,
		.txTimer = TIM6,
		.txBreakTime_us = 100,
		.txMarkAfterBreakTime_us = 50,
		.txMarkBetweenTime_us = 0,
		.txBreakToBreakTime_ms = dmx_cfg_MIN_BREAK_TO_BREAK_TIME_MS + 10,
		/*Rx definitions*/
		.uart = USART2,
		.universe = dmx1Universe,
		.universeSize = sizeof(dmx1Universe),
//		.universeSize = 5,
};

dmx_cfg_Instance dmx2In = {
		.type = dmx_cfg_TYPE_RECEIVER,
		/*Rx definitions*/
		.rxState = dmx_cfg_RX_STATE_NONE,
		.rxTimer = TIM3,
		.rxTimerChannel = TIM_CHANNEL_2,
		.uart = USART1,
		.universe = dmx2Universe,
		.universeSize = sizeof(dmx2Universe),
};

