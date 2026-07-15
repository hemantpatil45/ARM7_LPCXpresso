#ifndef ETHERNET_H
#define ETHERNET_H

#include <stdint.h>
#include <stdbool.h>

extern unsigned int RX_BUF_1[4];
extern unsigned int TX_BUF[4];

void write_PHY(unsigned char PhyRegW, unsigned short ValueW);
unsigned short read_PHY(unsigned char PhyRegR);
void rx_descr_init(void);
void tx_descr_init(void);
void EMAC_init(void);

#endif /* ETHERNET_H */
