/*
 * dmx_cfg.h
 *
 *  Created on: 31.12.2019
 *      Author: erics
 */

#ifndef DMX_DMX_CFG_H_
#define DMX_DMX_CFG_H_
/******************************************************************************
* Includes
*******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include "gpio.h"
#include "usart.h"
#include "tim.h"
#include "usart.h"
/******************************************************************************
* Constants
*******************************************************************************/
#define dmx_cfg_MIN_BREAK_TIME_US				96
#define dmx_cfg_MAX_BREAK_TIME_US				96

#define dmx_cfg_MIN_MARK_AFTER_BREAK_TIME_US	12
#define dmx_cfg_MIN_MARK_BETWEEN_TIME_US		0
#define dmx_cfg_MIN_BREAK_TO_BREAK_TIME_MS		25
/******************************************************************************
* Macros
*******************************************************************************/

/******************************************************************************
* Configuration
*******************************************************************************/

/******************************************************************************
* Typedefs
*******************************************************************************/
typedef enum{
	dmx_cfg_TYPE_TRANSMITTER,
	dmx_cfg_TYPE_RECEIVER
}dmx_cfg_Type;

typedef enum{
	dmx_cfg_TIME_BREAK,
	dmx_cfg_TIME_MARK_AFTER_BREAK,
	dmx_cfg_TIME_MARK_BETWEEN,
	dmx_cfg_TIME_BREAK_TO_BREAK,
}dmx_cfg_Time;

typedef enum{
	dmx_cfg_TX_STATE_NONE,
	dmx_cfg_TX_STATE_WAIT_FOR_BREAK,
	dmx_cfg_TX_STATE_SEND_BREAK,
	dmx_cfg_TX_STATE_SEND_MARK,
	dmx_cfg_TX_STATE_SEND_START_CODE,
	dmx_cfg_TX_STATE_SEND_UNIVERSE,
}dmx_cfg_TxState;

typedef enum{
	dmx_cfg_RX_STATE_NONE,
	dmx_cfg_RX_STATE_WAIT_FOR_BREAK,
	dmx_cfg_RX_STATE_WAIT_FOR_MARK_AFTER_BREAK,
	dmx_cfg_RX_STATE_RECEIVE_START_CODE,
	dmx_cfg_RX_STATE_RECEIVE_UNIVERSE,
}dmx_cfg_RxState;

typedef enum{
	dmx_cfg_CALL_TX_COMPLETE,
	dmx_cfg_CALL_RX_COMPLETE,
	dmx_cfg_CALL_MAX
}dmx_cfg_CallbackType;

typedef struct DMX_CFG_INSTANCE{
	dmx_cfg_Type	type;
	/*Transmitter settings*/
	dmx_cfg_TxState txState;
	GPIO_TypeDef* txBreakPort;
	uint16_t txBreakPin;
	TIM_TypeDef *txTimer;
	int32_t txBreakTime_us;
	int32_t txMarkAfterBreakTime_us;
	int32_t txMarkBetweenTime_us;
	int32_t txBreakToBreakTime_ms;
	int32_t txBreakToBreakCnt;
	/*Receiver settings*/
	dmx_cfg_RxState rxState;
	TIM_TypeDef *rxTimer;
	uint32_t rxTimerChannel;
	int32_t rxFallingTime;
	int32_t rxRisingTime;
	int32_t rxMeasuredBreakTime;
	int32_t rxMeasuredMABTime;
	int32_t rxTimerUpdateCnt;
	/*Interface settings*/
	USART_TypeDef *uart;
	/*Universe settings*/
	uint16_t universeSize;
	uint8_t *universe;
	uint16_t universeCnt;
	uint8_t universeStartCode;
	bool universeComplete;
	/*Callback*/
	bool (*callback)(struct DMX_CFG_INSTANCE *instance);
	dmx_cfg_CallbackType callbackType;
}dmx_cfg_Instance;

typedef bool (*dmx_cfg_callback)(dmx_cfg_Instance *instance);


#endif /* DMX_DMX_CFG_H_ */
