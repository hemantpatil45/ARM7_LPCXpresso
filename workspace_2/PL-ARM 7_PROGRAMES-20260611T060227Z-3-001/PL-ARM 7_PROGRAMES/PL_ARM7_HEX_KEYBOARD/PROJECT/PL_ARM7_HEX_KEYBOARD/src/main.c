#include <stdint.h>
#include "LPC24xx.h"
#include "glcd.h"
#include "system_init.h"
#include "KEYPAD.h"
extern char keymap[4][4];

/* ================= MAIN ================= */
int main(void)
{
	PLL_Init();
    system_Init();
    GLCD_Init();
    keypad_init();
    GLCD_Clear();
    GLCD_RowWriteMargin(0, "WELCOME TO AKADEMIKA");
    GLCD_RowWriteMargin(2, "      PL-ARM7       ");
    GLCD_RowWriteMargin(4, "PRESS ANY KEY       ");

    while (1)
    {
        char key = keypad_scan_once();
        if (key)
        {
            char line[] = "KEY X PRESSED       ";
            line[4] = key;
            GLCD_RowWriteMargin(7, line);
            delay_us(5);   // debounce repeat guard ~50ms
        }
    }
    return 0;
}
