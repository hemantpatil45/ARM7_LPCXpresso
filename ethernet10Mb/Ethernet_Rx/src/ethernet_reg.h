#ifndef ETHERNET_REG_H
#define ETHERNET_REG_H

#include <stdint.h>

typedef struct {
    volatile uint32_t MAC1;             /* 0x000 */
    volatile uint32_t MAC2;             /* 0x004 */
    volatile uint32_t IPGT;             /* 0x008 */
    volatile uint32_t IPGR;             /* 0x00C */
    volatile uint32_t CLRT;             /* 0x010 */
    volatile uint32_t MAXF;             /* 0x014 */
    volatile uint32_t SUPP;             /* 0x018 */
    volatile uint32_t TEST;             /* 0x01C */
    volatile uint32_t MCFG;             /* 0x020 */
    volatile uint32_t MCMD;             /* 0x024 */
    volatile uint32_t MADR;             /* 0x028 */
    volatile uint32_t MWTD;             /* 0x02C */
    volatile uint32_t MRDD;             /* 0x030 */
    volatile uint32_t MIND;             /* 0x034 */
    volatile uint32_t RESERVED0[2];
    volatile uint32_t SA0;              /* 0x040 */
    volatile uint32_t SA1;              /* 0x044 */
    volatile uint32_t SA2;              /* 0x048 */
    volatile uint32_t RESERVED1[45];
    volatile uint32_t COMMAND;          /* 0x100 */
    volatile uint32_t STATUS;           /* 0x104 */
    volatile uint32_t RXDESCRIPTOR;     /* 0x108 */
    volatile uint32_t RXSTATUS;         /* 0x10C */
    volatile uint32_t RXDESCRIPTORNUM;  /* 0x110 */
    volatile uint32_t RXPRODUCEINDEX;   /* 0x114 */
    volatile uint32_t RXCONSUMEINDEX;   /* 0x118 */
    volatile uint32_t TXDESCRIPTOR;     /* 0x11C */
    volatile uint32_t TXSTATUS;         /* 0x120 */
    volatile uint32_t TXDESCRIPTORNUM;  /* 0x124 */
    volatile uint32_t TXPRODUCEINDEX;   /* 0x128 */
    volatile uint32_t TXCONSUMEINDEX;   /* 0x12C */
    volatile uint32_t RESERVED2[10];
    volatile uint32_t TSV0;             /* 0x158 */
    volatile uint32_t TSV1;             /* 0x15C */
    volatile uint32_t RSV;              /* 0x160 */
    volatile uint32_t RESERVED3[3];
    volatile uint32_t FLOWCONTROLCNT;   /* 0x170 */
    volatile uint32_t FLOWCONTROLSTAT;  /* 0x174 */
    volatile uint32_t RESERVED4[34];
    volatile uint32_t RXFILTERCTRL;     /* 0x200 */
    volatile uint32_t RXFILTERWOLSTAT;  /* 0x204 */
    volatile uint32_t RXFILTERWOLCLR;   /* 0x208 */
    volatile uint32_t RESERVED5[1];
    volatile uint32_t HASHFILTERL;      /* 0x210 */
    volatile uint32_t HASHFILTERH;      /* 0x214 */
    volatile uint32_t RESERVED6[882];
    volatile uint32_t INTSTATUS;        /* 0xFE0 */
    volatile uint32_t INTENABLE;        /* 0xFE4 */
    volatile uint32_t INTCLEAR;         /* 0xFE8 */
    volatile uint32_t INTSET;           /* 0xFEC */
    volatile uint32_t INTSTEER;         /* 0xFF0 */
    volatile uint32_t POWERDOWN;        /* 0xFF4 */
} EMAC_TypeDef;

#define LPC_EMAC_BASE ((uint32_t)0xFFE00000)
#define LPC_EMAC      ((EMAC_TypeDef *)LPC_EMAC_BASE)

/* EMAC Register Bits */
#define MAC1_REC_EN         (1 << 0)
#define MAC1_PASS_ALL       (1 << 1)
#define MAC1_RES_TX         (1 << 8)
#define MAC1_RES_MCS_TX     (1 << 9)
#define MAC1_RES_RX         (1 << 10)
#define MAC1_RES_MCS_RX     (1 << 11)
#define MAC1_SIM_RES        (1 << 14)
#define MAC1_SOFT_RES       (1 << 15)

#define MAC2_FULL_DUP       (1 << 0)
#define MAC2_CRC_EN         (1 << 4)
#define MAC2_PAD_EN         (1 << 5)

#define CR_RX_EN            (1 << 0)
#define CR_TX_EN            (1 << 1)
#define CR_REG_RES          (1 << 3)
#define CR_TX_RES           (1 << 4)
#define CR_RX_RES           (1 << 5)
#define CR_PASS_RUNT_FRM    (1 << 6)
#define CR_RMII             (1 << 9)
#define CR_FULL_DUP         (1 << 10)

#define RXFILT_CTRL_BROADCAST  (1 << 1)
#define RXFILT_CTRL_PERFECT    (1 << 5)

/* PHY DP83848 Registers & Bits */
/* PHY DP83848 Registers & Bits */
#define DP83848_DEF_ADR     0x0100   /* <--- Add this line here */
#define DP83848_BMCR        0x00
#define DP83848_BMSR        0x01
#define DP83848_PHYSTS      0x10


#define PHY_BMCR_RESET      (1 << 15)
#define PHY_BMCR_AUTONEG_EN (1 << 12)
#define PHY_BMCR_RESTART_AN (1 << 9)
#define PHY_BMSR_AUTONEG_C  (1 << 5)
#define PHY_BMSR_LINK_STAT  (1 << 2)
#define PHY_STS_SPEED       (1 << 1)
#define PHY_STS_DUPLEX      (1 << 2)

/* DMA Descriptor Structures */
typedef struct {
    uint32_t Packet;
    uint32_t Control;
} TX_Desc_t;

typedef struct {
    uint32_t Info;
} TX_Stat_t;

#define TX_DESC_CTRL_SIZE(x)   ((x) & 0x7FF)
#define TX_DESC_CTRL_PAD       (1UL << 28)
#define TX_DESC_CTRL_CRC       (1UL << 29)
#define TX_DESC_CTRL_LAST      (1UL << 30)

/* DMA RX Descriptor Structures (Added) */
typedef struct {
    uint32_t Packet;
    uint32_t Control;
} RX_Desc_t;

typedef struct {
    uint32_t Info;
    uint32_t HashCRC;
} RX_Stat_t;

/* Correct Error Block */
#define RXSTAT_LAST_FLAG       (1UL << 30)
#define RXSTAT_ERROR           (1UL << 31)

#endif /* ETHERNET_REG_H */
