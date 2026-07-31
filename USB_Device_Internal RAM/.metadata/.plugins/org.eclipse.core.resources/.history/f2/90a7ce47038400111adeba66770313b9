#ifndef ETHERNET_RX_H
#define ETHERNET_RX_H
#define KBD_ROW1_BIT    0u
#define KBD_COL1_BIT   25u
#define KBD_ROW1_MASK  (1u << KBD_ROW1_BIT)
#define KBD_COL1_MASK  (1u << KBD_COL1_BIT)

#define KBD_ROW2_BIT    1u
#define KBD_ROW2_MASK  (1u << KBD_ROW2_BIT)

void SW2_Keypad_Init(void);
#include <stdint.h>
#include <stdbool.h>

void EMAC_RxInit(void);

/* rx_buffer_capacity: size in bytes of the buffer the caller is passing in.
 * The function will never write more than this many bytes, even if the
 * received frame is larger - it truncates instead of overflowing. */
bool EMAC_ReceivePacket(uint8_t *rx_buffer, uint32_t rx_buffer_capacity, uint32_t *rx_length);

#endif /* ETHERNET_RX_H */
