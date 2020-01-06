/*
 * soft_i2c_bsp.h
 *
 *  Created on: 05.01.2020
 *      Author: erics
 */

#ifndef SOFT_I2C_SOFT_I2C_BSP_H_
#define SOFT_I2C_SOFT_I2C_BSP_H_

/******************************************************************************
* Includes
*******************************************************************************/
#include <stdint.h>
#include "gpio.h"
#include "tim.h"
/******************************************************************************
* Typedefs
*******************************************************************************/
typedef enum{
	soft_i2c_bsp_GPIO_MODE_INPUT,
	soft_i2c_bsp_GPIO_MODE_OUTPUT,
}i2c_bsp_GpioMode;

typedef struct{
	GPIO_TypeDef *GPIOx;
	uint16_t GPIO_Pin;
	uint8_t GPIO_Pin_BitPos;
}soft_i2c_bsp_pin_device;

typedef struct{
	TIM_TypeDef *TIMx;
}soft_i2c_bsp_tim_device;

/******************************************************************************
* Function Implementation
*******************************************************************************/
#define soft_i2c_bsp_GPIO_PULL_MSK 			0x03
#define soft_i2c_bsp_GPIO_PULL_NONE 		0x00

#define soft_i2c_bsp_GPIO_SPEED_MSK 		0x03
#define soft_i2c_bsp_GPIO_SPEED_VERY_HIGH 	0x03

#define soft_i2c_bsp_GPIO_MODE_MSK 			0x03
#define soft_i2c_bsp_GPIO_MODE_INPUT_FLOAT 	0x00
#define soft_i2c_bsp_GPIO_MODE_OUTPUT_OD 	0x01

static inline void soft_i2c_bsp_gpioInit(soft_i2c_bsp_pin_device *pin){
	/*Enable open drain output*/
	pin->GPIOx->OTYPER |= pin->GPIO_Pin;
	/*Set as input*/
	pin->GPIOx->MODER &= ~(soft_i2c_bsp_GPIO_MODE_MSK << (pin->GPIO_Pin_BitPos << 1));
	pin->GPIOx->MODER |= (soft_i2c_bsp_GPIO_MODE_INPUT_FLOAT << (pin->GPIO_Pin_BitPos << 1));
	/*Disable all pull ups or pull downs*/
	pin->GPIOx->PUPDR &= ~(soft_i2c_bsp_GPIO_PULL_MSK << (pin->GPIO_Pin_BitPos << 1));
	pin->GPIOx->PUPDR |= (soft_i2c_bsp_GPIO_PULL_NONE << (pin->GPIO_Pin_BitPos << 1));
	/*Set gpio speed as high as possible*/
	pin->GPIOx->OSPEEDR &= ~(soft_i2c_bsp_GPIO_SPEED_MSK << (pin->GPIO_Pin_BitPos << 1));
	pin->GPIOx->OSPEEDR |= soft_i2c_bsp_GPIO_SPEED_VERY_HIGH << (pin->GPIO_Pin_BitPos << 1);
	/*Set gpio output level always to low*/
	pin->GPIOx->ODR &= ~pin->GPIO_Pin;
}


static inline void soft_i2c_bsp_setGpioMode(i2c_bsp_GpioMode mode, soft_i2c_bsp_pin_device *pin){
	if(mode == soft_i2c_bsp_GPIO_MODE_INPUT){
		pin->GPIOx->MODER &= ~(soft_i2c_bsp_GPIO_MODE_MSK << (pin->GPIO_Pin_BitPos << 1));
		pin->GPIOx->MODER |= (soft_i2c_bsp_GPIO_MODE_INPUT_FLOAT << (pin->GPIO_Pin_BitPos << 1));
	}else{
		pin->GPIOx->MODER &= ~(soft_i2c_bsp_GPIO_MODE_MSK << (pin->GPIO_Pin_BitPos << 1));
		pin->GPIOx->MODER |= (soft_i2c_bsp_GPIO_MODE_OUTPUT_OD << (pin->GPIO_Pin_BitPos << 1));
	}
}

static inline void soft_i2c_bsp_timInit(soft_i2c_bsp_tim_device *tim){
	tim->TIMx->CR1 &= ~TIM_CR1_CEN;		//Stop counter
	tim->TIMx->CR1 |= TIM_CR1_UDIS; 	//Avoid that update can be triggered
	tim->TIMx->SR &= ~TIM_SR_UIF;		//Kill the last update flag
	tim->TIMx->EGR |= TIM_EGR_UG;		//Force reload of shadow register -> Start from beginning
	tim->TIMx->DIER |= TIM_DIER_UIE;	//Enable update interrupts
	tim->TIMx->CR1 |= TIM_CR1_CEN;		//Enable counter
}

static inline void soft_i2c_bsp_timStart(soft_i2c_bsp_tim_device *tim){
//	tim->TIMx->CR1 |= TIM_CR1_CEN;		//Enable counter
	tim->TIMx->CR1 &= ~TIM_CR1_UDIS;	//Allow that update can be triggered
}

static inline void soft_i2c_bsp_timStop(soft_i2c_bsp_tim_device *tim){
//	tim->TIMx->CR1 &= ~TIM_CR1_CEN;		//Enable counter
	tim->TIMx->CR1 |= TIM_CR1_UDIS;		//Avoid that update can be triggered
}

#endif /* SOFT_I2C_SOFT_I2C_BSP_H_ */
