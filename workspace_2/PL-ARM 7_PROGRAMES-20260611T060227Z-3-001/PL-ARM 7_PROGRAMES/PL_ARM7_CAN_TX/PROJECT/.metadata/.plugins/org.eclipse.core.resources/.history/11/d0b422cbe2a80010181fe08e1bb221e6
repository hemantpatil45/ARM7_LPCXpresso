#include <stdint.h>
#include "LPC24xx.h"
#include "glcd.h"
#include "system_init.h"
#include "CAN_regs.h"
#include "CAN_TX.h"

extern char CAN_DATA[8];

int main(void)
{
    PLL_Init();
    system_Init();
    GLCD_Init();
    GLCD_Clear();
    CAN_Pins_Init();

    unsigned int CAN_11bit_ID = 0x123;
    unsigned char CAN_DATA_frame[8] = {0, 1, 2, 3, 4, 5, 6, 7};
    char DATA_BUFFER[40];

    GLCD_RowWriteMargin(0,"WELCOME TO AKADEMIKA");
    GLCD_RowWriteMargin(2,"      PL-ARM7       ");
    GLCD_RowWriteMargin(4,"CAN BUS DEMO        ");
    GLCD_RowWriteMargin(6,"PRESS SW2 TO INIT   ");

    Timer0_init();
    SW2_Keypad_Init();
    SW6_Keypad_Init();
    CAN_CONTROLLER_NORMAL_INIT();
    uint8_t armed = 1;

    while (armed == 1)
    {
        if (armed && SW2_Pressed())
        {
            GLCD_Clear();
            GLCD_RowWriteMargin(0,"CAN BUS IN TX MODE  ");

            sprintf(DATA_BUFFER, "TX ID= %03X           ", CAN_11bit_ID);
            GLCD_RowWriteMargin(2, DATA_BUFFER);
            sprintf(DATA_BUFFER, "TX DATA A = %02X%02X%02X%02X",
            		CAN_DATA_frame[0], CAN_DATA_frame[1], CAN_DATA_frame[2], CAN_DATA_frame[3]);
            GLCD_RowWriteMargin(3, DATA_BUFFER);
            sprintf(DATA_BUFFER, "TX DATA B = %02X%02X%02X%02X",
            		CAN_DATA_frame[4], CAN_DATA_frame[5], CAN_DATA_frame[6], CAN_DATA_frame[7]);
            GLCD_RowWriteMargin(4, DATA_BUFFER);
            GLCD_RowWriteMargin(5,"PRESS SW6 TO SEND   ");
        }
            else if(armed && SW6_Pressed())
            {
            	CAN_CONTROLLER_NORMAL_INIT();
            CAN1_TX1_frame(CAN_11bit_ID, CAN_DATA_frame);

            CAN1CMR_STB1 = 1;
            CAN1CMR_STB2 = 1;
            CAN1CMR_STB3 = 1;

            timer_delay(10000);
            CAN1CMR_TR   = 1;

            GLCD_RowWriteMargin(5,"TX RUNNING          ");
            while ((CAN1GSR_TCS) != 1)
            {
                // wait until TCS bit becomes 1
            }
            GLCD_RowWriteMargin(5,"TX COMPLETED        ");
            }
        }
}
