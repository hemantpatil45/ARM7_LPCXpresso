#include <stdint.h>
#include "LPC24xx.h"
#include "glcd.h"
#include "glcd_font5x7.h"
#include "system_init.h"
#include "can_rx.h"

extern int key;
extern char DATA_BUFFER[8];
extern uint8_t rx_data;

int main(void) {
    PLL_Init();
    system_Init();
    GLCD_Init();
    GLCD_Clear();

    GLCD_RowWriteMargin(0x00, "WELCOME TO AKADEMIKA");
    GLCD_RowWriteMargin(0x02, "      PL-ARM7       ");
    GLCD_RowWriteMargin(0x04, "CAN BUS DEMO        ");
    Timer0_init();
    GLCD_RowWriteMargin(0x06, "PRESS SW2 TO INIT   ");

    SW2_Keypad_Init();
    SW6_Keypad_Init();
    CAN_CONTROLLER_NORMAL_INIT();
    uint8_t armed = 1;

    while (armed == 1)
    {
        if (armed && SW2_Pressed())
        {
        	GLCD_RowWriteMargin(0, "CAN BUS IN RX MODE  ");
        	CAN_CONTROLLER_NORMAL_INIT();
        	CAN1CMR |= (1 << 2);
        	GLCD_RowWriteMargin(5, "WAIT..........      ");
        	timer_delay(10000);

        	sprintf(DATA_BUFFER, "RX ID = %X                  ", CAN1RID);
        	GLCD_RowWriteMargin(5, DATA_BUFFER);

        	sprintf(DATA_BUFFER, "RX DATA A = %02X%02X%02X%02X", CAN1RDA_D1, CAN1RDA_D2, CAN1RDA_D3, CAN1RDA_D4);
        	GLCD_RowWriteMargin(6, DATA_BUFFER);

        	sprintf(DATA_BUFFER, "RX DATA B = %02X%02X%02X%02X", CAN1RDB_D5, CAN1RDB_D6, CAN1RDB_D7, CAN1RDB_D8);
        	GLCD_RowWriteMargin(7, DATA_BUFFER);
        }
    }
}
