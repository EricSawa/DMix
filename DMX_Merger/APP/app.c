/*
 * app.c
 *
 *  Created on: 22.11.2019
 *      Author: erics
 */
/******************************************************************************
* Includes
*******************************************************************************/
#include <stdint.h>
#include <stdlib.h>
#include "main.h"
#include "app.h"
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
/* Callback's ----------------------------------------------------------------*/
uint8_t eepromCallback(eeprom_cfg_Config *config);
/* Helper --------------------------------------------------------------------*/
/******************************************************************************
* Variable Declarations
*******************************************************************************/
/* Buffer --------------------------------------------------------------------*/
//#define MSG_BUF_SIZE 512
//char MSG_Data[MSG_BUF_SIZE] = {0};
//msg_Buffer msgBuf = msg_BUF(MSG_Data, MSG_BUF_SIZE);

#define UART_TX_SIZE 256
#define UART_RX_SIZE 256
char uart1TxData[UART_TX_SIZE] = {0};
char uart1RxData[UART_RX_SIZE] = {0};
char uart2TxData[UART_TX_SIZE] = {0};
char uart2RxData[UART_RX_SIZE] = {0};
char uart4TxData[UART_TX_SIZE] = {0};
char uart4RxData[UART_RX_SIZE] = {0};
stRingBuf uart1TxBuf = {.data = uart1TxData, .size = UART_TX_SIZE, .in = 0, .out = 0};
stRingBuf uart1RxBuf = {.data = uart1RxData, .size = UART_RX_SIZE, .in = 0, .out = 0};
stRingBuf uart2TxBuf = {.data = uart2TxData, .size = UART_TX_SIZE, .in = 0, .out = 0};
stRingBuf uart2RxBuf = {.data = uart2RxData, .size = UART_RX_SIZE, .in = 0, .out = 0};
stRingBuf uart4TxBuf = {.data = uart4TxData, .size = UART_TX_SIZE, .in = 0, .out = 0};
stRingBuf uart4RxBuf = {.data = uart4RxData, .size = UART_RX_SIZE, .in = 0, .out = 0};
/* Instances -----------------------------------------------------------------*/
/* Tasks ---------------------------------------------------------------------*/

/******************************************************************************
* Function Definitions
*******************************************************************************/
void app_main(){
	uart_enableReceiveInt(huart1.Instance);
	uart_enableReceiveInt(huart2.Instance);
	uart_enableReceiveInt(huart4.Instance);

	eeprom_registerCallback(&eepromDev1, eepromCallback);
	eeprom_init(&eepromDev1);

	HAL_GPIO_WritePin(O_USART1_DIR_GPIO_Port, O_USART1_DIR_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(O_USART2_DIR_GPIO_Port, O_USART2_DIR_Pin, GPIO_PIN_SET);
	while (1){
		if(HAL_GPIO_ReadPin(I_SW2_GPIO_Port, I_SW2_Pin) == GPIO_PIN_RESET){
			app_dmx_sendStart();
			HAL_Delay(100);
		}
	 }
}

void app_1ms(){
	static uint16_t timer100ms = 0;
	static uint16_t timer500ms = 0;
	if(++timer100ms >= 100){
		timer100ms = 0;
		uart_writeStringToBuffer("AT+NAMEERIC12  ", &uart4TxBuf, huart4.Instance);
//		uart_writeStringToBuffer("AT+NAMEERIC12  ", &uart2TxBuf, huart2.Instance);
	}
	if(++timer500ms >= 500){
		timer500ms = 0;
	}

}
/* Callback's ---------------------------------------------------------------------*/
uint8_t eepromCallback(eeprom_cfg_Config *config){
	uint8_t state = 1;
	if(config->callbackType == EEPROM_CFG_CALL_TX_START){
		if(HAL_I2C_Mem_Write(&hi2c2, config->devAddr, config->memAddr, I2C_MEMADD_SIZE_16BIT, config->txBuffer, config->dataSize, 100) != HAL_OK) state = 0;
	}
	if(config->callbackType == EEPROM_CFG_CALL_RX_START){
		if(HAL_I2C_Mem_Read(&hi2c2, config->devAddr, config->memAddr, I2C_MEMADD_SIZE_16BIT, config->rxBuffer, config->dataSize, 10) != HAL_OK) state = 0;
	}
	if(config->callbackType == EEPROM_CFG_CALL_CHECK_TX_STATE){
		if(HAL_I2C_Master_Transmit(&hi2c2, config->devAddr, config->rxBuffer, config->dataSize, 1) != HAL_OK) state = 0;
	}
	return state;
}
/* IRQs ---------------------------------------------------------------------*/
void USART1_IRQHandler(void){
	uart_irqHandler(huart1.Instance, &uart1RxBuf, &uart1TxBuf);
}

void USART2_IRQHandler(void){
	uart_irqHandler(huart2.Instance, &uart2RxBuf, &uart2TxBuf);
}

void UART4_IRQHandler(void){
	uart_irqHandler(huart4.Instance, &uart4RxBuf, &uart4TxBuf);
}


