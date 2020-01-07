/*
 * app_btn.c
 *
 *  Created on: 26.01.2019
 *      Author: erics
 */
/******************************************************************************
* Includes
*******************************************************************************/
#include "app_btn.h"
/******************************************************************************
* Configuration
*******************************************************************************/
const uint8_t swIndex[app_cfg_NMBR_BTNS] = {
	0x00, 0x01, 0x02, 0x03, 0x04

};
static uint8_t othButtonState[app_btn_NMBR_OTH] = {0};
static uint8_t othButtonBuffer[app_btn_NMBR_OTH] = {0};
static uint16_t othButtonTime[app_cfg_NMBR_BTNS] = {0};
static int16_t othButtonHoldTimeShort[app_cfg_NMBR_BTNS] = {0};
static int16_t othButtonHoldTimeLong[app_cfg_NMBR_BTNS] = {0};
/******************************************************************************
* Variable Declarations
*******************************************************************************/
static volatile bool cyclicFlag = false;
static volatile int16_t internalTimer = 0;
/******************************************************************************
* Callback's
*******************************************************************************/

/******************************************************************************
* Function Prototypes
*******************************************************************************/
static void getOthSwitches(eal_task_Task *self);
static void debounceOthSwitches(eal_task_Task *self);
/******************************************************************************
* Function Definitions
*******************************************************************************/
void app_btn_init(eal_task_Task *self){
	memset(othButtonState, 0, sizeof(othButtonState));
	memset(othButtonBuffer, 0, sizeof(othButtonBuffer));
	memset(othButtonTime, 0, sizeof(othButtonTime));
	memset(othButtonHoldTimeShort, 0, sizeof(othButtonHoldTimeShort));
	memset(othButtonHoldTimeLong, 0, sizeof(othButtonHoldTimeLong));
}

void app_btn_process(eal_task_Task *self){
	if(!self->enable) return;
	if(!cyclicFlag) return;
	getOthSwitches(self);
	debounceOthSwitches(self);
	cyclicFlag = false;
}

void app_btn_cyclic1ms(eal_task_Task *self, bool stat){
	cyclicFlag = true;
	if(internalTimer > 0)--internalTimer;
}

void app_btn_receiveMsg(eal_task_Task *self, msg_Message *message){

}

/* Helper ----------------------------------------------------------------------*/
static void getOthSwitches(eal_task_Task *self){
	if(HAL_GPIO_ReadPin(I_SW2_GPIO_Port, I_SW2_Pin) == 0) 		othButtonBuffer[0] |= (1 << 0); else othButtonBuffer[0] &= ~(1 << 0);
	if(HAL_GPIO_ReadPin(I_GPIO1_GPIO_Port, I_GPIO1_Pin) == 0) 	othButtonBuffer[0] |= (1 << 1); else othButtonBuffer[0] &= ~(1 << 1);
	if(HAL_GPIO_ReadPin(I_GPIO2_GPIO_Port, I_GPIO2_Pin) == 0) 	othButtonBuffer[0] |= (1 << 2); else othButtonBuffer[0] &= ~(1 << 2);
	if(HAL_GPIO_ReadPin(I_GPIO3_GPIO_Port, I_GPIO3_Pin) == 0) 	othButtonBuffer[0] |= (1 << 3); else othButtonBuffer[0] &= ~(1 << 3);
	if(HAL_GPIO_ReadPin(I_GPIO4_GPIO_Port, I_GPIO4_Pin) == 0) 	othButtonBuffer[0] |= (1 << 4); else othButtonBuffer[0] &= ~(1 << 4);

}

static void debounceOthSwitches(eal_task_Task *self){
	static uint8_t swNmbr = 0;
	if(swIndex[swNmbr] != app_btn_NONE){
		uint16_t msk = 1 << (swIndex[swNmbr]&0xf);
		uint8_t  idx = (swIndex[swNmbr]&0xf0) >> 4;
		if (!(othButtonBuffer[idx] & msk)){ 			/*button is released*/
			if(othButtonTime[swNmbr] > 0){
				--othButtonTime[swNmbr];
				if(othButtonTime[swNmbr] <= 0){
					if(othButtonState[idx] & msk){
						if(self->sendMsg != NULL){
							msg_Message newMessage = msg_LITERAL(app_cfg_BTN_MSG, msg_EVENT(swNmbr), app_cfg_btn_STAT_RELEASED, 0, 0);
							self->sendMsg(self, &newMessage);
						}
						othButtonState[idx] &=~msk;
						othButtonHoldTimeShort[swNmbr] = 0;
						othButtonHoldTimeLong[swNmbr] = 0;
					}
				}
			}
		}else{					 						/*button is down*/
			if(othButtonTime[swNmbr] < app_cfg_btn_TIME_DEBOUNCE){
				++othButtonTime[swNmbr];
				if(othButtonTime[swNmbr] >= app_cfg_btn_TIME_DEBOUNCE){
					if(self->sendMsg != NULL){
						msg_Message newMessage = msg_LITERAL(app_cfg_BTN_MSG, msg_EVENT(swNmbr), app_cfg_btn_STAT_PRESSED, 0, 0);
						self->sendMsg(self, &newMessage);
					}
					othButtonState[idx] |= msk;
					othButtonHoldTimeShort[swNmbr] = app_cfg_btn_TIME_HOLD_SHORT;
					othButtonHoldTimeLong[swNmbr] = app_cfg_btn_TIME_HOLD_LONG;
				}
			}
		}
		if(othButtonHoldTimeShort[swNmbr]>0){
			--othButtonHoldTimeShort[swNmbr];
			if(othButtonHoldTimeShort[swNmbr] == 0) {
				if(self->sendMsg != NULL){
					msg_Message newMessage = msg_LITERAL(app_cfg_BTN_MSG, msg_EVENT(swNmbr), app_cfg_btn_STAT_HOLD_SHORT, 0, 0);
					self->sendMsg(self, &newMessage);
				}
			}
		}
		if(othButtonHoldTimeLong[swNmbr]>0){
			--othButtonHoldTimeLong[swNmbr];
			if(othButtonHoldTimeLong[swNmbr] == 0) {
				if(self->sendMsg != NULL){
					msg_Message newMessage = msg_LITERAL(app_cfg_BTN_MSG, msg_EVENT(swNmbr), app_cfg_btn_STAT_HOLD_LONG, 0, 0);
					self->sendMsg(self, &newMessage);
				}
			}
		}
	}
	if(++swNmbr >= (app_cfg_NMBR_BTNS))swNmbr = 0;
}
