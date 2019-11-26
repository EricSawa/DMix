/*
 * app_uart.c
 *
 *  Created on: 22.11.2019
 *      Author: erics
 */
#include "app_uart.h"
#include "string.h"


void uart_bufferInit(stRingBuf *buf, char *data, uint16_t size){
	buf->in = 0;
	buf->out = 0;
	buf->size = size;
	buf->data = data;
}
void uart_enableReceiveInt(USART_TypeDef *uartHandler){
	uartHandler->CR1 |= USART_CR1_RXNEIE;
}
uint8_t uart_getCharFromBuffer(char *data, stRingBuf *buf){
   uint8_t bufferState = RINGBUFFER_cOut(data, buf);
   if(bufferState == ringbuffer_NO_BUF){
	   //TODO error handling
   }
   return bufferState;
}
uint8_t uart_writeCharToBuffer(char data, stRingBuf *buf, USART_TypeDef *uartHandler){
   uint8_t bufferState = RINGBUFFER_cIn(data, buf);
   if(bufferState != ringbuffer_OK){
	   //TODO error handling
   }
   if(bufferState != ringbuffer_NO_BUF) uartHandler->CR1 |= USART_CR1_TXEIE;
   return bufferState;
}
uint8_t uart_writeStringToBuffer(const char *string, stRingBuf *buf, USART_TypeDef *uartHandler){
	uint16_t size = strlen(string);
	uint8_t bufferState = ringbuffer_OK;
	uint16_t oldIn = buf->in;
	for(uint16_t i = 0; i < size; ++i){
		bufferState = RINGBUFFER_cIn(string[i], buf);
		if(bufferState == ringbuffer_FULL) {
			buf->in = oldIn;                //load old in position
			break;
		}
		if(bufferState != ringbuffer_NO_BUF) uartHandler->CR1 |= USART_CR1_TXEIE;
	}
	return bufferState;
}


void uart_irqHandler(USART_TypeDef *uartHandler, stRingBuf *rxBuf, stRingBuf *txBuf){
		volatile uint16_t ISR;
		char bufferState;

		ISR = uartHandler->ISR;
		if(ISR & USART_ISR_RXNE){ //Bit 5 = Read Data Register Not Empty in SR
			bufferState = RINGBUFFER_cIn(uartHandler->RDR, rxBuf);
			if(bufferState != ringbuffer_OK){
				//TODO Error handling
			}
		}
		if(ISR & USART_ISR_TXE){
			uartHandler->ISR &= ~USART_ISR_TXE;
			char data;
			bufferState = RINGBUFFER_cOut(&data, txBuf);
			if(bufferState == ringbuffer_EMPTY){
				uartHandler->CR1 &= ~USART_CR1_TXEIE;
			}else if (bufferState == ringbuffer_NO_BUF){

			}else{
				uartHandler->TDR = data;
			}
		}
		if(ISR & USART_ISR_ORE){
			uartHandler->ICR |= USART_ICR_ORECF;
		}
	}

char uart_receiveCharBlocking(USART_TypeDef *uartHandler){
	while(!(uartHandler->ISR & USART_ISR_RXNE));
	return uartHandler->TDR;
}
void uart_transmitCharBlocking(char data, USART_TypeDef *uartHandler){
	while(!(uartHandler->ISR & USART_ISR_TXE));
	uartHandler->TDR = data;
}
void uart_transmitStringBlocking(char *string, USART_TypeDef *uartHandler){
	uint16_t size = strlen(string);
	for(uint16_t i = 0; i < size; ++i){
		uart_transmitCharBlocking(string[i], uartHandler);
	}
}

