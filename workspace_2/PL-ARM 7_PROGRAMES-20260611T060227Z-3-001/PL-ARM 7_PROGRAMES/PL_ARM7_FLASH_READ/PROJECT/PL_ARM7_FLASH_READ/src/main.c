///*
//===============================================================================
// Name        : main.c
// Author      : $(author)
// Version     :
// Copyright   : $(copyright)
// Description : main definition



#include <stdint.h>
#include "LPC24xx.h"
#include "system_init.h"
#include "glcd.h"
#include "flashread.h"    /* library: EMC + read + keypad */

static void hex4(uint16_t v, char s[5]){
    for (int i=0;i<4;i++){ uint8_t n=(v>>(12-4*i))&0xF; s[i]=(n<10)?('0'+n):('A'+(n-10)); }
    s[4]='\0';
}
static void make_line(uint16_t addr_word, uint16_t value, char out20[21])
{
    char a[5], v[5]; hex4(addr_word,a); hex4(value,v);
    int pos=0;
    out20[pos++]=a[0]; out20[pos++]=a[1]; out20[pos++]=a[2]; out20[pos++]=a[3];
    out20[pos++]=':';  out20[pos++]=v[0]; out20[pos++]=v[1]; out20[pos++]=v[2]; out20[pos++]=v[3];
    while (pos<20) out20[pos++]=' ';
    out20[20]='\0';
}

static inline void delay_loops(volatile uint32_t n){ while(n--) __asm volatile("nop"); }
static inline void delay_ms(uint32_t ms){ while (ms--) delay_loops(72000); } /* ~1ms @ ~72MHz */

int main(void)
{
    PLL_Init();
    system_Init();
    GLCD_Init();
    GLCD_Clear();


    GLCD_RowWriteMargin(0, "WELCOME TO AKADEMIKA");
    GLCD_RowWriteMargin(2, "      PL-ARM7       ");
    GLCD_RowWriteMargin(4, "FLASH READ EXPT     ");
    GLCD_RowWriteMargin(5, "PRESS SW2 TO READ   ");


    EMC_Init_CS0_x16();
    nor_reset();
    Keypad_Col1Row12_Init();

    while (!SW2_Pressed()) { /* wait */ }
    for (volatile uint32_t d=0; d<600000; ++d) __asm volatile("nop");
    while (SW2_Pressed()) {  }


    char line[21];
    for (uint32_t i=0;i<1000u;i++){ // TOTAL NUMBER CAN BE  WRITE HERE  i VALUE SET THAT TIME FLASH WRITE OTHER DATA WILL SHOW FFFF
 //IF i=200u SET C8.... IF i VALUE SET 1000u THE 3E8... IF iarflashwrite VALUE SHOW 200 OTHER SHOW FFFF
    	uint16_t val = flash_read_word(i);   /* word offset i */
        make_line((uint16_t)i, val, line);
        GLCD_RowWriteMargin(6, line);
        delay_ms(1);
    }

    GLCD_RowWriteMargin(7, "FLASH Read COMPLETED");
    while (1) { }
}

