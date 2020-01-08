/*
 * soft_i2c.c
 *
 *  Created on: 04.01.2020
 *      Author: erics
 */
/******************************************************************************
* Includes
*******************************************************************************/
#include "soft_i2c.h"
/******************************************************************************
* Configuration
*******************************************************************************/

/******************************************************************************
* Variable Declarations
*******************************************************************************/

/******************************************************************************
* Callback's
*******************************************************************************/

/******************************************************************************
* Function Prototypes
*******************************************************************************/
static void busy_wait_us(soft_i2c_cfg_Config *self, uint32_t time_us);
static int wait_for_clock_stretching_end(soft_i2c_cfg_Config *self);

static int start_cond(soft_i2c_cfg_Config *self);
static int stop_cond(soft_i2c_cfg_Config *self);
static int write_bit(soft_i2c_cfg_Config *self, int value);
static int read_bit(soft_i2c_cfg_Config *self, uint8_t *value_p);
static int write_byte(soft_i2c_cfg_Config *self, uint8_t byte);
static int read_byte(soft_i2c_cfg_Config *self, uint8_t *byte_p, int ack);
/******************************************************************************
* Function Definitions
*******************************************************************************/
void i2c_soft_init(soft_i2c_cfg_Config *self){
    self->baudrate_us = (1000000L / 2L / self->baudrate);
    soft_i2c_bsp_gpioInit(&self->scl_pin);
    soft_i2c_bsp_gpioInit(&self->sda_pin);
    soft_i2c_bsp_timInit(&self->timer);
}

void i2c_soft_timer_us(soft_i2c_cfg_Config *self){
	if(self->timer_us > 0) --self->timer_us;
}


int i2c_soft_read(soft_i2c_cfg_Config *self, int address, uint8_t *data, int size){
	int i;
	int ack;
	int status = soft_i2c_RETURN_OK;
	/*** Read part ***/
	/* 1. Send the start condition. */
	status = start_cond(self);
	if (status != soft_i2c_RETURN_OK) return status;
	/* 2. Write the address with the direction bit set to 1.*/
	status = write_byte(self, ((address) | 0x1));
	if (status != soft_i2c_RETURN_OK) {
		stop_cond(self); /*Ignore the possible error of stop condition*/
		return status;
	}
	/* 3. Read the data. */
	for (i = 0; i < size; i++) {
		/* ACK all but last read byte. */
		ack = (i + 1 != size);
		status = read_byte(self, &data[i], ack);
		if (status != soft_i2c_RETURN_OK) {
			stop_cond(self); /*Ignore the possible error of stop condition*/
			return status;
		}
	}
	/*4. Send the stop condition. */
	status = stop_cond(self);
	return status;
}

int i2c_soft_mem_read(soft_i2c_cfg_Config *self, int address, int memAddr, int memAddrSize, uint8_t *data, int size){
	int i;
	int ack;
	int status = soft_i2c_RETURN_OK;
	/*** Write part ***/
	/* 1. Send the start condition. */
	status = start_cond(self);
	if (status != soft_i2c_RETURN_OK) return status;
	/* 2. Write the address with the direction bit set to 0.*/
	status = write_byte(self, address);
	if (status != soft_i2c_RETURN_OK) {
		stop_cond(self);	/*Ignore the possible error of stop condition*/
		return status;
	}
	/* 3. Write the mem address. */
	for (i = memAddrSize; i > 0; i--) {
		uint8_t addressPart = ((memAddr >> ((i-1) << 3)) & 0xFF);
		status = write_byte(self, addressPart);
		if (status != soft_i2c_RETURN_OK) {
			stop_cond(self); /*Ignore the possible error of stop condition*/
			return status;
		}
	}
	/*** Read part ***/
	/* 4. Send the start condition. */
	status = start_cond(self);
	if (status != soft_i2c_RETURN_OK) return status;
	/* 5. Write the address with the direction bit set to 1.*/
	status = write_byte(self, ((address) | 0x1));
	if (status != soft_i2c_RETURN_OK) {
		stop_cond(self); /*Ignore the possible error of stop condition*/
		return status;
	}
	/* 6. Read the data. */
	for (i = 0; i < size; i++) {
		/* ACK all but last read byte. */
		ack = (i + 1 != size);
		status = read_byte(self, &data[i], ack);
		if (status != soft_i2c_RETURN_OK) {
			stop_cond(self); /*Ignore the possible error of stop condition*/
			return status;
		}
	}
	/*7. Send the stop condition. */
	status = stop_cond(self);
	return status;
}

int i2c_soft_write(soft_i2c_cfg_Config *self, int address, const uint8_t *data, int size){
	int i;
	int status = soft_i2c_RETURN_OK;
	/* 1. Send the start condition. */
	status = start_cond(self);
	if (status != soft_i2c_RETURN_OK) return status;
	/* 2. Write the address with the direction bit set to 0. */
	status = write_byte(self, address);
	if (status != soft_i2c_RETURN_OK) {
		stop_cond(self);	/*Ignore the possible error of stop condition*/
		return status;
	}
	/* 3. Write the data. */
	for (i = 0; i < size; i++) {
		status = write_byte(self, data[i]);
		if (status != soft_i2c_RETURN_OK) {
			stop_cond(self); /*Ignore the possible error of stop condition*/
			return status;
		}
	}
	/*4. Send the stop condition. */
	status = stop_cond(self);
	return status;
}

int i2c_soft_mem_write(soft_i2c_cfg_Config *self, int address, int memAddr, int memAddrSize, const uint8_t *data, int size){
	int i;
	int status = soft_i2c_RETURN_OK;
	/* 1. Send the start condition. */
	status = start_cond(self);
	if (status != soft_i2c_RETURN_OK) return status;
	/* 2. Write the address with the direction bit set to 0. */
	status = write_byte(self, address);
	if (status != soft_i2c_RETURN_OK) {
		stop_cond(self);	/*Ignore the possible error of stop condition*/
		return status;
	}
	/* 3. Write the mem address. */
	for (i = memAddrSize; i > 0; i--) {
		uint8_t addressPart = ((memAddr >> ((i-1) << 3)) & 0xFF);
		status = write_byte(self, addressPart);
		if (status != soft_i2c_RETURN_OK) {
			stop_cond(self); /*Ignore the possible error of stop condition*/
			return status;
		}
	}
	/* 4. Write the data. */
	for (i = 0; i < size; i++) {
		status = write_byte(self, data[i]);
		if (status != soft_i2c_RETURN_OK) {
			stop_cond(self); /*Ignore the possible error of stop condition*/
			return status;
		}
	}
	/*5. Send the stop condition. */
	status = stop_cond(self);
	return status;
}

int i2c_soft_scan(soft_i2c_cfg_Config *self, int address){
    int res;
    int status = soft_i2c_RETURN_OK;
    /* Send the start condition. */
    status = start_cond(self);
    if (status != soft_i2c_RETURN_OK) return status;
    /* Write the address with the direction bit set to 0. */
    res = write_byte(self, address);
    /* Send the stop condition. */
    status = stop_cond(self);
    if (status != soft_i2c_RETURN_OK) return status;
    return res;
}

/* Helper ----------------------------------------------------------------------*/
static void busy_wait_us(soft_i2c_cfg_Config *self, uint32_t time_us){
	self->timer_us = time_us;
	//TODO: Enable timer
	soft_i2c_bsp_timStart(&self->timer);
//	htim16.Instance->CR1 &= ~TIM_CR1_UDIS;
	while(self->timer_us > 0);
	//TODO: stop timer
	soft_i2c_bsp_timStop(&self->timer);
//	htim16.Instance->CR1 |= TIM_CR1_UDIS;

}

static int wait_for_clock_stretching_end(soft_i2c_cfg_Config *self){
    uint32_t clock_stretching_us = 0;
    do {
        if (soft_i2c_bsp_readGpio(&self->scl_pin) == 1) {
            return soft_i2c_RETURN_OK;
        }
        busy_wait_us(self, self->clock_stretching_sleep_us);
        clock_stretching_us += self->clock_stretching_sleep_us;
    } while (clock_stretching_us < self->max_clock_stretching_us);
    return soft_i2c_RETURN_CLOCK_STRETCH;
}

static int start_cond(soft_i2c_cfg_Config *self){
	soft_i2c_bsp_setGpioMode(soft_i2c_bsp_GPIO_MODE_INPUT, &self->scl_pin);
	/* The line is busy if SDA is low. */
	soft_i2c_bsp_setGpioMode(soft_i2c_bsp_GPIO_MODE_INPUT, &self->sda_pin);
    if (soft_i2c_bsp_readGpio(&self->sda_pin) == 0) {
        return soft_i2c_RETURN_BUSY;
    }
    /* SCL is high, set SDA from 1 to 0. */
    soft_i2c_bsp_setGpioMode(soft_i2c_bsp_GPIO_MODE_OUTPUT, &self->sda_pin);
    busy_wait_us(self, self->baudrate_us);
    /* Set SCL low as preparation for the first transfer. */
    soft_i2c_bsp_setGpioMode(soft_i2c_bsp_GPIO_MODE_OUTPUT, &self->scl_pin);
    return soft_i2c_RETURN_OK;
}

static int stop_cond(soft_i2c_cfg_Config *self){
    /* Set SDA to 0. */
    soft_i2c_bsp_setGpioMode(soft_i2c_bsp_GPIO_MODE_OUTPUT, &self->sda_pin);
    busy_wait_us(self, self->baudrate_us);
    /* SDA to 1. */
    soft_i2c_bsp_setGpioMode(soft_i2c_bsp_GPIO_MODE_INPUT, &self->scl_pin);
    /* Clock stretching. */
    if (wait_for_clock_stretching_end(self) != soft_i2c_RETURN_OK) {
        return soft_i2c_RETURN_CLOCK_STRETCH;
    }
    /* Stop bit setup time, minimum 4us. */
    busy_wait_us(self, self->baudrate_us);
    /* SCL is high, set SDA from 0 to 1. */
    soft_i2c_bsp_setGpioMode(soft_i2c_bsp_GPIO_MODE_INPUT, &self->sda_pin);
    busy_wait_us(self, self->baudrate_us);
    /* Make sure no device is pulling SDA low. */
    if (soft_i2c_bsp_readGpio(&self->sda_pin) == 0) {
        return soft_i2c_RETURN_BUSY;
    }
    busy_wait_us(self, self->baudrate_us);
    return soft_i2c_RETURN_OK;
}

static int write_bit(soft_i2c_cfg_Config *self, int value){
    if (value == 1) soft_i2c_bsp_setGpioMode(soft_i2c_bsp_GPIO_MODE_INPUT, &self->sda_pin);
    else soft_i2c_bsp_setGpioMode(soft_i2c_bsp_GPIO_MODE_OUTPUT, &self->sda_pin);
    /* SDA change propagation delay. */
    busy_wait_us(self, self->baudrate_us);
    /* Set SCL high to indicate a new valid SDA value is available */
    soft_i2c_bsp_setGpioMode(soft_i2c_bsp_GPIO_MODE_INPUT, &self->scl_pin);
    /* Wait for SDA value to be read by slave, minimum of 4us for
       standard mode. */
    busy_wait_us(self, self->baudrate_us);
    /* Clock stretching */
    if (wait_for_clock_stretching_end(self) != soft_i2c_RETURN_OK) {
        return soft_i2c_RETURN_CLOCK_STRETCH;
    }
    /* Clear the SCL to low in preparation for next change. */
    soft_i2c_bsp_setGpioMode(soft_i2c_bsp_GPIO_MODE_OUTPUT, &self->scl_pin);
    return soft_i2c_RETURN_OK;
}

static int read_bit(soft_i2c_cfg_Config *self, uint8_t *value_p){
    /* Let the slave drive data. */
    soft_i2c_bsp_setGpioMode(soft_i2c_bsp_GPIO_MODE_INPUT, &self->sda_pin);
    /* Wait for SDA value to be written by slave, minimum of 4us for
       standard mode. */
    busy_wait_us(self, self->baudrate_us);
    /* Set SCL high to indicate a new valid SDA value is available. */
    soft_i2c_bsp_setGpioMode(soft_i2c_bsp_GPIO_MODE_INPUT, &self->scl_pin);
    /* Clock stretching. */
    if (wait_for_clock_stretching_end(self) != soft_i2c_RETURN_OK) {
        return soft_i2c_RETURN_CLOCK_STRETCH;
    }
    /* Wait for SDA value to be written by slave, minimum of 4us for
       standard mode. */
    busy_wait_us(self, self->baudrate_us);
    /* SCL is high, read out bit. */
    *value_p = soft_i2c_bsp_readGpio(&self->sda_pin);
    /* Set SCL low in preparation for next operation. */
    soft_i2c_bsp_setGpioMode(soft_i2c_bsp_GPIO_MODE_OUTPUT, &self->scl_pin);
    return soft_i2c_RETURN_OK;
}

static int write_byte(soft_i2c_cfg_Config *self, uint8_t byte){
    uint8_t data;
    int status = soft_i2c_RETURN_OK;
    for (int i = 0; i < 8; i++) {
    	status = write_bit(self, (byte & 0x80) != 0);
        if (status != soft_i2c_RETURN_OK) return status;
        byte <<= 1;
    }
    status = read_bit(self, &data);
    if (status != soft_i2c_RETURN_OK) return status;
    if(data == 0) return soft_i2c_RETURN_OK;
    else return soft_i2c_RETURN_NACK;
}

static int read_byte(soft_i2c_cfg_Config *self, uint8_t *byte_p, int ack){
    uint8_t bit = 0;
    *byte_p = 0;
    int status = soft_i2c_RETURN_OK;
    for (int i = 0; i < 8; i++) {
    	status = read_bit(self, &bit);
        if (status != 0) return status;
        *byte_p = ((*byte_p << 1 ) | bit);
    }
    /* Acknowledge that the byte was successfully received. */
    status = write_bit(self, !ack);
    if (status != soft_i2c_RETURN_OK) return status;
    return soft_i2c_RETURN_OK;
}

