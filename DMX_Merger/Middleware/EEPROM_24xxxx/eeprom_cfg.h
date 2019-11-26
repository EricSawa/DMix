/*
 * eeprom_cfg.h
 *
 *  Created on: 20.07.2018
 *      Author: sawadee
 */

#ifndef EEPROM_24XXXX_EEPROM_CFG_H_
#define EEPROM_24XXXX_EEPROM_CFG_H_

/******************************************************************************
* Includes
*******************************************************************************/
#include <stdint.h>
#include <string.h>
/******************************************************************************
* Typedefs
*******************************************************************************/
typedef enum{
	EEPROM_CFG_MEMSIZE_1KBIT = 1024,
	EEPROM_CFG_MEMSIZE_2KBIT = 2048,
	EEPROM_CFG_MEMSIZE_64KBIT = 65536,
	EEPROM_CFG_MEMSIZE_256KBIT = 262144,
	EEPROM_CFG_MEMSIZE_512KBIT = 524288,
	EEPROM_CFG_MEMSIZE_1MBIT = 1048576,
	EEPROM_CFG_MEMSIZE_MAX
}eeprom_cfg_MemSize;

typedef enum{
	EEPROM_CFG_PAGESIZE_32BYTE = 32,
	EEPROM_CFG_PAGESIZE_256BYTE = 256,
	EEPROM_CFG_SIZE_MAX
}eeprom_cfg_PageSize;

typedef enum{
	EEPROM_CFG_CALL_TX_START,
	EEPROM_CFG_CALL_RX_START,
	EEPROM_CFG_CALL_TX_ERROR,
	EEPROM_CFG_CALL_RX_ERROR,
	EEPROM_CFG_CALL_CHECK_TX_STATE,
	EEPROM_CFG_CALL_MAX
}eeprom_cfg_CallbackType;

/******************************************************************************
* Configuration
*******************************************************************************/
typedef struct EEPROM_CFG_CONFIG{
	uint8_t devAddr;
	eeprom_cfg_MemSize memorySize;
	eeprom_cfg_PageSize pageBufferSize;
	eeprom_cfg_CallbackType callbackType;
	uint8_t (*callback)(struct EEPROM_CFG_CONFIG *p);
	uint32_t maxAddress;
	uint32_t dataSize;
	uint32_t memAddr;
	uint8_t *txBuffer;
	uint8_t *rxBuffer;
}eeprom_cfg_Config;

typedef uint8_t (*eeprom_cfg_callback)(eeprom_cfg_Config *p);




#endif /* EEPROM_24XXXX_EEPROM_CFG_H_ */
