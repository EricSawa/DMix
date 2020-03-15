/*
 * eeprom.h
 *
 *  Created on: 20.07.2018
 *      Author: sawadee
 */

#ifndef EEPROM_24XXXX_EEPROM_H_
#define EEPROM_24XXXX_EEPROM_H_
/******************************************************************************
* Includes
*******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include "eeprom_cfg.h"
/******************************************************************************
* Function Prototypes
*******************************************************************************/
/* Examples for an valid callback function
 * uint8_t EEPROM_Callback(eeprom_cfg_Config *config){
 * 		if(config->callbackType == EEPROM_CFG_CALL_TX_START) 		//Start transmission
 * 		if(config->callbackType == EEPROM_CFG_CALL_RX_START)		//Start receive
 * 		...
 * 		return OK;
 */
void eeprom_registerCallback(eeprom_cfg_Config *config, eeprom_cfg_callback callback);
void eeprom_init(eeprom_cfg_Config *config);
void eeprom_write(eeprom_cfg_Config *config, uint16_t startAddr, uint8_t *data, uint16_t dataSize);
bool eeprom_isFree(eeprom_cfg_Config *config);
void eeprom_read(eeprom_cfg_Config *config, uint16_t startAddr, uint8_t *data, uint16_t dataSize);


#endif /* EEPROM_24XXXX_EEPROM_H_ */
