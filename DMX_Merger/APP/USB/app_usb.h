/*
 * app_usb.h
 *
 *  Created on: 12.01.2020
 *      Author: erics
 */

#ifndef USB_APP_USB_H_
#define USB_APP_USB_H_
/******************************************************************************
* Includes
*******************************************************************************/
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "main.h"
#include "ringbuffer.h"
#include "usbd_cdc_if.h"
/******************************************************************************
* Typedefs
*******************************************************************************/
typedef enum{
	app_usb_LINK_DOWN = 0,
	app_usb_LINK_UP
}usb_app_Link;
/******************************************************************************
* Function Prototypes
*******************************************************************************/
void app_usb_init();
void app_usb_setLink(usb_app_Link link);
void app_usb_txLoop();
void app_usb_1ms();
void app_usb_rxBufferInit(char *data, uint16_t size);
void app_usb_txBufferInit(char *data, uint16_t size);
uint8_t app_usb_getCharFromRxBuffer(char *data);
uint8_t app_usb_getCharFromTxBuffer(char *data);
uint8_t app_usb_writeCharToBuffer(char data);
uint8_t app_usb_writeStringToBuffer(const char *string);



#endif /* USB_APP_USB_H_ */
