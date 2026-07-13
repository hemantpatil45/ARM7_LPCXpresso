#ifndef ETHERNET_H
#define ETHERNET_H

#include <stdint.h>
#include <stdbool.h>

void Eth_WritePHY(uint32_t PhyReg, uint16_t Value);
uint16_t Eth_ReadPHY(uint32_t PhyReg);
bool Eth_Init(void);

#endif /* ETHERNET_H */
