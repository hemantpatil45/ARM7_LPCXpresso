#include "ethernet_tx.h"
#include "ethernet_reg.h"
#include <string.h>
#include "system_init.h"
#include "LPC24xx.h"

// The Ethernet peripheral uses AHB SRAM (a dedicated, high-speed memory area)
// because standard internal RAM might be too slow or inaccessible by the DMA engine.
#define EMAC_AHB_RAM_BASE 0x7FE00000

// We map our descriptors (instructions for the DMA) and buffers directly
// to these hardcoded memory addresses to ensure the hardware can "see" them.
static TX_Desc_t *const TX_Desc = (TX_Desc_t *) EMAC_AHB_RAM_BASE;
static TX_Stat_t *const TX_Stat = (TX_Stat_t *)(EMAC_AHB_RAM_BASE + (NUM_TX_FRAG * sizeof(TX_Desc_t)));
static uint8_t   *const TX_Buffer = (uint8_t *)(EMAC_AHB_RAM_BASE + (NUM_TX_FRAG * sizeof(TX_Desc_t)) + (NUM_TX_FRAG * sizeof(TX_Stat_t)));

/* ============================================================================== */
/* TX FUNCTIONS                                                                   */
/* ============================================================================== */

void EMAC_TxInit(void)
{
    uint32_t i;

    // 1. Prepare the Descriptor ring.
    // Each fragment needs a pointer to its data buffer and status.
    for (i = 0; i < NUM_TX_FRAG; i++)
    {
        // Tell the descriptor where the actual data buffer is located in memory
        TX_Desc[i].Packet  = (uint32_t)(TX_Buffer + (i * ETH_MAX_FLEN));
        // Initialize control and status to zero
        TX_Desc[i].Control = 0;
        TX_Stat[i].Info    = 0;
    }

    // 2. Link our SRAM arrays to the actual EMAC peripheral registers
    LPC_EMAC->TXDESCRIPTOR    = (uint32_t)TX_Desc;
    LPC_EMAC->TXSTATUS        = (uint32_t)TX_Stat;
    LPC_EMAC->TXDESCRIPTORNUM = NUM_TX_FRAG - 1;

    // 3. Reset the transmit logic
    LPC_EMAC->TXPRODUCEINDEX  = 0;
}

bool EMAC_SendPacket(uint8_t *pData, uint16_t length)
{
    uint32_t produce_idx;
    uint32_t next_idx;
    uint8_t *target_addr;

    // Validate input length
    if (length == 0 || length > ETH_MAX_FLEN) {
        return false;
    }

    // Check availability: If the hardware is still busy (producing meets consuming), return false
    produce_idx = LPC_EMAC->TXPRODUCEINDEX;
    next_idx = (produce_idx + 1) % NUM_TX_FRAG;

    if (next_idx == LPC_EMAC->TXCONSUMEINDEX) {
        return false; // Buffer is full; hardware hasn't finished sending previous packets
    }

    // Copy our raw data into the DMA-accessible buffer
    target_addr = TX_Buffer + (produce_idx * ETH_MAX_FLEN);
    memcpy(target_addr, pData, length);

    // Set hardware control flags for the packet:
    // - Size of the frame
    // - LAST: This is the last buffer for this packet
    // - PAD: Automatically pad short frames
    // - CRC: Calculate and append CRC automatically
    TX_Desc[produce_idx].Control = TX_DESC_CTRL_SIZE(length - 1) |
                                   TX_DESC_CTRL_LAST |
                                   TX_DESC_CTRL_PAD |
                                   TX_DESC_CTRL_CRC;

    // Advance the Produce Index. This "fires" the DMA, telling the EMAC:
    // "There is new data ready at this index, please send it."
    LPC_EMAC->TXPRODUCEINDEX = next_idx;

    return true;
}

// Enable the Transmit function in the command register
void EMAC_TxEnable(void)
{
    LPC_EMAC->COMMAND |= CR_TX_EN;
}

// Initialize GPIOs for SW2 (Row P2.0, Col P2.25)
void SW2_Keypad_Init(void)
{
    PINSEL4 &= ~(3u << (KBD_ROW1_BIT*2));             // Set Row pin to GPIO
    PINSEL5 &= ~(3u << ((KBD_COL1_BIT-16u)*2));       // Set Col pin to GPIO
    FIO2DIR &= ~KBD_ROW1_MASK;    // Set Row as input
    FIO2DIR |=  KBD_COL1_MASK;    // Set Column as output
    FIO2SET  =  KBD_COL1_MASK;    // Set Col to High (idle)
}

// Poll SW2: Drive column low, read row, then restore column
uint32_t SW2_Pressed(void)
{
    FIO2CLR = KBD_COL1_MASK;      // Drive scan column low
    delay_us(200);                // Wait for signal to settle
    uint32_t pressed = ((FIO2PIN & KBD_ROW1_MASK) == 0u); // Check if Row was pulled low
    FIO2SET = KBD_COL1_MASK;      // Return column to high
    return pressed;
}

// Initialize GPIOs for SW6
void SW6_Keypad_Init(void)
{
    PINSEL4 &= ~(3u << (KBD_ROW2_MASK*2));
    PINSEL5 &= ~(3u << ((KBD_COL1_BIT-16u)*2));

    FIO2DIR &= ~KBD_ROW2_MASK;    // Set Row as input
    FIO2DIR |=  KBD_COL1_MASK;    // Set Column as output

    FIO2SET  =  KBD_COL1_MASK;
}

// Poll SW6: Same logic as SW2 but checks a different Row bit
uint32_t SW6_Pressed(void)
{
    FIO2CLR = KBD_COL1_MASK;
    delay_us(200);
    uint32_t pressed = ((FIO2PIN & KBD_ROW2_MASK) == 0u);
    FIO2SET = KBD_COL1_MASK;
    return pressed;
}

// Timer Interrupt Service Routine
void Timer0_irq(void) __attribute__((interrupt("IRQ")));
volatile unsigned int timer_cnt = 0;

void Timer0_irq(void)
{
    T0IR = 0x01;    // Clear match interrupt flag
    timer_cnt++;    // Increment timer tick count
}

// Setup Timer 0 to trigger interrupts at a specific interval
void Timer0_init()
{
    T0TCR = 0;                  // Disable timer
    T0PR = 0x0;                 // No prescaler
    T0MR0 = 6000;               // Match value (Interval)
    T0MCR = 0x03;               // Interrupt and reset on Match 0
    T0CCR = 0;                  // No capture
    T0EMR = 0;                  // No external output

    // Configure VIC (Vectored Interrupt Controller) to link Timer0 to our ISR
    VICVectAddr4 = (unsigned long)Timer0_irq;
    VICVectCntl4 = 0x20 | 4;                  // Enable slot and map to Timer0
    VICIntEnable = (1 << 4);                  // Unmask Timer0 interrupt

    T0TCR=0x00;                               // Keep off until needed
    T0IR = 0x01;                              // Clear interrupts
}

// Simple blocking delay using the Timer
void timer_delay(unsigned int time)
{
    T0IR = 0x01;
    T0TCR = 0x01;     // Start Timer
    timer_cnt = 0x00; // Reset tick counter
    while(timer_cnt != time)
    {
        timer_cnt++;  // Wait for counter to reach target
    }
    T0TCR=0x00;       // Stop Timer
}


void SW10_Keypad_Init(void)
{
    PINSEL4 &= ~(3u << (KBD_ROW3_MASK*2));
    PINSEL5 &= ~(3u << ((KBD_COL1_BIT-16u)*2));

    FIO2DIR &= ~KBD_ROW3_MASK;    // Set Row as input
    FIO2DIR |=  KBD_COL1_MASK;    // Set Column as output

    FIO2SET  =  KBD_COL1_MASK;
}

// Poll SW10: Same logic as SW2 but checks a different Row bit
uint32_t SW10_Pressed(void)
{
    FIO2CLR = KBD_COL1_MASK;
    delay_us(200);
    uint32_t pressed = ((FIO2PIN & KBD_ROW3_MASK) == 0u);
    FIO2SET = KBD_COL1_MASK;
    return pressed;
}

void SW14_Keypad_Init(void)
{
    PINSEL4 &= ~(3u << (KBD_ROW4_MASK*2));
    PINSEL5 &= ~(3u << ((KBD_COL1_BIT-16u)*2));

    FIO2DIR &= ~KBD_ROW4_MASK;    // Set Row as input
    FIO2DIR |=  KBD_COL1_MASK;    // Set Column as output

    FIO2SET  =  KBD_COL1_MASK;
}

// Poll SW10: Same logic as SW2 but checks a different Row bit
uint32_t SW14_Pressed(void)
{
    FIO2CLR = KBD_COL1_MASK;
    delay_us(200);
    uint32_t pressed = ((FIO2PIN & KBD_ROW4_MASK) == 0u);
    FIO2SET = KBD_COL1_MASK;
    return pressed;
}
