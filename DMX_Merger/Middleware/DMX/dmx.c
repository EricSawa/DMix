/*
 * dmx.c
 *
 *  Created on: 24.11.2019
 *      Author: erics
 */
/******************************************************************************
* Includes
*******************************************************************************/
#include "dmx.h"
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
* Function Prototypes
*******************************************************************************/
/******************************************************************************
* Variable Declarations
*******************************************************************************/
/******************************************************************************
* Function Definitions
*******************************************************************************/
void dmx_registerCallback(dmx_cfg_Instance *dmx, dmx_cfg_callback callback){
	dmx->callback = callback;
}

void dmx_init(dmx_cfg_Instance *dmx){
	if(dmx->type == dmx_cfg_TYPE_TRANSMITTER){
		gpio_ConfigureToFloating(dmx->txBreakPort, dmx->txBreakPin);
		dmx->txState = dmx_cfg_TX_STATE_NONE;
		dmx->txBreakToBreakCnt = 0;
		dmx->txBreakToBreakTime_ms = dmx->txBreakToBreakTime_ms + (dmx->txBreakTime_us + dmx->txMarkAfterBreakTime_us + dmx->txMarkBetweenTime_us*dmx->universeSize)/1000;
	}else if(dmx->type == dmx_cfg_TYPE_RECEIVER){
		dmx->rxState = dmx_cfg_RX_STATE_NONE;
		dmx->rxFallingTime = 0;
		dmx->rxRisingTime = 0;
		dmx->rxTimerUpdateCnt = 0;
	}
	dmx->universeCnt = 0;
}

void dmx_loop(dmx_cfg_Instance *dmx){
	if(dmx->type == dmx_cfg_TYPE_TRANSMITTER){
		if(dmx->txState == dmx_cfg_TX_STATE_NONE) return;
		if(dmx->txBreakToBreakCnt == 0){
			dmx_sendStart(dmx);
			dmx->txBreakToBreakCnt = dmx->txBreakToBreakTime_ms;
		}
	}else if(dmx->type == dmx_cfg_TYPE_RECEIVER){
	}
	/*Loop flags*/
	if(dmx->universeComplete){
		dmx->universeComplete = false;
		if(dmx->type == dmx_cfg_TYPE_TRANSMITTER) dmx->callbackType = dmx_cfg_CALL_TX_COMPLETE;
		else dmx->callbackType = dmx_cfg_CALL_RX_COMPLETE;
		if(dmx->callback) dmx->callback(dmx);
	}
}

void dmx_1ms(dmx_cfg_Instance *dmx){
	if(dmx->txBreakToBreakCnt > 0) --dmx->txBreakToBreakCnt;
}

void dmx_sendStart(dmx_cfg_Instance *dmx){
	if(dmx->type != dmx_cfg_TYPE_TRANSMITTER) return;
	if(dmx->universe == NULL) return;
	if(dmx->txBreakToBreakTime_ms < dmx_cfg_MIN_BREAK_TO_BREAK_TIME_MS) dmx->txBreakToBreakTime_ms = dmx_cfg_MIN_BREAK_TO_BREAK_TIME_MS;
	if(dmx->txBreakTime_us < dmx_cfg_MIN_BREAK_TIME_US) dmx->txBreakTime_us = dmx_cfg_MIN_BREAK_TIME_US;
	if(dmx->txMarkAfterBreakTime_us < dmx_cfg_MIN_MARK_AFTER_BREAK_TIME_US) dmx->txMarkAfterBreakTime_us = dmx_cfg_MIN_MARK_AFTER_BREAK_TIME_US;
	dmx->universeComplete = false;
	dmx->txState = dmx_cfg_TX_STATE_SEND_BREAK;
	tim_setNewTimeAndStart(dmx->txTimer, dmx->txBreakTime_us);
	gpio_ConfigureToPushPull(dmx->txBreakPort, dmx->txBreakPin);
}

void dmx_setTxTime(dmx_cfg_Instance *dmx, dmx_cfg_Time timeIdx, uint32_t time_us){
	if(timeIdx == dmx_cfg_TIME_BREAK) dmx->txBreakTime_us = time_us;
	else if(timeIdx == dmx_cfg_TIME_MARK_AFTER_BREAK) dmx->txMarkAfterBreakTime_us = time_us;
	else if(timeIdx == dmx_cfg_TIME_MARK_BETWEEN) dmx->txMarkBetweenTime_us = time_us;
	else if(timeIdx == dmx_cfg_TIME_BREAK_TO_BREAK) dmx->txBreakToBreakTime_ms = (time_us + dmx->txBreakTime_us + dmx->txMarkAfterBreakTime_us + dmx->txMarkBetweenTime_us*dmx->universeSize)/1000;
}

void dmx_txStart(dmx_cfg_Instance *dmx){
	if(dmx->type != dmx_cfg_TYPE_TRANSMITTER) return;
	dmx->txState = dmx_cfg_TX_STATE_WAIT_FOR_BREAK;
}

void dmx_txStop(dmx_cfg_Instance *dmx){
	if(dmx->type != dmx_cfg_TYPE_TRANSMITTER) return;
	dmx->txState = dmx_cfg_TX_STATE_NONE;
}

void dmx_rxStart(dmx_cfg_Instance *dmx){
	if(dmx->type != dmx_cfg_TYPE_RECEIVER) return;
	app_uart_enableReceiveInt(dmx->uart);
	dmx->rxState = dmx_cfg_RX_STATE_WAIT_FOR_BREAK;
}

void dmx_rxStop(dmx_cfg_Instance *dmx){
	if(dmx->type != dmx_cfg_TYPE_RECEIVER) return;
	app_uart_disableReceiveInt(dmx->uart);
	dmx->rxState = dmx_cfg_RX_STATE_NONE;
}

void dmx_incrementRxUpdateCnt(dmx_cfg_Instance *dmx){
	++dmx->rxTimerUpdateCnt;
}

/*Transmit IRQs*/
void dmx_irqTxTimerHandler(dmx_cfg_Instance *dmx){
	if(dmx->txState == dmx_cfg_TX_STATE_NONE) return;
	if(dmx->txState == dmx_cfg_TX_STATE_SEND_BREAK){
		gpio_ConfigureToFloating(dmx->txBreakPort, dmx->txBreakPin);
		tim_setNewTimeAndStart(dmx->txTimer, dmx->txMarkAfterBreakTime_us);
		dmx->txState = dmx_cfg_TX_STATE_SEND_MARK;
	}else if(dmx->txState == dmx_cfg_TX_STATE_SEND_MARK){
		dmx->universeCnt = 0;
		dmx->uart->CR1 &= ~USART_CR1_TXEIE;
		dmx->uart->TDR = dmx->universeStartCode;
		if(dmx->txMarkBetweenTime_us == 0) {
			dmx->uart->CR1 |= USART_CR1_TXEIE;		//start with transmission --> enable transmitbuffer empty interrupt
			tim_stop(dmx->txTimer);
			dmx->txState = dmx_cfg_TX_STATE_WAIT_FOR_BREAK;	//The whole universe can be transmitted as fast as possible
		}
		else{
			tim_setNewTimeAndStart(dmx->txTimer, dmx->txMarkBetweenTime_us);
			dmx->txState = dmx_cfg_TX_STATE_SEND_UNIVERSE;	//The universe can be transmitted with defined delay between all data bytes
		}
	}else if(dmx->txState == dmx_cfg_TX_STATE_SEND_UNIVERSE){
		dmx->uart->CR1 |= USART_CR1_TXEIE;		//start with transmission --> enable transmitbuffer empty interrupt
	}
}

void dmx_irqTxInterfaceHandler(dmx_cfg_Instance *dmx){
	if (dmx->uart->ISR & USART_ISR_TXE){				//transmit dmx values
		dmx->uart->ISR &= ~USART_ISR_TXE; //Kill transmit-buffer empty flag
		if(dmx->txState == dmx_cfg_TX_STATE_NONE) return;
		if((dmx->txState == dmx_cfg_TX_STATE_WAIT_FOR_BREAK)||(dmx->txState == dmx_cfg_TX_STATE_SEND_UNIVERSE)){
			if(dmx->universeCnt < dmx->universeSize){
				dmx->uart->CR1 &= ~USART_CR1_TXEIE;
				dmx->uart->TDR = dmx->universe[dmx->universeCnt];
				dmx->universeCnt++;
				if(dmx->txState == dmx_cfg_TX_STATE_SEND_UNIVERSE) tim_setNewTimeAndStart(dmx->txTimer, dmx->txMarkBetweenTime_us);
				else dmx->uart->CR1 |= USART_CR1_TXEIE;
			}else{
				dmx->universeCnt = 0;
				dmx->uart->CR1 &= ~USART_CR1_TXEIE;	//disable transmit-buffer empty interrupt
				dmx->txState = dmx_cfg_TX_STATE_WAIT_FOR_BREAK;
				tim_stop(dmx->txTimer);
				dmx->universeComplete = true;
			}
		}

	}
}

/*Receive IRQs*/
void dmx_irqRxTimerHandler(dmx_cfg_Instance *dmx){
//TODO: Add measurement of break
}

void dmx_irqRxInterfaceHandler(dmx_cfg_Instance *dmx){
	uint32_t sr = dmx->uart->ISR;
	if (sr & USART_ISR_RXNE){
		if(sr & USART_ISR_ORE) dmx->uart->ICR |= USART_ICR_ORECF; /*Kill overrun error*/
		uint8_t  dmxByte = (uint8_t)dmx->uart->RDR;
		if(dmx->rxState == dmx_cfg_RX_STATE_NONE) return;
		if(sr & USART_ISR_FE){	/*Frame error*/
			dmx->uart->ICR |= USART_ICR_FECF; /*Kill frame error*/
			dmx->universeCnt = 0;
			dmx->rxState = dmx_cfg_RX_STATE_RECEIVE_START_CODE;
			dmx->universeComplete = false;
		}else if(dmx->rxState == dmx_cfg_RX_STATE_RECEIVE_START_CODE){
			dmx->rxState = dmx_cfg_RX_STATE_RECEIVE_UNIVERSE;
			dmx->universeStartCode = dmxByte;
		}else if(dmx->rxState == dmx_cfg_RX_STATE_RECEIVE_UNIVERSE){
			dmx->universe[dmx->universeCnt] = dmxByte;
			dmx->universeCnt++;
			if(dmx->universeCnt >= dmx->universeSize){
				dmx->universeComplete = true;
				dmx->rxState = dmx_cfg_RX_STATE_WAIT_FOR_BREAK;
			}
		}
	}
}
