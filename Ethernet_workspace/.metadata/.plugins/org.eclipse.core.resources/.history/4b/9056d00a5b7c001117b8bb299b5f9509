/*
===============================================================================
 Name        : main.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

// TODO: insert include files here

// TODO: insert other definitions and declarations here

#include "LPC24xx.h"
#include "system_init.h"
#include "glcd.h"
#include "uart.h"

int main(void)
{
//    PLL_Init();
    system_Init();

    GLCD_Init();
    GLCD_Clear();

    UART0_Init();        /* shows help on GLCD and UART banner */

    while(1){
        UART0_Task();    /* poll & handle UART commands + GLCD updates */
    }
}
