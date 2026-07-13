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
#include "glcd.h"
#include "system_init.h"
#define DELAY 1000000
static inline void delay_cycles(uint32_t cycles){ while(cycles--) __asm__("nop"); }
int main(void) {
	PLL_Init();
	system_Init();
    GLCD_Init();
    GLCD_Clear();

    GLCD_RowWriteMargin(0, "     WELCOME TO     ");

    GLCD_RowWriteMargin(2, "     AKADEMIKA      ");

    GLCD_RowWriteMargin(4, "PL-ARM7 DEVELOPMENT ");

    GLCD_RowWriteMargin(6, "       BOARD        ");

    delay_cycles(DELAY);

    GLCD_RowWriteMargin(4, "CONNECT J47 TO 1-2  ");
    GLCD_RowWriteMargin(5, "FOR RELAY & 2-3 FOR ");
    GLCD_RowWriteMargin(6, "BUZZER              ");

	while(1) {
    	RELAY_BUZZER_ON;//relay connect to NO and buzzer on
    	delay_cycles(DELAY);
    	RELAY_BUZZER_OFF;//elay connect to NC and buzzer Off
    	delay_cycles(DELAY);


    }
    return 0 ;
}














