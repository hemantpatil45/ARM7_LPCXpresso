/*
 * uart.h
 *
 *  Created on: 10-Oct-2025
 *      Author: ADMIN
 */

#ifndef UART_H_
#define UART_H_

#include <stdint.h>

void UART0_Init(void);

void UART0_SendChar(char c);
void UART0_SendString(const char *s);

uint8_t UART0_ReadAvailable(void);
char    UART0_ReadChar(void);

void UART0_Task(void);

void UART0_ShowHelpOnGLCD(void);

#endif /* UART_H_ */
