#include "ethernet_tx.h"
#include "ethernet_reg.h"
#include <string.h>
#include"system_init.h"
#include "LPC24xx.h"

/* ============================================================================== */
/* ABSOLUTE MEMORY OVERRIDE FOR LPC2478 AHB SRAM                                  */
/* ============================================================================== */

/* Hardcode base address of the LPC2478 Dedicated Ethernet RAM */
#define EMAC_AHB_RAM_BASE 0x7FE00000

/* Explicitly map pointers directly into the AHB SRAM */
static TX_Desc_t *const TX_Desc = (TX_Desc_t *) EMAC_AHB_RAM_BASE;
static TX_Stat_t *const TX_Stat = (TX_Stat_t *)(EMAC_AHB_RAM_BASE + (NUM_TX_FRAG * sizeof(TX_Desc_t)));
static uint8_t   *const TX_Buffer = (uint8_t *)(EMAC_AHB_RAM_BASE + (NUM_TX_FRAG * sizeof(TX_Desc_t)) + (NUM_TX_FRAG * sizeof(TX_Stat_t)));

/* ============================================================================== */
/* TX FUNCTIONS                                                                   */
/* ============================================================================== */

void EMAC_TxInit(void)
{
    uint32_t i;

    /* 1. Initialize the Descriptor and Status arrays directly in AHB SRAM */
    for (i = 0; i < NUM_TX_FRAG; i++)
    {
        /* Calculate absolute address for each buffer slot using flat pointer math */
        TX_Desc[i].Packet  = (uint32_t)(TX_Buffer + (i * ETH_MAX_FLEN));
        TX_Desc[i].Control = 0;
        TX_Stat[i].Info    = 0;
    }

    /* 2. Point the EMAC registers to our hardcoded AHB arrays */
    LPC_EMAC->TXDESCRIPTOR    = (uint32_t)TX_Desc;
    LPC_EMAC->TXSTATUS        = (uint32_t)TX_Stat;
    LPC_EMAC->TXDESCRIPTORNUM = NUM_TX_FRAG - 1;

    /* 3. Reset indices */
    LPC_EMAC->TXPRODUCEINDEX  = 0;
}

bool EMAC_SendPacket(uint8_t *pData, uint16_t length)
{
    uint32_t produce_idx;
    uint32_t next_idx;
    uint8_t *target_addr;

    if (length == 0 || length > ETH_MAX_FLEN) {
        return false;
    }

    produce_idx = LPC_EMAC->TXPRODUCEINDEX;
    next_idx = (produce_idx + 1) % NUM_TX_FRAG;

    if (next_idx == LPC_EMAC->TXCONSUMEINDEX) {
        return false; /* Hardware hasn't cleared the previous packets */
    }

    /* Get exact memory address for this fragment and copy the data */
    target_addr = TX_Buffer + (produce_idx * ETH_MAX_FLEN);
    memcpy(target_addr, pData, length);

    /* Set control flags (NO INTERRUPT FLAG) */
    TX_Desc[produce_idx].Control = TX_DESC_CTRL_SIZE(length - 1) |
                                   TX_DESC_CTRL_LAST |
                                   TX_DESC_CTRL_PAD |
                                   TX_DESC_CTRL_CRC;

    /* Advance Produce Index to trigger DMA */
    LPC_EMAC->TXPRODUCEINDEX = next_idx;

    return true;
}

void EMAC_TxEnable(void)
{
    LPC_EMAC->COMMAND |= CR_TX_EN;
}
void SW2_Keypad_Init(void)
{
    PINSEL4 &= ~(3u << (KBD_ROW1_BIT*2));                 /* P2.0  -> GPIO */
    PINSEL5 &= ~(3u << ((KBD_COL1_BIT-16u)*2));           /* P2.25 -> GPIO */
    FIO2DIR &= ~KBD_ROW1_MASK;    /* row input  */
    FIO2DIR |=  KBD_COL1_MASK;    /* col output */
    FIO2SET  =  KBD_COL1_MASK;
}
uint32_t SW2_Pressed(void)
{
    FIO2CLR = KBD_COL1_MASK;      /* scan: drive low */
    delay_us(200);              /* settle (not debounce) */
    uint32_t pressed = ((FIO2PIN & KBD_ROW1_MASK) == 0u);
    FIO2SET = KBD_COL1_MASK;      /* release column */
    return pressed;
}

void Timer0_irq(void) __attribute__((interrupt("IRQ")));
volatile unsigned int timer_cnt = 0;
void Timer0_irq(void)
{
	T0IR = 0x01;
	timer_cnt++;
}

void Timer0_init()
{
	T0TCR = 0; // Disable timer 0.
	T0PR = 0x0; // Prescaler is set to no division.
	T0MR0 = 6000; // Timer intervel
	T0MCR = 0x03;	// Interrupt and reset on MR0 match
	T0CCR = 0; // Capture is disabled.
	T0EMR = 0; // No external match output.
	VICVectAddr4 = (unsigned long)Timer0_irq; // Set ISR address
	VICVectCntl4 = 0x20 | 4;	//Enable slot and assign to timer 0
	VICIntEnable = (1 << 4);	//Enable timer 0 interrupt
    T0TCR=0x00;         // diable timer 0
    T0IR = 0x01;
}

void timer_delay(unsigned int time)
{
	T0IR = 0x01;
	T0TCR = 0x01;	// Enable timer0
	timer_cnt = 0x00;
	while(timer_cnt != time)
	{
		timer_cnt++;
	}
    T0TCR=0x00;         // diable timer 0
}
