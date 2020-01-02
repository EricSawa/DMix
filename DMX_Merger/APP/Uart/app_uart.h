/*
 * app_uart.h
 *
 *  Created on: 22.11.2019
 *      Author: erics
 */

#ifndef UART_APP_UART_H_
#define UART_APP_UART_H_

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "main.h"
#include "ringbuffer.h"

/* Public functions (prototypes) ---------------------------------------------------*/
void app_uart_bufferInit(stRingBuf *buf, char *data, uint16_t size);
void app_uart_enableReceiveInt(USART_TypeDef *uartHandler);
uint8_t app_uart_getCharFromBuffer(char *data, stRingBuf *buf);
uint8_t app_uart_writeCharToBuffer(char data, stRingBuf *buf, USART_TypeDef *uartHandler);
uint8_t app_uart_writeStringToBuffer(const char *string, stRingBuf *buf, USART_TypeDef *uartHandler);
void app_uart_irqHandler(USART_TypeDef *uartHandler, stRingBuf *rxBuf, stRingBuf *txBuf);

#endif /* UART_APP_UART_H_ */
