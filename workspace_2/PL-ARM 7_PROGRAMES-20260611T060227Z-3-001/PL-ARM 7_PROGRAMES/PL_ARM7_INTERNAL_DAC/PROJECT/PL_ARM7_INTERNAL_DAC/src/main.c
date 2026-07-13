#include <stdint.h>
#include "LPC24xx.h"
#include "glcd.h"
#include "system_init.h"
#include "INT_DAC.h"

int main(void)
{
    PLL_Init();
    system_Init();
    GLCD_Init();
    GLCD_Clear();

    GLCD_RowWriteMargin(0, "WELCOME TO AKADEMIKA");
    GLCD_RowWriteMargin(2, "      PL-ARM7       ");
    GLCD_RowWriteMargin(4, "Internal DAC demo   ");


    delay(1000000);

    GLCD_RowWriteMargin(5, "CONNECT DSO PROBE   ");
    GLCD_RowWriteMargin(6, "TO J55 & J56        ");

    dac_init();
    timer0_init_us();

    uint16_t code = 0;

    for (;;){
        timer0_wait_match0();
        dac_write(code);

        /* Increment code to produce ramp wave */
        code++;
        if (code > 1023u) {
            code = 0;
        }
    }
}
