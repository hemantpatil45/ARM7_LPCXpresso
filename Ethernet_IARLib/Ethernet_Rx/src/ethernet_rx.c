#include "ethernet_rx.h"
#include "ethernet.h"
#include "system_init.h"

unsigned char *RXDESBUFFER_1;

void transmit_ethernet_payload(void)
{
    int i;

    RXDESBUFFER_1 = (unsigned char*)(RX_BUF_1[0]);

    /* Move the pointer past the 14-byte Ethernet header */
    for(i = 0; i < 14; i++) {
        RXDESBUFFER_1++;
    }

    transmit_byte(0x0D); // Carriage Return
    transmit_byte(0x0A); // Line Feed

    /* Transmit exactly 58 bytes of parsed payload via UART0 */
    for(i = 0; i <= 57; i++) {
        transmit_byte(*RXDESBUFFER_1);
        RXDESBUFFER_1++;
        delay_us(1000);
    }
}
