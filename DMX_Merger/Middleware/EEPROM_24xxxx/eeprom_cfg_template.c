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
eeprom_cfg_Config eepromDev1_temp = {
	.devAddr = 0xA0,
	.memorySize = EEPROM_CFG_MEMSIZE_64KBIT,
	.pageBufferSize = EEPROM_CFG_PAGESIZE_32BYTE,
	.dataSize = 0,
	.maxAddress = 0,
	.callback = NULL,
	.txBuffer = NULL,
	.rxBuffer = NULL,
#ifdef IPA_2CH
#endif
#ifdef IPA_4CH
#endif
#ifdef IPA_8CH
#endif
};

