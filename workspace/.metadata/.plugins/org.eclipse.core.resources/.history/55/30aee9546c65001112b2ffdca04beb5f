#include <stdint.h>
#include "internaladc.h"
#include "glcd.h"
#include "SYSTEM_INIT.h"

static inline void delay_ms(uint32_t ms){
    while (ms--) { for (volatile uint32_t i=0;i<72000u;i++) __asm__("nop"); }
}

int main(void) {
    PLL_Init();
    system_Init();

    GLCD_Init();
    GLCD_Clear();

    /* Banners (centered) */
    GLCD_RowWriteMargin(0, "WELCOME TO AKADEMIKA");
    GLCD_RowWriteMargin(2, "      PL-ARM7       ");
    GLCD_RowWriteMargin(4, "Internal ADC demo   ");
    GLCD_RowWriteMargin(5, "CONNECT J43-J53 &   ");
    GLCD_RowWriteMargin(6, "J44-J69 & VARY R50  ");

    /* ADC setup (AD0.0 on P0.23) */
    ADC0_InitCh0();

    /* Bottom row template (exactly 21 chars wide) */
    GLCD_RowWriteMargin(7, "ADC=0000  V=0.0000V ");

    char line[22];  /* 21 chars + NUL */

    while (1)
    {
        uint16_t raw = ADC0_ReadCh0_Avg(8);        /* average for stability */
        ADC_FormatLine_ADC_V(line, raw, 3300u);    /* "ADC=####  V=X.YYYYV " */
        GLCD_RowWriteMargin(7, line);              /* write full line (no XY) */
        delay_ms(10);
    }
}
