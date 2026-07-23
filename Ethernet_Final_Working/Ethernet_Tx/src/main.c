#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "LPC24xx.h"
#include "glcd.h"
#include "system_init.h"
#include "ethernet.h"
#include "ethernet_tx.h"

// Defined globally to avoid Stack Overflow on constrained systems
// This array holds the complete Ethernet frame: 6 (Dest MAC) + 6 (Src MAC) + 2 (Type) = 14 bytes header
// Plus the payload buffer size
uint8_t broadcast_frame[1536] = {
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // Dest MAC: Broadcast address
    0x00, 0x11, 0x22, 0x33, 0x44, 0x55, // Src MAC: Source address
    0x88, 0xB5                          // Type: Custom protocol ID
};

// Simple blocking delay loop (approx 1 second)
void Delay1Sec(void)
{
    volatile uint32_t i;
    for(i = 0; i < 120000; i++) {
        __asm("NOP");
    }
}

// Simple blocking delay loop (approx 2 seconds)
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

    // Safely inject the custom message into the payload area
    // Index 14 is the start of the payload (immediately after the 14-byte Ethernet header)
    const char *custom_payload1 = "\r\nWEL-COME TO AKADEMIKA, THIS IS PL-ARM7 DEVELOPMENT BOARD, IN THIS PROGRAM WE ARE DEMONSTRATING WORKING OF ETHERNET PROTOCOL\n ";
    const char *custom_payload2= "\r\nPayload2 Ethernet Testing ";
    const char *custom_payload3= "\r\nPayload3 for Ethernet Testing ";



    // Initialize system hardware, peripherals, and display
    PLL_Init();
    system_Init();
    GLCD_Init();
    GLCD_Clear();
    Timer0_init();
    SW2_Keypad_Init();
    SW6_Keypad_Init();
    SW10_Keypad_Init();
    SW14_Keypad_Init();

    // Set initial text on the GLCD
    GLCD_RowWriteMargin(0,"WELCOME TO AKADEMIKA");
    GLCD_RowWriteMargin(2,"       PL-ARM7      ");
    GLCD_RowWriteMargin(4,"ETHERNET TX         ");
    GLCD_RowWriteMargin(6,"PRESS SW2 TO ETH Init");

    // Initialize Ethernet link
    link_status = Eth_Init();

    // Blocking wait until the user presses the 'Reset/Start' button (SW2)
    while(SW2_Pressed() == 0) { }

    // Clear the button instruction text
    GLCD_RowWriteMargin(6,"                    ");

    // Start a countdown before allowing transmission
    GLCD_RowWriteMargin(6,"WAIT.               ");
    Delay2Sec();
    GLCD_RowWriteMargin(6,"WAIT..              ");
    Delay2Sec();
    GLCD_RowWriteMargin(6,"WAIT...             ");
    Delay2Sec();
    GLCD_RowWriteMargin(6,"WAIT....            ");
    Delay2Sec();
    GLCD_RowWriteMargin(6,"WAIT.....           ");
    Delay2Sec();
    GLCD_RowWriteMargin(6,"WAIT......          ");
    Delay2Sec();

    GLCD_RowWriteMargin(6,"PRESS SW6 To send Tx");

    // Infinite loop to handle packet transmission
    while(1) {
        // If SW6 is held, initiate the transmission sequence
        if (SW6_Pressed()) {
            GLCD_RowWriteMargin(7, "Transmitting...     ");
        }
            // Keep transmitting continuously while the user holds the button
            while(SW6_Pressed()) {
                if (link_status) {

                	memset(&broadcast_frame[14], 0, sizeof(broadcast_frame) - 14);
                	strcpy((char *)&broadcast_frame[14], custom_payload1); // copy first payload to the frame
                    // Attempt to send the frame via EMAC
                    if (EMAC_SendPacket(broadcast_frame, sizeof(broadcast_frame))) {
                        tx_counter++;

                        // Extract a snippet of the payload to show on the GLCD
                        char display_payload[21] = {0};
                        memcpy(display_payload, &broadcast_frame[14], 20);

                        // Update GLCD with the payload being transmitted
                        GLCD_RowWriteMargin(7, display_payload);
                    }
                }
                // Control the transmission rate (1 packet per second)
                Delay1Sec();
            }
            while(SW10_Pressed()) {
                         if (link_status) {
                        	 memset(&broadcast_frame[14], 0, sizeof(broadcast_frame) - 14);
                         	strcpy((char *)&broadcast_frame[14], custom_payload2); // copy first payload to the frame
                             // Attempt to send the frame via EMAC
                             if (EMAC_SendPacket(broadcast_frame, sizeof(broadcast_frame))) {
                                 tx_counter++;

                                 // Extract a snippet of the payload to show on the GLCD
                                 char display_payload[21] = {0};
                                 memcpy(display_payload, &broadcast_frame[14], 20);

                                 // Update GLCD with the payload being transmitted
                                 GLCD_RowWriteMargin(7, display_payload);
                             }
                         }
                         // Control the transmission rate (1 packet per second)
                         Delay1Sec();
                     }

            while(SW14_Pressed()) {
                                     if (link_status) {
                                    	 memset(&broadcast_frame[14], 0, sizeof(broadcast_frame) - 14);
                                     	strcpy((char *)&broadcast_frame[14], custom_payload3); // copy first payload to the frame
                                         // Attempt to send the frame via EMAC
                                         if (EMAC_SendPacket(broadcast_frame, sizeof(broadcast_frame))) {
                                             tx_counter++;

                                             // Extract a snippet of the payload to show on the GLCD
                                             char display_payload[21] = {0};
                                             memcpy(display_payload, &broadcast_frame[14], 20);

                                             // Update GLCD with the payload being transmitted
                                             GLCD_RowWriteMargin(7, display_payload);
                                         }
                                     }
                                     // Control the transmission rate (1 packet per second)
                                     Delay1Sec();
                                 }
        }

    }

