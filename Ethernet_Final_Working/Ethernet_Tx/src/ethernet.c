#include "LPC24xx.h"
#include "ethernet.h"
#include "ethernet_reg.h"
#include "ethernet_tx.h"
//#include "ethernet_rx.h"

#define PCONP   (*((volatile uint32_t *)0xE01FC0C4))
#define PINSEL2 (*((volatile uint32_t *)0xE002C008))
#define PINSEL3 (*((volatile uint32_t *)0xE002C00C))

static void Eth_InitPins(void)
{
    PCONP |= (1 << 30);
    /* RMII Pin Configuration (0x50150105) */
    PINSEL2 = (PINSEL2 & ~0xF03F030F) | 0x50150105;
    PINSEL3 = (PINSEL3 & ~0x0000000F) | 0x00000005;
}

void Eth_WritePHY(uint32_t PhyReg, uint16_t Value)
{
    LPC_EMAC->MCMD = 0;
    LPC_EMAC->MADR = DP83848_DEF_ADR | PhyReg;
    LPC_EMAC->MWTD = Value;
    while ((LPC_EMAC->MIND & 1) != 0);
}

uint16_t Eth_ReadPHY(uint32_t PhyReg)
{
    LPC_EMAC->MADR = DP83848_DEF_ADR | PhyReg;
    LPC_EMAC->MCMD = 1;
    while ((LPC_EMAC->MIND & 1) != 0);
    LPC_EMAC->MCMD = 0;
    return (uint16_t)(LPC_EMAC->MRDD & 0xFFFF);
}

bool Eth_Init(void)
{
    uint32_t timeout;

    Eth_InitPins();

    /* 1. Reset MAC */
    LPC_EMAC->MAC1 = MAC1_RES_TX | MAC1_RES_MCS_TX | MAC1_RES_RX | MAC1_RES_MCS_RX | MAC1_SIM_RES | MAC1_SOFT_RES;
    LPC_EMAC->COMMAND = CR_REG_RES | CR_TX_RES | CR_RX_RES;
    for (timeout = 100; timeout; timeout--);

    /* 2. MAC RMII Config (STRICT FILTERING)
     * PER DATASHEET:
     * - We removed MAC1_PASS_ALL from MAC1. (Bit 1 = 0)
     * - COMMAND register does NOT have CR_PASS_RX_ERR or CR_PASS_RUNT_FRM.
     * This guarantees the MAC hardware will silently destroy the corrupted
     * packets (like those with CRC errors) before they ever reach DMA SRAM. */
    LPC_EMAC->MAC1 = MAC1_REC_EN;
    LPC_EMAC->COMMAND = CR_RMII;
    LPC_EMAC->MCFG = 0x0018;
    LPC_EMAC->MAC2 = MAC2_CRC_EN | MAC2_PAD_EN;
    LPC_EMAC->MAXF = 1536;
    LPC_EMAC->CLRT = 0x370F;
    LPC_EMAC->IPGR = 0x0C12;
    LPC_EMAC->IPGT = 0x0015;

    /* 3. PHY Soft Reset */
    Eth_WritePHY(DP83848_BMCR, PHY_BMCR_RESET);
    for (timeout = 0; timeout < 0xFFFFF; timeout++) {
        if ((Eth_ReadPHY(DP83848_BMCR) & PHY_BMCR_RESET) == 0) break;
    }


   //  0x0100 translates to:
   //  Bit 13 = 0 (10 Mbps)
   //  Bit 12 = 0 (Auto-Negotiation Disabled)
   //  Bit 8  = 1 (Full Duplex)

    Eth_WritePHY(DP83848_BMCR, 0x0100);

    /* Small delay to allow the PHY configuration to settle */
    for (timeout = 0; timeout < 100000; timeout++);

    /* =========================================================
     * 5. CONFIGURE MAC FOR 10 Mbps / FULL DUPLEX
     * ========================================================= */
    /* Hardcode MAC to match the forced PHY Full Duplex settings */
    LPC_EMAC->MAC2 |= MAC2_FULL_DUP;
    LPC_EMAC->COMMAND |= CR_FULL_DUP;
    LPC_EMAC->IPGT = 0x0015;

    /* Set MAC Speed: Bit 8 = 0 for 10Mbps */
    LPC_EMAC->SUPP &= ~(1 << 8); // Explicitly clear bit 8

    /* 6. Set MAC Address */
    LPC_EMAC->SA0 = 0x1100;
    LPC_EMAC->SA1 = 0x3322;
    LPC_EMAC->SA2 = 0x5544;

    /* 7. Init DMA Memory */
    EMAC_TxInit();
    //EMAC_RxInit();

    /* 8. Enable TX/RX and Filters */
    LPC_EMAC->RXFILTERCTRL = RXFILT_CTRL_BROADCAST | RXFILT_CTRL_PERFECT;
    LPC_EMAC->INTENABLE = 0;
    LPC_EMAC->INTCLEAR = 0xFFFF;

    LPC_EMAC->COMMAND |= (CR_RX_EN | CR_TX_EN);
    LPC_EMAC->MAC1 |= MAC1_REC_EN; // Ensure receive is enabled

    return true;
}
