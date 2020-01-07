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
#include "soft_i2c_cfg.h"
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

/******************************************************************************
* Callback's
*******************************************************************************/

/******************************************************************************
* Function Prototypes
*******************************************************************************/
void i2c_soft_init(soft_i2c_cfg_Config *self);

int i2c_soft_read(soft_i2c_cfg_Config *self, int address, uint8_t *data, int size);
int i2c_soft_mem_read(soft_i2c_cfg_Config *self, int address, int memAddr, int memAddrSize, uint8_t *data, int size);

int i2c_soft_write(soft_i2c_cfg_Config *self, int address, const uint8_t *data, int size);
int i2c_soft_mem_write(soft_i2c_cfg_Config *self, int address, int memAddr, int memAddrSize, const uint8_t *data, int size);

int i2c_soft_scan(soft_i2c_cfg_Config *self, int address);

void i2c_soft_timer_us(soft_i2c_cfg_Config *self);


#endif /* SOFT_I2C_SOFT_I2C_H_ */
