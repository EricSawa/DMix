/*
 * eeprom_cfg.c
 *
 *  Created on: 20.07.2018
 *      Author: sawadee
 */
/******************************************************************************
* Includes
*******************************************************************************/
#include "eeprom_cfg.h"
/******************************************************************************
* Instances definitions
*******************************************************************************/
eeprom_cfg_Config eepromDev1 = {
	.devAddr = 0xA0,
	.memorySize = EEPROM_CFG_MEMSIZE_1MBIT,
	.pageBufferSize = EEPROM_CFG_PAGESIZE_256BYTE,
	.dataSize = 0,
	.maxAddress = 0,
	.callback = NULL,
	.txBuffer = NULL,
	.rxBuffer = NULL,
};

