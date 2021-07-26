/*
 * app_usb.c
 *
 *  Created on: 12.01.2020
 *      Author: erics
 */
/******************************************************************************
* Includes
*******************************************************************************/
#include "app_usb.h"
#include "string.h"
/******************************************************************************
* Configuration
*******************************************************************************/
#define USB_RX_SIZE 256
#define USB_TX_SIZE 256
static char usbRxData[USB_RX_SIZE] = {0};
static char usbTxData[USB_TX_SIZE] = {0};

#define MAX_USB_TX_SIZE	25
static stRingBuf app_usb_txBuf;
static stRingBuf app_usb_rxBuf;
static usb_app_Link linkStatus = app_usb_LINK_DOWN;
int32_t app_usb_timer = 0;
#define APP_USB_TIMEOUT 100
/******************************************************************************
* Function Prototypes
*******************************************************************************/
void setTxBufferPointer(uint16_t in, uint16_t out);
void getTxBufferPointer(uint16_t *in, uint16_t *out);
void usb_irqRxHandler(char *pData, uint16_t size);
/******************************************************************************
* Function Definitions
*******************************************************************************/
void app_usb_init(){
	usb_registerRxCallback(usb_irqRxHandler);
	linkStatus = app_usb_LINK_DOWN;
	app_usb_timer = 0;
	app_usb_rxBufferInit(usbRxData, sizeof(usbRxData));
	app_usb_rxBufferInit(usbRxData, sizeof(usbRxData));
}

void app_usb_rxBufferInit(char *data, uint16_t size){
	app_usb_rxBuf.in = 0;
	app_usb_rxBuf.out = 0;
	app_usb_rxBuf.size = size;
	app_usb_rxBuf.data = data;
}

void app_usb_txBufferInit(char *data, uint16_t size){
	app_usb_txBuf.in = 0;
	app_usb_txBuf.out = 0;
	app_usb_txBuf.size = size;
	app_usb_txBuf.data = data;
}

void app_usb_setLink(usb_app_Link link){
	linkStatus = link;
}
/*Main Loop*/
void app_usb_txLoop(){
	char usbString[MAX_USB_TX_SIZE] = {0};	//TODO: Check new line
	char usbData;
	int size = 0;
	if(HAL_GPIO_ReadPin(I_USB_VBUS_GPIO_Port, I_USB_VBUS_Pin)) linkStatus = app_usb_LINK_UP;
	else linkStatus = app_usb_LINK_DOWN;
	if(linkStatus == app_usb_LINK_UP){
		/*Grab in out pointer*/
		uint16_t lastIn, lastOut;
		getTxBufferPointer(&lastIn, &lastOut);
		/*Grab next string*/
		for(size = 0; size < MAX_USB_TX_SIZE; ++size){
			if(app_usb_getCharFromTxBuffer(&usbData) == ringbuffer_OK){
				usbString[size] = usbData;
			}else break;
		}
		if(size > 0) {
			app_usb_timer = APP_USB_TIMEOUT;
			while(app_usb_timer){
				if(CDC_Transmit_FS((uint8_t*)usbString, size+1) == USBD_OK) return;
			}
			setTxBufferPointer(lastIn, lastOut);
		}
	}
}

void app_usb_1ms(){
	if(app_usb_timer > 0) --app_usb_timer;
}
uint8_t app_usb_getCharFromRxBuffer(char *data){
   uint8_t bufferState = RINGBUFFER_cOut(data, &app_usb_rxBuf);
   if(bufferState == ringbuffer_NO_BUF);//TODO error handling
   return bufferState;
}

uint8_t app_usb_getCharFromTxBuffer(char *data){
   uint8_t bufferState = RINGBUFFER_cOut(data, &app_usb_txBuf);
   if(bufferState == ringbuffer_NO_BUF);//TODO error handling
   return bufferState;
}

uint8_t app_usb_writeCharToBuffer(char data){
   uint8_t bufferState = RINGBUFFER_cIn(data, &app_usb_txBuf);
   if(bufferState != ringbuffer_OK); 		//TODO: Error Handling
   if(bufferState != ringbuffer_NO_BUF);	//TODO: Error Handling
   return bufferState;
}
uint8_t app_usb_writeStringToBuffer(const char *string){
	uint16_t size = strlen(string);
	uint8_t bufferState = ringbuffer_OK;
	uint16_t oldIn = app_usb_txBuf.in;
	for(uint16_t i = 0; i < size; ++i){
		bufferState = RINGBUFFER_cIn(string[i], &app_usb_txBuf);
		if(bufferState == ringbuffer_FULL) {
			app_usb_txBuf.in = oldIn;                //load old in position
			break;
		}
		if(bufferState != ringbuffer_NO_BUF); //TODO: Error Handling
	}
	return bufferState;
}

/*Helper*/
void getTxBufferPointer(uint16_t *in, uint16_t *out){
	in = app_usb_txBuf.in;
	out = app_usb_txBuf.out;
}
void setTxBufferPointer(uint16_t in, uint16_t out){
	app_usb_txBuf.in = in;
	app_usb_txBuf.out = out;
}
/* IRQ Handler*/
void usb_irqRxHandler(char *pData, uint16_t size){
	char buffer_state;
	for(uint16_t x = 0; x < size; x++) buffer_state = RINGBUFFER_cIn(pData[x], &app_usb_rxBuf);
	if(buffer_state != ringbuffer_OK); //TODO: Error Handling
}



