/*
 * app_dmx.c
 *
 *  Created on: 24.11.2019
 *      Author: erics
 */
/******************************************************************************
* Includes
*******************************************************************************/
#include "app_dmx.h"
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
typedef enum{
	app_dmx_STATE_NONE,
	app_dmx_STATE_SEND_START,
	app_dmx_STATE_SEND_MARK,
}app_dmx_State;
/******************************************************************************
* Function Prototypes
*******************************************************************************/
/******************************************************************************
* Variable Declarations
*******************************************************************************/
app_dmx_State dmxState;
/******************************************************************************
* Function Definitions
*******************************************************************************/
void app_dmx_sendStart(){
	gpioConfigureToFloating(O_USART2_BREAK_GPIO_Port, O_USART2_BREAK_Pin);
	dmxState = app_dmx_STATE_SEND_START;
	HAL_TIM_Base_Start_IT(&htim6);
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	if(dmxState == app_dmx_STATE_SEND_START){
		gpioConfigureToPushPull(O_USART2_BREAK_GPIO_Port, O_USART2_BREAK_Pin);
		//TODO: Set timer to mark time
		dmxState = app_dmx_STATE_SEND_MARK;
	}else if(dmxState == app_dmx_STATE_SEND_MARK){
		gpioConfigureToFloating(O_USART2_BREAK_GPIO_Port, O_USART2_BREAK_Pin);
		HAL_TIM_Base_Stop_IT(&htim6);
	}
}
