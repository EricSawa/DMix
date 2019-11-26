/*
 * eeprom.c
 *
 *  Created on: 20.07.2018
 *      Author: sawadee
 */
/******************************************************************************
* Includes
*******************************************************************************/
#include "eeprom.h"
/******************************************************************************
* Constants
*******************************************************************************/
#define OK		1
#define NOK		0
/******************************************************************************
* Function Prototypes
*******************************************************************************/
static inline uint8_t writeValidation(eeprom_cfg_Config *config, uint16_t startAddr, uint8_t *data, uint8_t dataSize);
static inline uint8_t readValidation(eeprom_cfg_Config *config, uint16_t startAddr, uint8_t *data, uint16_t dataSize);
static inline void writeStartAddr(eeprom_cfg_Config *config, uint16_t startAddr);
static inline void writeData(eeprom_cfg_Config *config, uint8_t *data, uint8_t dataSize);
static inline void readData(eeprom_cfg_Config *config, uint8_t *data, uint16_t dataSize);
static inline void callbackError(uint8_t type);
/******************************************************************************
* Function Definitions
*******************************************************************************/
void eeprom_registerCallback(eeprom_cfg_Config *config, eeprom_cfg_callback callback){
	config->callback = callback;
}
void eeprom_init(eeprom_cfg_Config *config){
	if(config->callback != NULL){
		config->maxAddress = config->memorySize >> 3; /*Convert Bit to Byte*/
	}
}

void eeprom_write(eeprom_cfg_Config *config, uint16_t startAddr, uint8_t *data, uint8_t dataSize){
	if(writeValidation(config, startAddr, data, dataSize) == OK){
		writeStartAddr(config, startAddr);
		writeData(config, data, dataSize);
	}
}

void eeprom_read(eeprom_cfg_Config *config, uint16_t startAddr, uint8_t *data, uint16_t dataSize){
	if(readValidation(config, startAddr, data, dataSize) == OK){
		writeStartAddr(config, startAddr);
		readData(config, data, dataSize);
	}
}

bool eeprom_isFree(eeprom_cfg_Config *config){
	config->dataSize = 0;
	uint8_t dummyData = 0xFF;
	config->txBuffer = &dummyData;
	config->callbackType = EEPROM_CFG_CALL_CHECK_TX_STATE;
	if(config->callback(config) != OK) return false;
	else return true;
}


static inline void writeStartAddr(eeprom_cfg_Config *config, uint16_t startAddr){
	config->memAddr = startAddr;
}

static inline void writeData(eeprom_cfg_Config *config, uint8_t *data, uint8_t dataSize){
	config->dataSize = dataSize;
	config->txBuffer = data;
	config->callbackType = EEPROM_CFG_CALL_TX_START;
	if(config->callback(config) != OK) callbackError(EEPROM_CFG_CALL_TX_ERROR);
}

static inline void readData(eeprom_cfg_Config *config, uint8_t *data, uint16_t dataSize){
	config->dataSize = dataSize;
	config->rxBuffer = data;
	config->callbackType = EEPROM_CFG_CALL_RX_START;
	if(config->callback(config) != OK) callbackError(EEPROM_CFG_CALL_RX_ERROR);
}

static inline uint8_t writeValidation(eeprom_cfg_Config *config, uint16_t startAddr, uint8_t *data, uint8_t dataSize){
	if(config->callback == NULL) return NOK;
	if(data == NULL) return NOK;
	if((startAddr + dataSize) > config->maxAddress) return NOK;
	if(dataSize > config->pageBufferSize) return NOK;
	else return OK;
}
static inline uint8_t readValidation(eeprom_cfg_Config *config, uint16_t startAddr, uint8_t *data, uint16_t dataSize){
	if(config->callback == NULL) return NOK;
	if(data == NULL) return NOK;
	if((startAddr + dataSize) > config->maxAddress) return NOK;
	else return OK;
}

static inline void callbackError(uint8_t type){
	//TODO: Do something with the error
}
