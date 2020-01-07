/*
 * soft_i2c_cfg.c
 *
 *  Created on: 07.01.2020
 *      Author: erics
 */
/******************************************************************************
* Includes
*******************************************************************************/
#include "configs.h"
/******************************************************************************
* Instances definitions
*******************************************************************************/
soft_i2c_cfg_Config softI2C2 = {
		.timer_us = 0,
		.sda_pin = {.GPIOx = GPIOB, .GPIO_Pin = GPIO_PIN_11, .GPIO_Pin_BitPos = 11},
		.scl_pin = {.GPIOx = GPIOB, .GPIO_Pin = GPIO_PIN_10, .GPIO_Pin_BitPos = 10},
		.max_clock_stretching_us = 100000,
		.clock_stretching_sleep_us = 1000,
		.baudrate = 200000,
		.timer = {.TIMx = TIM16},
};
