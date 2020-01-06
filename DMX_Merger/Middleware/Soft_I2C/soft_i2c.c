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
#define pin_device_read(PIN) HAL_GPIO_ReadPin(PIN.GPIOx, PIN.GPIO_Pin)
/******************************************************************************
* Variable Declarations
*******************************************************************************/
#define gpio_setPinMode gpio_setPinMode2
//#define GPIO_Pin GPIO_Pin_BitPos
/******************************************************************************
* Callback's
*******************************************************************************/

/******************************************************************************
* Function Prototypes
*******************************************************************************/
static void busy_wait_us(soft_i2c_Config *self_p, uint32_t time_us);
static int wait_for_clock_stretching_end(soft_i2c_Config *self_p);

static int start_cond(soft_i2c_Config *self_p);
static int stop_cond(soft_i2c_Config *self_p);
static int write_bit(soft_i2c_Config *self_p, int value);
static int read_bit(soft_i2c_Config *self_p, uint8_t *value_p);
static int write_byte(soft_i2c_Config *self_p, uint8_t byte);
static int read_byte(soft_i2c_Config *self_p, uint8_t *byte_p, int ack);
/******************************************************************************
* Function Definitions
*******************************************************************************/
void i2c_soft_init(soft_i2c_Config *self_p){
    self_p->baudrate_us = (1000000L / 2L / self_p->baudrate);
//    gpio_setPinMode(GPIO_MODE_INPUT, self_p->scl_pin.GPIOx, self_p->scl_pin.GPIO_Pin_BitPos, GPIO_PIN_RESET);
//    gpio_setPinMode(GPIO_MODE_INPUT, self_p->sda_pin.GPIOx, self_p->sda_pin.GPIO_Pin_BitPos, GPIO_PIN_RESET);

    soft_i2c_bsp_gpioInit(&self_p->scl_pin);
    soft_i2c_bsp_gpioInit(&self_p->sda_pin);
    soft_i2c_bsp_timInit(&self_p->timer);

//    pin_device_set_mode(self_p->scl_pin, PIN_INPUT);
//    pin_device_set_mode(self_p->sda_pin, PIN_INPUT);

    /* The pin output values are always set to 0. The bus state is
       high by configuring the pin as an input with a pullup
       resistor. */
//    pin_device_write_low(self_p->scl_pin);
//    pin_device_write_low(self_p->sda_pin);
}

void i2c_soft_timer_us(soft_i2c_Config *self_p){
	if(self_p->timer_us > 0) --self_p->timer_us;
}


int i2c_soft_read(soft_i2c_Config *self_p, int address, uint8_t *data, int size){
	int i;
	int ack;
	int status = soft_i2c_RETURN_OK;
	/*** Read part ***/
	/* 1. Send the start condition. */
	status = start_cond(self_p);
	if (status != soft_i2c_RETURN_OK) return status;
	/* 2. Write the address with the direction bit set to 1.*/
	status = write_byte(self_p, ((address) | 0x1));
	if (status != soft_i2c_RETURN_OK) {
		stop_cond(self_p); /*Ignore the possible error of stop condition*/
		return status;
	}
	/* 3. Read the data. */
	for (i = 0; i < size; i++) {
		/* ACK all but last read byte. */
		ack = (i + 1 != size);
		status = read_byte(self_p, &data[i], ack);
		if (status != soft_i2c_RETURN_OK) {
			stop_cond(self_p); /*Ignore the possible error of stop condition*/
			return status;
		}
	}
	/*4. Send the stop condition. */
	status = stop_cond(self_p);
	return status;
}

int i2c_soft_mem_read(soft_i2c_Config *self_p, int address, int memAddr, int memAddrSize, uint8_t *data, int size){
	int i;
	int ack;
	int status = soft_i2c_RETURN_OK;
	/*** Write part ***/
	/* 1. Send the start condition. */
	status = start_cond(self_p);
	if (status != soft_i2c_RETURN_OK) return status;
	/* 2. Write the address with the direction bit set to 0.*/
	status = write_byte(self_p, address);
	if (status != soft_i2c_RETURN_OK) {
		stop_cond(self_p);	/*Ignore the possible error of stop condition*/
		return status;
	}
	/* 3. Write the mem address. */
	for (i = memAddrSize; i > 0; i--) {
		uint8_t addressPart = ((memAddr >> ((i-1) << 3)) & 0xFF);
		status = write_byte(self_p, addressPart);
		if (status != soft_i2c_RETURN_OK) {
			stop_cond(self_p); /*Ignore the possible error of stop condition*/
			return status;
		}
	}
	/*** Read part ***/
	/* 4. Send the start condition. */
	status = start_cond(self_p);
	if (status != soft_i2c_RETURN_OK) return status;
	/* 5. Write the address with the direction bit set to 1.*/
	status = write_byte(self_p, ((address) | 0x1));
	if (status != soft_i2c_RETURN_OK) {
		stop_cond(self_p); /*Ignore the possible error of stop condition*/
		return status;
	}
	/* 6. Read the data. */
	for (i = 0; i < size; i++) {
		/* ACK all but last read byte. */
		ack = (i + 1 != size);
		status = read_byte(self_p, &data[i], ack);
		if (status != soft_i2c_RETURN_OK) {
			stop_cond(self_p); /*Ignore the possible error of stop condition*/
			return status;
		}
	}
	/*7. Send the stop condition. */
	status = stop_cond(self_p);
	return status;
}

int i2c_soft_write(soft_i2c_Config *self_p, int address, const uint8_t *data, int size){
	int i;
	int status = soft_i2c_RETURN_OK;
	/* 1. Send the start condition. */
	status = start_cond(self_p);
	if (status != soft_i2c_RETURN_OK) return status;
	/* 2. Write the address with the direction bit set to 0. */
	status = write_byte(self_p, address);
	if (status != soft_i2c_RETURN_OK) {
		stop_cond(self_p);	/*Ignore the possible error of stop condition*/
		return status;
	}
	/* 3. Write the data. */
	for (i = 0; i < size; i++) {
		status = write_byte(self_p, data[i]);
		if (status != soft_i2c_RETURN_OK) {
			stop_cond(self_p); /*Ignore the possible error of stop condition*/
			return status;
		}
	}
	/*4. Send the stop condition. */
	status = stop_cond(self_p);
	return status;
}

int i2c_soft_mem_write(soft_i2c_Config *self_p, int address, int memAddr, int memAddrSize, const uint8_t *data, int size){
	int i;
	int status = soft_i2c_RETURN_OK;
	/* 1. Send the start condition. */
	status = start_cond(self_p);
	if (status != soft_i2c_RETURN_OK) return status;
	/* 2. Write the address with the direction bit set to 0. */
	status = write_byte(self_p, address);
	if (status != soft_i2c_RETURN_OK) {
		stop_cond(self_p);	/*Ignore the possible error of stop condition*/
		return status;
	}
	/* 3. Write the mem address. */
	for (i = memAddrSize; i > 0; i--) {
		uint8_t addressPart = ((memAddr >> ((i-1) << 3)) & 0xFF);
		status = write_byte(self_p, addressPart);
		if (status != soft_i2c_RETURN_OK) {
			stop_cond(self_p); /*Ignore the possible error of stop condition*/
			return status;
		}
	}
	/* 4. Write the data. */
	for (i = 0; i < size; i++) {
		status = write_byte(self_p, data[i]);
		if (status != soft_i2c_RETURN_OK) {
			stop_cond(self_p); /*Ignore the possible error of stop condition*/
			return status;
		}
	}
	/*5. Send the stop condition. */
	status = stop_cond(self_p);
	return status;
}

int i2c_soft_scan(soft_i2c_Config *self_p, int address){
    int res;
    int status = soft_i2c_RETURN_OK;
    /* Send the start condition. */
    status = start_cond(self_p);
    if (status != soft_i2c_RETURN_OK) return status;
    /* Write the address with the direction bit set to 0. */
    res = write_byte(self_p, address);
    /* Send the stop condition. */
    status = stop_cond(self_p);
    if (status != soft_i2c_RETURN_OK) return status;
    return res;
}

/* Helper ----------------------------------------------------------------------*/
static void busy_wait_us(soft_i2c_Config *self_p, uint32_t time_us){
	self_p->timer_us = time_us;
	//TODO: Enable timer
	soft_i2c_bsp_timStart(&self_p->timer);
//	htim16.Instance->CR1 &= ~TIM_CR1_UDIS;
	while(self_p->timer_us > 0);
	//TODO: stop timer
	soft_i2c_bsp_timStop(&self_p->timer);
//	htim16.Instance->CR1 |= TIM_CR1_UDIS;

}

static int wait_for_clock_stretching_end(soft_i2c_Config *self_p){
    uint32_t clock_stretching_us = 0;
    do {
        if (pin_device_read(self_p->scl_pin) == 1) {
            return soft_i2c_RETURN_OK;
        }
        busy_wait_us(self_p, self_p->clock_stretching_sleep_us);
        clock_stretching_us += self_p->clock_stretching_sleep_us;
    } while (clock_stretching_us < self_p->max_clock_stretching_us);
    return soft_i2c_RETURN_CLOCK_STRETCH;
}

static int start_cond(soft_i2c_Config *self_p){
	gpio_setPinMode(GPIO_MODE_INPUT, self_p->scl_pin.GPIOx, self_p->scl_pin.GPIO_Pin_BitPos, GPIO_PIN_RESET);
	/* The line is busy if SDA is low. */
	gpio_setPinMode(GPIO_MODE_INPUT, self_p->sda_pin.GPIOx, self_p->sda_pin.GPIO_Pin_BitPos, GPIO_PIN_RESET);
    if (pin_device_read(self_p->sda_pin) == 0) {
        return soft_i2c_RETURN_BUSY;
    }
    /* SCL is high, set SDA from 1 to 0. */
    gpio_setPinMode(GPIO_MODE_OUTPUT_OD, self_p->sda_pin.GPIOx, self_p->sda_pin.GPIO_Pin_BitPos, GPIO_PIN_RESET);
    busy_wait_us(self_p, self_p->baudrate_us);
    /* Set SCL low as preparation for the first transfer. */
    gpio_setPinMode(GPIO_MODE_OUTPUT_OD, self_p->scl_pin.GPIOx, self_p->scl_pin.GPIO_Pin_BitPos, GPIO_PIN_RESET);
    return soft_i2c_RETURN_OK;
}

static int stop_cond(soft_i2c_Config *self_p){
    /* Set SDA to 0. */
    gpio_setPinMode(GPIO_MODE_OUTPUT_OD, self_p->sda_pin.GPIOx, self_p->sda_pin.GPIO_Pin_BitPos, GPIO_PIN_RESET);
    busy_wait_us(self_p, self_p->baudrate_us);
    /* SDA to 1. */
    gpio_setPinMode(GPIO_MODE_INPUT, self_p->scl_pin.GPIOx, self_p->scl_pin.GPIO_Pin_BitPos, GPIO_PIN_RESET);
    /* Clock stretching. */
    if (wait_for_clock_stretching_end(self_p) != soft_i2c_RETURN_OK) {
        return soft_i2c_RETURN_CLOCK_STRETCH;
    }
    /* Stop bit setup time, minimum 4us. */
    busy_wait_us(self_p, self_p->baudrate_us);
    /* SCL is high, set SDA from 0 to 1. */
    gpio_setPinMode(GPIO_MODE_INPUT, self_p->sda_pin.GPIOx, self_p->sda_pin.GPIO_Pin_BitPos, GPIO_PIN_RESET);
    busy_wait_us(self_p, self_p->baudrate_us);
    /* Make sure no device is pulling SDA low. */
    if (pin_device_read(self_p->sda_pin) == 0) {
        return soft_i2c_RETURN_BUSY;
    }
    busy_wait_us(self_p, self_p->baudrate_us);
    return soft_i2c_RETURN_OK;
}

static int write_bit(soft_i2c_Config *self_p, int value){
    if (value == 1) gpio_setPinMode(GPIO_MODE_INPUT, self_p->sda_pin.GPIOx, self_p->sda_pin.GPIO_Pin_BitPos, GPIO_PIN_RESET);
    else gpio_setPinMode(GPIO_MODE_OUTPUT_OD, self_p->sda_pin.GPIOx, self_p->sda_pin.GPIO_Pin_BitPos, GPIO_PIN_RESET);
    /* SDA change propagation delay. */
    busy_wait_us(self_p, self_p->baudrate_us);
    /* Set SCL high to indicate a new valid SDA value is available */
    gpio_setPinMode(GPIO_MODE_INPUT, self_p->scl_pin.GPIOx, self_p->scl_pin.GPIO_Pin_BitPos, GPIO_PIN_RESET);
    /* Wait for SDA value to be read by slave, minimum of 4us for
       standard mode. */
    busy_wait_us(self_p, self_p->baudrate_us);
    /* Clock stretching */
    if (wait_for_clock_stretching_end(self_p) != soft_i2c_RETURN_OK) {
        return soft_i2c_RETURN_CLOCK_STRETCH;
    }
    /* Clear the SCL to low in preparation for next change. */
    gpio_setPinMode(GPIO_MODE_OUTPUT_OD, self_p->scl_pin.GPIOx, self_p->scl_pin.GPIO_Pin_BitPos, GPIO_PIN_RESET);
    return soft_i2c_RETURN_OK;
}

static int read_bit(soft_i2c_Config *self_p, uint8_t *value_p){
    /* Let the slave drive data. */
    gpio_setPinMode(GPIO_MODE_INPUT, self_p->sda_pin.GPIOx, self_p->sda_pin.GPIO_Pin_BitPos, GPIO_PIN_RESET);
    /* Wait for SDA value to be written by slave, minimum of 4us for
       standard mode. */
    busy_wait_us(self_p, self_p->baudrate_us);
    /* Set SCL high to indicate a new valid SDA value is available. */
    gpio_setPinMode(GPIO_MODE_INPUT, self_p->scl_pin.GPIOx, self_p->scl_pin.GPIO_Pin_BitPos, GPIO_PIN_RESET);
    /* Clock stretching. */
    if (wait_for_clock_stretching_end(self_p) != soft_i2c_RETURN_OK) {
        return soft_i2c_RETURN_CLOCK_STRETCH;
    }
    /* Wait for SDA value to be written by slave, minimum of 4us for
       standard mode. */
    busy_wait_us(self_p, self_p->baudrate_us);
    /* SCL is high, read out bit. */
    *value_p = pin_device_read(self_p->sda_pin);
    /* Set SCL low in preparation for next operation. */
    gpio_setPinMode(GPIO_MODE_OUTPUT_OD, self_p->scl_pin.GPIOx, self_p->scl_pin.GPIO_Pin_BitPos, GPIO_PIN_RESET);
    return soft_i2c_RETURN_OK;
}

static int write_byte(soft_i2c_Config *self_p, uint8_t byte){
    uint8_t data;
    int status = soft_i2c_RETURN_OK;
    for (int i = 0; i < 8; i++) {
    	status = write_bit(self_p, (byte & 0x80) != 0);
        if (status != soft_i2c_RETURN_OK) return status;
        byte <<= 1;
    }
    status = read_bit(self_p, &data);
    if (status != soft_i2c_RETURN_OK) return status;
    if(data == 0) return soft_i2c_RETURN_OK;
    else return soft_i2c_RETURN_NACK;
}

static int read_byte(soft_i2c_Config *self_p, uint8_t *byte_p, int ack){
    uint8_t bit = 0;
    *byte_p = 0;
    int status = soft_i2c_RETURN_OK;
    for (int i = 0; i < 8; i++) {
    	status = read_bit(self_p, &bit);
        if (status != 0) return status;
        *byte_p = ((*byte_p << 1 ) | bit);
    }
    /* Acknowledge that the byte was successfully received. */
    status = write_bit(self_p, !ack);
    if (status != soft_i2c_RETURN_OK) return status;
    return soft_i2c_RETURN_OK;
}
