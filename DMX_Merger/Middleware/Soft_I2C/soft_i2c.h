/*
 * soft_i2c.h
 *
 *  Created on: 04.01.2020
 *      Author: erics
 */

#ifndef SOFT_I2C_SOFT_I2C_H_
#define SOFT_I2C_SOFT_I2C_H_
/******************************************************************************
* Includes
*******************************************************************************/
#include "soft_i2c_bsp.h"
/******************************************************************************
* Return values
*******************************************************************************/
enum{
	soft_i2c_RETURN_CLOCK_STRETCH = -3,
	soft_i2c_RETURN_BUSY = -2,
	soft_i2c_RETURN_NACK = -1,
	soft_i2c_RETURN_OK = 0,
};
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
}soft_i2c_Config ;
/******************************************************************************
* Callback's
*******************************************************************************/



/******************************************************************************
* Function Prototypes
*******************************************************************************/
void i2c_soft_init(soft_i2c_Config *self_p);


int i2c_soft_read(soft_i2c_Config *self_p, int address, uint8_t *data, int size);
int i2c_soft_mem_read(soft_i2c_Config *self_p, int address, int memAddr, int memAddrSize, uint8_t *data, int size);

int i2c_soft_write(soft_i2c_Config *self_p, int address, const uint8_t *data, int size);
int i2c_soft_mem_write(soft_i2c_Config *self_p, int address, int memAddr, int memAddrSize, const uint8_t *data, int size);

int i2c_soft_scan(soft_i2c_Config *self_p, int address);

void i2c_soft_timer_us(soft_i2c_Config *self_p);


#endif /* SOFT_I2C_SOFT_I2C_H_ */
