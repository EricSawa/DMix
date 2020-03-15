/*
 * app_eeprom.c
 *
 *  Created on: 15.03.2020
 *      Author: erics
 */
/******************************************************************************
* Includes
*******************************************************************************/
#include "app_eeprom.h"
/******************************************************************************
* Configuration
*******************************************************************************/

/******************************************************************************
* Variable Declarations
*******************************************************************************/
static volatile bool cyclicFlag = false;
static volatile int16_t internalTimer = 0;

static int16_t secondTimer = 0;
/******************************************************************************
* Callback's
*******************************************************************************/

/******************************************************************************
* Function Prototypes
*******************************************************************************/
uint8_t eepromCallback(eeprom_cfg_Config *config);
static bool writeUntilLastEEPROMWriteFinished();
static void writeVersion(uint32_t version);
static uint32_t readVersion();
static void erase();

static void storeModel();
static void readModel();
static void storePreset(uint16_t pageAddress, app_dmx_preset_cfg_Preset *preset);
static void readPreset(uint16_t pageAddress, app_dmx_preset_cfg_Preset *preset);
static void storeFootswitch(uint16_t pageAddress, app_footcontrol_cfg_GpioCfg *footswitch);
static void readFootswitch(uint16_t pageAddress, app_footcontrol_cfg_GpioCfg *footswitch);
/******************************************************************************
* Function Definitions
*******************************************************************************/
void app_eeprom_init(eal_task_Task *self){
	eeprom_registerCallback(&eepromDev1, eepromCallback);
	eeprom_init(&eepromDev1);
	secondTimer = 0;
	if(readVersion() != EE_FW_VERSION){
		erase();
		writeVersion(EE_FW_VERSION);
		storeModel();
	}
	readModel();
}

void app_eeprom_process(eal_task_Task *self){
	if(!self->enable) return;
	if(!cyclicFlag) return;
	if(++secondTimer == 1000){
		secondTimer = 0;
		if(app_model_model.dirty) storeModel();
	}

	cyclicFlag = false;
}

void app_eeprom_cyclic1ms(eal_task_Task *self, bool stat){
	cyclicFlag = true;
	if(internalTimer > 0)--internalTimer;
}

void app_eeprom_receiveMsg(eal_task_Task *self, msg_Message *message){
}
/* Store/Read ----------------------------------------------------------------------*/
static void writeVersion(uint32_t version){
	uint8_t versionArray[4] = {0};
	versionArray[0] = ((version >> 24) & 0xFF);
	versionArray[1] = ((version >> 16) & 0xFF);
	versionArray[2] = ((version >> 8) & 0xFF);
	versionArray[3] = ((version >> 0) & 0xFF);
	if(!writeUntilLastEEPROMWriteFinished()) return;
	eeprom_write(&eepromDev1, 0, versionArray, 4);
}

static uint32_t readVersion(){
	uint8_t versionArray[4] = {0};
	if(!writeUntilLastEEPROMWriteFinished()) return 0;
	eeprom_read(&eepromDev1, 0, versionArray, 4);
	uint32_t version = (versionArray[0] << 24)|(versionArray[1] << 16)|(versionArray[2] << 8)|(versionArray[3] << 0);
	return version;
}

static void erase(){
	app_eeprom_Page newPage;
	uint8_t eraseTxData = 0x00;
	for(int i = 0; i < app_eeprom_PAGE_SIZE; ++i) newPage.data[i] = eraseTxData;
	for(int i = 0; i < app_eeprom_PAGE_NMBR; ++i){
		if(!writeUntilLastEEPROMWriteFinished())return;
		newPage.addr = i*app_eeprom_PAGE_SIZE;
		eeprom_write(&eepromDev1, newPage.addr, newPage.data, app_eeprom_PAGE_SIZE);
	}
}

static void storeModel(){
	/*Store presets*/
	uint16_t pageAddress = app_eeprom_PRESET_START_PAGE*app_eeprom_PAGE_SIZE;
	for(int i = 0; i < app_cfg_NMBR_PRESETS; ++i){
		if(!writeUntilLastEEPROMWriteFinished()) return;
		storePreset(pageAddress, &app_model_model.presets[i]);
		pageAddress += (app_eeprom_PAGES_PER_PRESET*app_eeprom_PAGE_SIZE);
	}
	/*Store footswitch*/
	pageAddress = app_eeprom_FOOTSWITCH_START_PAGE*app_eeprom_PAGE_SIZE;
	if(!writeUntilLastEEPROMWriteFinished()) return;
	storeFootswitch(pageAddress, &app_model_model.footswitches[0]);
	app_model_model.dirty = false;
}

static void readModel(){
	/*Read presets*/
	uint16_t pageAddress = app_eeprom_PRESET_START_PAGE*app_eeprom_PAGE_SIZE;
	for(int i = 0; i < app_cfg_NMBR_PRESETS; ++i){
		if(!writeUntilLastEEPROMWriteFinished()) return;
		readPreset(pageAddress, &app_model_model.presets[i]);
		pageAddress += (app_eeprom_PAGES_PER_PRESET*app_eeprom_PAGE_SIZE);
	}
	/*Read footswitch*/
	pageAddress = app_eeprom_FOOTSWITCH_START_PAGE*app_eeprom_PAGE_SIZE;
	if(!writeUntilLastEEPROMWriteFinished()) return;
	readFootswitch(pageAddress, &app_model_model.footswitches[0]);
}

static void storePreset(uint16_t pageAddress, app_dmx_preset_cfg_Preset *preset){
	/*Move struct into byte array*/
	uint8_t storage[app_dmx_preset_cfg_PRESET_SIZE] = {0};
	memcpy(storage, preset, app_dmx_preset_cfg_PRESET_SIZE);
	/*Generate pages to store*/
	app_eeprom_Page newPage[2];
	memset(newPage[0].data, 0, app_eeprom_PAGE_SIZE); /*Clean data*/
	memset(newPage[1].data, 0, app_eeprom_PAGE_SIZE); /*Clean data*/
	newPage[0].addr = pageAddress;
	newPage[1].addr = pageAddress + app_eeprom_PAGE_SIZE;
	memcpy(newPage[0].data, &storage[0], app_eeprom_PAGE_SIZE);
	memcpy(newPage[1].data, &storage[app_eeprom_PAGE_SIZE], (app_dmx_preset_cfg_PRESET_SIZE - app_eeprom_PAGE_SIZE));
	eeprom_write(&eepromDev1, newPage[0].addr, newPage[0].data, app_eeprom_PAGE_SIZE);
	eeprom_write(&eepromDev1, newPage[1].addr, newPage[1].data, app_eeprom_PAGE_SIZE);
}

static void readPreset(uint16_t pageAddress, app_dmx_preset_cfg_Preset *preset){
	/*Generate pages to store*/
	app_eeprom_Page newPage[2];
	memset(newPage[0].data, 0, app_eeprom_PAGE_SIZE); /*Clean data*/
	memset(newPage[1].data, 0, app_eeprom_PAGE_SIZE); /*Clean data*/
	newPage[0].addr = pageAddress;
	newPage[1].addr = pageAddress + app_eeprom_PAGE_SIZE;
	eeprom_read(&eepromDev1, newPage[0].addr, newPage[0].data, app_eeprom_PAGE_SIZE);
	eeprom_read(&eepromDev1, newPage[1].addr, newPage[1].data, app_eeprom_PAGE_SIZE);
	uint8_t storage[app_dmx_preset_cfg_PRESET_SIZE] = {0};
	memcpy(&storage[0], newPage[0].data, app_eeprom_PAGE_SIZE);
	memcpy(&storage[app_eeprom_PAGE_SIZE], newPage[1].data, (app_dmx_preset_cfg_PRESET_SIZE - app_eeprom_PAGE_SIZE));
	/*Move byte array into struct*/
	memcpy(preset, storage, app_dmx_preset_cfg_PRESET_SIZE);
}

static void storeFootswitch(uint16_t pageAddress, app_footcontrol_cfg_GpioCfg *footswitch){
	/*Move struct into byte array*/
	uint8_t storage[app_dmx_footswitch_cfg_FOOTSWITCH_SIZE*app_cfg_NMBR_BTNS] = {0};
	memcpy(storage, footswitch, sizeof(storage));
	/*Generate pages to store*/
	app_eeprom_Page newPage;
	memset(newPage.data, 0, app_eeprom_PAGE_SIZE); /*Clean data*/
	newPage.addr = pageAddress;
	memcpy(newPage.data, &storage[0], sizeof(storage));
	eeprom_write(&eepromDev1, newPage.addr, newPage.data, app_eeprom_PAGE_SIZE);
}
static void readFootswitch(uint16_t pageAddress, app_footcontrol_cfg_GpioCfg *footswitch){
	/*Generate pages to store*/
	app_eeprom_Page newPage;
	memset(newPage.data, 0, app_eeprom_PAGE_SIZE); /*Clean data*/
	newPage.addr = pageAddress;
	eeprom_read(&eepromDev1, newPage.addr, newPage.data, app_eeprom_PAGE_SIZE);
	uint8_t storage[app_dmx_footswitch_cfg_FOOTSWITCH_SIZE*app_cfg_NMBR_BTNS] = {0};
	memcpy(&storage, newPage.data, sizeof(storage));
	/*Move byte array into struct*/
	memcpy(footswitch, storage, sizeof(storage));
}
/* Helper ----------------------------------------------------------------------*/
static bool writeUntilLastEEPROMWriteFinished(){
	internalTimer = 1000;
	while(!eeprom_isFree(&eepromDev1)){
		if(internalTimer <= 0){
			return false;
		}
	}
	internalTimer = 0;
	return true;
}
/* Callback's ------------------------------------------------------------------*/
uint8_t eepromCallback(eeprom_cfg_Config *config){
	uint8_t state = 1;
	if(config->callbackType == EEPROM_CFG_CALL_TX_START){
		if(HAL_I2C_Mem_Write(&hi2c2, config->devAddr, config->memAddr, I2C_MEMADD_SIZE_16BIT, config->txBuffer, config->dataSize, 100) != HAL_OK) state = 0;
	}
	if(config->callbackType == EEPROM_CFG_CALL_RX_START){
		if(HAL_I2C_Mem_Read(&hi2c2,  config->devAddr, config->memAddr, I2C_MEMADD_SIZE_16BIT, config->rxBuffer, config->dataSize, 100) != HAL_OK) state = 0;
	}
	if(config->callbackType == EEPROM_CFG_CALL_CHECK_TX_STATE){
		if(HAL_I2C_Master_Transmit(&hi2c2, config->devAddr, config->rxBuffer, config->dataSize, 100) != HAL_OK) state = 0;
	}
	return state;
}

