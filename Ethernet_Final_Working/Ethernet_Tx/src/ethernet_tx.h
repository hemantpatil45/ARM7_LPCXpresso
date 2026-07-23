#ifndef ETHERNET_TX_H
#define ETHERNET_TX_H

#include <stdint.h>
#include <stdbool.h>
#include "ethernet_reg.h"
#define KBD_ROW1_BIT    0u
#define KBD_COL1_BIT   25u
#define KBD_ROW1_MASK  (1u << KBD_ROW1_BIT)
#define KBD_COL1_MASK  (1u << KBD_COL1_BIT)
#define KBD_ROW2_BIT    1u
#define KBD_ROW2_MASK  (1u << KBD_ROW2_BIT)
#define KBD_ROW3_BIT   22u
#define KBD_ROW3_MASK (1u << KBD_ROW3_BIT)
#define KBD_ROW4_BIT  23u
#define KBD_ROW4_MASK (1u << KBD_ROW4_BIT)

void SW2_Keypad_Init(void);

#define NUM_TX_FRAG     4
#define ETH_MAX_FLEN    1536

void EMAC_TxInit(void);
bool EMAC_SendPacket(uint8_t *pData, uint16_t length);
void EMAC_TxEnable(void);

#endif /* ETHERNET_TX_H */
