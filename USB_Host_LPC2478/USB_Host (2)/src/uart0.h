#ifndef UART0_H_
#define UART0_H_

#include "LPC24xx.h"

void UART0_Init(void);
void UART0_SendChar(char c);
void UART0_Print(const char *str);
void UART0_PrintHex(unsigned int value);
void UART0_PrintDec(unsigned int value);
#endif /* UART0_H_ */
