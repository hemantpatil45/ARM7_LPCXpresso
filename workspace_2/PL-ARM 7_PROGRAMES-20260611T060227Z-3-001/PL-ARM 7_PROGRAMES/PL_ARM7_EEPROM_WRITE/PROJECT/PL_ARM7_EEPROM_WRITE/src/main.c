#include "LPC24xx.h"
#include "system_init.h"
#include "glcd.h"
#include "i2cwrite.h"

static inline void delay_cycles(uint32_t n){ while(n--) __asm__("nop"); }

int main(void) {
    PLL_Init();
    system_Init();

    GLCD_Init();
    GLCD_Clear();
    GLCD_RowWriteMargin(0, "WELCOME TO AKADEMIKA");
    GLCD_RowWriteMargin(2, "      PL-ARM7       ");
    GLCD_RowWriteMargin(4, "EEPROM WRITE EXPT   ");
    GLCD_RowWriteMargin(5,"PRESS SW2 TO WRITE  ");
    I2C0_Init_100k();
    SW2_Keypad_Init();

    static uint8_t tx[256];
    for (uint16_t i=0;i<256;i++) tx[i]=(uint8_t)i;

    uint8_t armed = 1;
    while(1){
        if(armed && SW2_Pressed()){
            armed = 0;

            AT24C512_WriteBuffer(0x0000u, tx, sizeof(tx));
            GLCD_RowWriteMargin(6,"DATA WRITE COMPLETED");
        } else if (!SW2_Pressed() && !armed) {
            armed = 1;

        }
    }
    return 0;
}
