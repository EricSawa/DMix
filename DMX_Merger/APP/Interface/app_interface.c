/*
 * app_interface.c
 *
 *  Created on: 12.01.2020
 *      Author: erics
 */
/******************************************************************************
* Includes
*******************************************************************************/
#include "app_interface.h"
/******************************************************************************
* Configuration
*******************************************************************************/
#define UART_TX_SIZE 1024
#define UART_RX_SIZE 1024
char uart4TxData[UART_TX_SIZE] = {0};
char uart4RxData[UART_RX_SIZE] = {0};

stRingBuf uart4TxBuf = {.data = uart4TxData, .size = UART_TX_SIZE, .in = 0, .out = 0};
stRingBuf uart4RxBuf = {.data = uart4RxData, .size = UART_RX_SIZE, .in = 0, .out = 0};

#define USB_TX_SIZE 1024
#define USB_RX_SIZE 1024
char usbTxData[USB_TX_SIZE] = {0};
char usbRxData[USB_RX_SIZE] = {0};
/******************************************************************************
* Variable Declarations
*******************************************************************************/
static volatile bool cyclicFlag = false;
static volatile int16_t internalTimer = 0;
/******************************************************************************
* Callback's
*******************************************************************************/

/******************************************************************************
* Function Prototypes
*******************************************************************************/
static void usbToBluetoothBridge();
/******************************************************************************
* Function Definitions
*******************************************************************************/
void app_interface_init(eal_task_Task *self){
	app_usb_init();
	app_usb_rxBufferInit(usbRxData, sizeof(usbRxData));
	app_usb_txBufferInit(usbTxData, sizeof(usbTxData));

	app_uart_bufferInit(&uart4TxBuf, uart4TxData, sizeof(uart4TxData));
	app_uart_bufferInit(&uart4RxBuf, uart4RxData, sizeof(uart4RxData));
	app_uart_enableReceiveInt(huart4.Instance);
}

void app_interface_process(eal_task_Task *self){
	if(!self->enable) return;
//	if(!cyclicFlag) return;
	usbToBluetoothBridge();
	cyclicFlag = false;
}

void app_interface_cyclic1ms(eal_task_Task *self, bool stat){
	cyclicFlag = true;
	if(internalTimer > 0)--internalTimer;
}

void app_interface_receiveMsg(eal_task_Task *self, msg_Message *message){
}

/*Helper --------------------------------------------------------------------*/
static void usbToBluetoothBridge(){
	char intData;
	if(app_usb_getCharFromRxBuffer(&intData) == ringbuffer_OK) app_uart_writeCharToBuffer(intData, &uart4TxBuf, huart4.Instance);
	if(app_uart_getCharFromBuffer(&intData, &uart4RxBuf) == ringbuffer_OK) app_usb_writeCharToBuffer(intData);
	app_usb_txLoop(); /*Can be called less often*/
}

/*IRQs ----------------------------------------------------------------------*/
void UART4_IRQHandler(void){
	app_uart_irqHandler(huart4.Instance, &uart4RxBuf, &uart4TxBuf);
}


