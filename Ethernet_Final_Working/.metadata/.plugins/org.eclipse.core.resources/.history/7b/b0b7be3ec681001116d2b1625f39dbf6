#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "LPC24xx.h"
#include "glcd.h"
#include "system_init.h"
#include "ethernet.h"
#include "ethernet_tx.h"

/* * FIX 1 & 2: Moved to global scope to prevent Stack Overflow.
 * Setup the 14-byte Ethernet Header. The payload string will be
 * copied into this array safely during main initialization.
 */
uint8_t broadcast_frame[1536] = {
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // Dest MAC
    0x00, 0x11, 0x22, 0x33, 0x44, 0x55, // Src MAC
    0x88, 0xB5                          // Type
};

void Delay1Sec(void)
{
    volatile uint32_t i;
    for(i = 0; i < 120000; i++) {
        __asm("NOP");
    }
}

void Delay2Sec(void)
{
    volatile uint32_t i;
    for(i = 0; i < 1200000; i++) {
        __asm("NOP");
    }
}

int main(void)
{
    bool link_status;
    uint32_t tx_counter = 0;

    /* * FIX 1 (Cont.): Safely copy the large custom text payload into the
     * frame starting exactly at index 14 (right after the header).
     */
    const char *custom_payload = "WEL-COME TO AKADEMIKA, THIS IS PL-ARM7 DEVELOPMENT BOARD, IN THIS PROGRAM WE ARE DEMONSTRATING WORKING OF ETHERNET PROTOCOL";
    strcpy((char *)&broadcast_frame[14], custom_payload);

    PLL_Init();
    system_Init();
    GLCD_Init();
    GLCD_Clear();
    Timer0_init();
    SW2_Keypad_Init();

    GLCD_RowWriteMargin(0,"WELCOME TO AKADEMIKA");
    GLCD_RowWriteMargin(2,"      PL-ARM7       ");
    GLCD_RowWriteMargin(4,"ETHERNET TX         ");
    GLCD_RowWriteMargin(6,"PRESS SW2 TO RESET  ");

    link_status = Eth_Init();

    while(SW2_Pressed() == 0) {
        // Wait for user confirmation
    }
    GLCD_RowWriteMargin(6,"                    ");

    GLCD_RowWriteMargin(6,"WAIT 05             ");
    Delay2Sec();
    GLCD_RowWriteMargin(6,"WAIT 04             ");
    Delay2Sec();
    GLCD_RowWriteMargin(6,"WAIT 03             ");
    Delay2Sec();
    GLCD_RowWriteMargin(6,"WAIT 02             ");
    Delay2Sec();
    GLCD_RowWriteMargin(6,"WAIT 01             ");
    Delay2Sec();
    GLCD_RowWriteMargin(6,"WAIT 00             ");
    Delay2Sec();

    GLCD_RowWriteMargin(6,"PRESS SW6 To send Tx");

    uint8_t armed = 1;
    while(armed == 1) {
        /* Check if the button is currently being held down */
        if (SW6_Pressed()) {
            GLCD_RowWriteMargin(7, "Transmitting...     ");

            /* While the button IS held, keep sending data */
            while(SW6_Pressed()) {
                if (link_status) {
                    if (EMAC_SendPacket(broadcast_frame, sizeof(broadcast_frame))) {
                        tx_counter++;

                        /* FIX 3: Copy 20 bytes of payload so it actually displays on the GLCD */
                        char display_payload[21] = {0};
                        memcpy(display_payload, &broadcast_frame[14], 20);
                        GLCD_RowWriteMargin(7, display_payload);
                    }
                }
                /* Define the speed of continuous transmission */
                Delay1Sec();
            }
        }
    }
}
