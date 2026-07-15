/*
===============================================================================
 Name        : main.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

#include "LPC24xx.h"
#include "system_init.h"
#include "glcd.h"
#include "uart.h"

/**
 * @brief Recalculate UART0 baud rate for 18MHz Peripheral Clock (derived from 72MHz system)
 * This fixes the issue where system_Init() changes the clock, invalidating
 * the hardcoded values in uart.c.
 */
void UART_Clock_Fix(void)
{
    // Assuming PCLK is 18MHz (Standard for 72MHz CCLK / 4)
    // 9600 baud calculation: 18,000,000 / (16 * 9600) = 117.18
    // We set U0DLL = 117 (0x75), U0DLM = 0, U0FDR = 0x10

    U0LCR = 0x83;        // Set DLAB = 1 to access DLL/DLM
    U0DLL = 117;
    U0DLM = 0;
    U0FDR = 0x10;        // Standard fractional divider
    U0LCR = 0x03;        // Set DLAB = 0 to lock baud rate
}
int main(void)
{
    system_Init();
    GLCD_Init();
    GLCD_Clear();
    UART0_Init();

    // Variable to track data
    int counter = 0;

    while(1){
        /* 1. Poll UART for commands (your existing task) */
        UART0_Task();

        /* 2. Your custom TX transmission logic */
        // Example: Send a status update every loop or based on a condition
        UART0_SendFormatted("System Status: Loop Count = %d\r\n", counter);

        counter++;

        // Add a small delay if you don't want to flood the serial port
        // (Assuming you have a delay function, or use a simple loop)
        for(volatile int i=0; i<1000000; i++);
    }
}
