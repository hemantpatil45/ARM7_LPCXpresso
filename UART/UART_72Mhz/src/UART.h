#ifndef UART_H_
#define UART_H_

#include <stdint.h>
#include <stdbool.h>

// Baud rate and clock definitions
#define UART0_BAUD 115200                           //[cite: 3]
#define CCLK_FREQ  72000000                         //[cite: 3]
#define PCLK_DIV   4                                //[cite: 3]
#define Fpclk      (CCLK_FREQ / PCLK_DIV)           //[cite: 3]

// Function Prototypes
void UART0_Init(uint32_t baudrate);
void UART0_SendChar(char ch);
void UART0_SendString(const char *str);
void UART0_SendBuffer(const uint8_t *buf, uint32_t len);

// Optional receive functions for a complete module
char UART0_ReceiveChar(void);
bool UART0_DataReady(void);

#endif /* UART_H_ */
