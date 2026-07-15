#ifndef ETHERNET_RX_H
#define ETHERNET_RX_H

#include <stdint.h>
#include <stdbool.h>

void EMAC_RxInit(void);

/* rx_buffer_capacity: size in bytes of the buffer the caller is passing in.
 * The function will never write more than this many bytes, even if the
 * received frame is larger - it truncates instead of overflowing. */
bool EMAC_ReceivePacket(uint8_t *rx_buffer, uint32_t rx_buffer_capacity, uint32_t *rx_length);

#endif /* ETHERNET_RX_H */
