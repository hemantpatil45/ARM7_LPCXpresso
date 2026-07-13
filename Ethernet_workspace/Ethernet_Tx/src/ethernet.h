#ifndef ETHERNET_H
#define ETHERNET_H

#include <stdint.h>
#include <stdbool.h>

#define DP83848_DEF_ADR     0x0100

bool Eth_Init(void);
void Eth_WritePHY(uint32_t PhyReg, uint16_t Value);
uint16_t Eth_ReadPHY(uint32_t PhyReg);

#endif /* ETHERNET_H */
