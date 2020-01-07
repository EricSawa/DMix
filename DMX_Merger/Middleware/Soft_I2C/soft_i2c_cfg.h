/*
 * soft_i2c_cfg.h
 *
 *  Created on: 07.01.2020
 *      Author: erics
 */

#ifndef SOFT_I2C_SOFT_I2C_CFG_H_
#define SOFT_I2C_SOFT_I2C_CFG_H_
/******************************************************************************
* Includes
*******************************************************************************/
#include <stdint.h>
#include <string.h>
#include "soft_i2c_bsp.h"
/******************************************************************************
* Typedefs
*******************************************************************************/
typedef struct{
    soft_i2c_bsp_pin_device scl_pin;
    soft_i2c_bsp_pin_device sda_pin;
    soft_i2c_bsp_tim_device timer;
    uint32_t baudrate;
    uint32_t baudrate_us;
    uint32_t max_clock_stretching_us;
    uint32_t clock_stretching_sleep_us;
    volatile int32_t timer_us;
}soft_i2c_cfg_Config ;

#endif /* SOFT_I2C_SOFT_I2C_CFG_H_ */
