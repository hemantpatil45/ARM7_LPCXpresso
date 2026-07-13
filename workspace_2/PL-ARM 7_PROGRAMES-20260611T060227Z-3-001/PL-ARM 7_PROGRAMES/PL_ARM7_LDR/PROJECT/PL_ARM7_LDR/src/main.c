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

// main.c — LDR experiment on LPC2478 (AD0.0 on P0.23) → GLCD

#include <stdint.h>
#include "ldr.h"
#include "glcd.h"
#include "SYSTEM_INIT.h"
#include "internaladc.h"
/* Map your row writer (if you prefer the short name) */
#ifndef row_wr
#define row_wr(page, str)  GLCD_RowWriteMargin((uint8_t)(page), (str))
#endif

static inline void delay_ms(uint32_t ms){
    while (ms--) { for (volatile uint32_t i=0;i<72000u;i++) __asm__("nop"); }
}

int main(void) {
    PLL_Init();
    system_Init();

    GLCD_Init();
    GLCD_Clear();

    row_wr(0x00, "WELCOME TO AKADEMIKA");
    row_wr(0x02, "      PL-ARM7       ");
    row_wr(0x04, "LDR EXPERIMENT      ");
    row_wr(0x05, "CONNECT J39-J53 &   ");
    row_wr(0x06, "J40-J69             ");

    LDR_Init();

    char line[22];
    while (1) {
        /* Build "LDR VTG = X.YYYYV" (exactly 21 chars) and show on 0x07 */
        LDR_ReadAndFormatLine_V(line, LDR_DEFAULT_VREF_MV, LDR_DEFAULT_SAMPLES);
        row_wr(0x07, line);
        delay_ms(10);
    }
}



//#include <stdint.h>
//#include "internaladc.h"
//#include "glcd.h"
//#include "SYSTEM_INIT.h"
//
///* Map your example name to the GLCD helper already in your codebase */
//#ifndef row_wr
//#define row_wr(page, str)  GLCD_RowWriteMargin((uint8_t)(page), (str))
//#endif
//
///* Vref in millivolts (adjust if your board uses a different reference) */
//#define VREF_MV  3300u
//
//static inline void delay_ms(uint32_t ms){
//    while (ms--) { for (volatile uint32_t i=0;i<72000u;i++) __asm__("nop"); }
//}
//
///* Build exactly 21 chars for one GLCD row: "LDR VTG = X.YYYYV_____" */
//static void format_ldr_vtg_line(char *dst21, uint16_t raw, uint32_t vref_mv)
//{
//    /* Convert RAW → microvolts with rounding: uV = raw * Vref(mV)*1000 / 1023 */
//    uint32_t uV   = ((uint32_t)raw * vref_mv * 1000u + 511u) / 1023u;
//    uint32_t vint = uV / 1000000u;           /* integer volts */
//    uint32_t frac = (uV % 1000000u) / 100u;  /* 4 decimal digits (0..9999) */
//
//    char *p = dst21;
//    const char *pre = "LDR VTG = ";
//    while (*pre) *p++ = *pre++;
//
//    /* write integer part */
//    char tmp[10]; int i=0;
//    if (vint == 0) { *p++='0'; }
//    else {
//        uint32_t t=vint; while(t){ tmp[i++] = (char)('0'+(t%10)); t/=10; }
//        while(i--) *p++ = tmp[i];
//    }
//
//    /* write .YYYY */
//    *p++ = '.';
//    *p++ = (char)('0' + (uint8_t)((frac/1000)%10));
//    *p++ = (char)('0' + (uint8_t)((frac/100)%10));
//    *p++ = (char)('0' + (uint8_t)((frac/10)%10));
//    *p++ = (char)('0' + (uint8_t)( frac%10));
//    *p++ = 'V';
//
//    /* pad to 21 chars */
//    while ((p - dst21) < 21) *p++ = ' ';
//    *p = '\0';
//}
//
//int main(void)
//{
//    PLL_Init();
//    system_Init();
//
//    GLCD_Init();
//    GLCD_Clear();
//
//    /* Banner */
//    row_wr(0x00, "WELCOME TO AKADEMIKA");   // 0th row
//    row_wr(0x02, "      PL-ARM7       ");   // 2nd row
//    row_wr(0x04, "LDR EXPERIMENT      ");   // 4th row
//    row_wr(0x05, "CONNECT J39-J53 &   ");   // 5th row
//    row_wr(0x06, "J40-J69             ");   // 6th row
//
//    /* ADC setup (AD0.0 on P0.23) */
//    ADC0_InitCh0();  /* from adc.h/adc.c */  /* averages use ADC0_ReadCh0_Avg() */
//
//    /* Live display on bottom row (0x07) */
//    char line[22];
//    while (1)
//    {
//        uint16_t raw = ADC0_ReadCh0_Avg(8);           // simple averaging for stability
//        format_ldr_vtg_line(line, raw, VREF_MV);      // make "LDR VTG = X.YYYYV"
//        row_wr(0x07, line);                           // print on last row
//        delay_ms(10);
//    }
//}
