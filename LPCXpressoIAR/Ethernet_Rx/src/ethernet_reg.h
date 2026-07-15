#ifndef ETHERNET_REG_H
#define ETHERNET_REG_H

#include <stdint.h>
#include <stdbool.h>
#include "LPC24xx.h"

/* === System & Clock Definitions === */
#define XTALFREQ_PLL  72000000
#define PCLKFREQ_PLL  18000000

#define BAUDRATEDIVISOR_9600        (PCLKFREQ_PLL/(9600*16))
#define BAUDRATEDIVISOR_9600_PLL    (PCLKFREQ_PLL/(9600*16))
#define BAUDRATEDIVISOR_115200_PLL  (PCLKFREQ_PLL/(115200*16))

/* === SDRAM Configuration === */
#define SDRAM_PERIOD          13.8
#define P2C(Period)           (((Period<SDRAM_PERIOD)?0:(int)((float)Period/SDRAM_PERIOD))+1)

#define SDRAM_REFRESH         7813
#define SDRAM_TRP             20
#define SDRAM_TRAS            45
#define SDRAM_TAPR            1
#define SDRAM_TDAL            3
#define SDRAM_TWR             3
#define SDRAM_TRC             65
#define SDRAM_TRFC            66
#define SDRAM_TXSR            67
#define SDRAM_TRRD            15
#define SDRAM_TMRD            3

#define SDRAM_BASE_ADDR 0xA0000000

/* === EMAC Configuration === */
#define NUM_RX_FRAG         4           /* Num.of RX Fragments 4*1536= 6.0kB */
#define NUM_TX_FRAG         3           /* Num.of TX Fragments 3*1536= 4.6kB */
#define ETH_FRAG_SIZE       1536        /* Packet Fragment size 1536 Bytes   */

/* DP83848C PHY Registers */
#define PHY_REG_BMCR        0x00
#define PHY_REG_BMSR        0x01
#define PHY_REG_IDR1        0x02
#define PHY_REG_IDR2        0x03
#define PHY_REG_ANAR        0x04
#define PHY_REG_ANLPAR      0x05
#define PHY_REG_ANER        0x06
#define PHY_REG_ANNPTR      0x07

/* MAC ID */
#define MAC_6    0x01
#define MAC_5    0x2B
#define MAC_4    0X1C
#define MAC_3    0X21
#define MAC_2    0X38
#define MAC_1    0XAB

/* === LCD and KBD Pin Controls (GCC Compatible) === */
#define LCD_RS_1 (FIO0SET = (1 << 4))
#define LCD_RS_0 (FIO0CLR = (1 << 4))
#define LCD_RW_1 (FIO0SET = (1 << 5))
#define LCD_RW_0 (FIO0CLR = (1 << 5))
#define LCD_ENABLE_1 (FIO0SET = (1 << 6))
#define LCD_ENABLE_0 (FIO0CLR = (1 << 6))
#define LCD_RST_1 (FIO1SET = (1 << 29))
#define LCD_RST_0 (FIO1CLR = (1 << 29))
#define LCD_CS1_1 (FIO1SET = (1 << 26))
#define LCD_CS1_0 (FIO1CLR = (1 << 26))
#define LCD_CS2_1 (FIO1SET = (1 << 28))
#define LCD_CS2_0 (FIO1CLR = (1 << 28))

#define scn0_1 (FIO2SET = (1 << 25))
#define scn0_0 (FIO2CLR = (1 << 25))
#define scn1_1 (FIO2SET = (1 << 26))
#define scn1_0 (FIO2CLR = (1 << 26))
#define scn2_1 (FIO2SET = (1 << 27))
#define scn2_0 (FIO2CLR = (1 << 27))
#define scn3_1 (FIO0SET = (1 << 11))
#define scn3_0 (FIO0CLR = (1 << 11))

/* === EMAC Memory Map Structure === */
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
#define DP83848_DEF_ADR     0x0100       /* Added: Default PHY Address */
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

/* DMA TX Descriptor Structures */
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
#define TX_DESC_CTRL_INT       (1UL << 31)

/* DMA RX Descriptor Structures */
typedef struct {
    uint32_t Packet;
    uint32_t Control;
} RX_Desc_t;

typedef struct {
    uint32_t Info;
    uint32_t HashCRC;
} RX_Stat_t;

#define RX_DESC_CTRL_SIZE(x)   ((x) & 0x7FF)
#define RX_DESC_CTRL_INT       (1UL << 31)

#define RXSTAT_CRC_ERR         (1UL << 21)
#define RXSTAT_SYM_ERR         (1UL << 22)
#define RXSTAT_LEN_ERR         (1UL << 23)
#define RXSTAT_ALN_ERR         (1UL << 25)
#define RXSTAT_OVERRUN         (1UL << 26)
#define RXSTAT_NO_DESCR        (1UL << 27)
#define RXSTAT_LAST_FLAG       (1UL << 30)
#define RXSTAT_ERROR           (1UL << 31)

/* === GCC Compatible Unions === */
union DataBus {
   uint8_t d_bus;
     struct {
        uint8_t d0 :1;
        uint8_t d1 :1;
        uint8_t d2 :1;
        uint8_t d3 :1;
        uint8_t d4 :1;
        uint8_t d5 :1;
        uint8_t d6 :1;
        uint8_t d7 :1;
     } d_bus_bits;
};

union RetBus {
   uint8_t bus;
     struct {
        uint8_t i0 :1;
        uint8_t i1 :1;
        uint8_t i2 :1;
        uint8_t i3 :1;
     } ret_bus_bits;
};

/* === Global Variable Declarations === */
extern union DataBus data;
extern union RetBus ret_bus;
extern unsigned char *Flash_Pointer1;
extern unsigned int *SDRAM_pointer;
extern unsigned char *p;
extern unsigned int *ADC_DATA_RECIEVED;
extern unsigned char buffer[50];
extern unsigned int *Descriptor_pt;
extern unsigned int RX_BUF[4];
extern unsigned int TX_BUF[4];
extern unsigned int smpl_tmr;
extern float *adc_val;

/* === Function Prototypes === */
void Eth_WritePHY(uint32_t PhyReg, uint16_t Value);
uint16_t Eth_ReadPHY(uint32_t PhyReg);
bool Eth_Init(void);
void Uart_0_init_9600_WITHOUT_PLL(void);
void Uart_0_init_9600_WITH_PLL(void);
void Uart_0_init_115200_WITH_PLL(void);
void Init_timer0(void);
void sys_init_without_TFT_32bit_SDRAM(void);
void SDRAM_init_32BIT(void);

#endif /* ETHERNET_REG_H */
