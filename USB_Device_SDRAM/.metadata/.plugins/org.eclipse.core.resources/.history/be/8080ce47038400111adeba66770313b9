#include "LPC24xx.h"
#include "ethernet_rx.h"
#include "ethernet_reg.h"

// Define buffer geometry and memory locations for Ethernet DMA
// Placed in AHB SRAM to ensure hardware can access them directly
#define NUM_RX_FRAG         4
#define ETH_RX_FRAG_SIZE    1536
#define RX_DESC_BASE        0x7FE02000
#define RX_STAT_BASE        (RX_DESC_BASE + (NUM_RX_FRAG * 8))
#define RX_BUF_BASE         (RX_STAT_BASE + (NUM_RX_FRAG * 8))

// Pointers for accessing DMA descriptor and status arrays
static uint32_t *RX_DESC_PACKET_PTRS;
static uint32_t *RX_DESC_CTRL_PTRS;
static uint32_t *RX_STAT_INFO_PTRS;

// Configure EMAC Receive Descriptors and hardware pointers
void EMAC_RxInit(void)
{
    uint32_t i;
    // Point our software pointers to the physical memory locations
    RX_DESC_PACKET_PTRS = (uint32_t *)RX_DESC_BASE;
    RX_DESC_CTRL_PTRS   = (uint32_t *)(RX_DESC_BASE + 4);
    RX_STAT_INFO_PTRS   = (uint32_t *)RX_STAT_BASE;

    // Initialize each RX fragment descriptor
    for (i = 0; i < NUM_RX_FRAG; i++) {
        // Assign memory for the packet buffer
        RX_DESC_PACKET_PTRS[i * 2] = RX_BUF_BASE + (i * ETH_RX_FRAG_SIZE);
        // Set fragment size and enable the interrupt bit (bit 31)
        RX_DESC_CTRL_PTRS[i * 2]   = (ETH_RX_FRAG_SIZE - 1) | (1UL << 31);
        // Clear status info
        RX_STAT_INFO_PTRS[i * 2] = 0;
        RX_STAT_INFO_PTRS[(i * 2) + 1] = 0;
    }

    // Configure EMAC hardware registers to point to our descriptor chain
    LPC_EMAC->RXDESCRIPTOR    = RX_DESC_BASE;
    LPC_EMAC->RXSTATUS        = RX_STAT_BASE;
    LPC_EMAC->RXDESCRIPTORNUM = NUM_RX_FRAG - 1;
    LPC_EMAC->RXCONSUMEINDEX  = 0;
}

// Retrieve data from the EMAC RX DMA buffer
bool EMAC_ReceivePacket(uint8_t *rx_buffer, uint32_t rx_buffer_capacity, uint32_t *rx_length)
{   // Clear the entire receive buffer
    uint32_t consume_idx = LPC_EMAC->RXCONSUMEINDEX;
    uint32_t produce_idx = LPC_EMAC->RXPRODUCEINDEX;
    uint32_t status_info, packet_size;
    uint8_t *dma_buffer_ptr;
    uint32_t i;

    // If indices match, no new data has been received by hardware
    if (consume_idx == produce_idx) return false;

    // Check status info for hardware errors
    status_info = RX_STAT_INFO_PTRS[consume_idx * 2];
    if (status_info & RXSTAT_ERROR) {
        // Skip corrupted packet and update index
        if (++consume_idx == (LPC_EMAC->RXDESCRIPTORNUM + 1)) consume_idx = 0;
        LPC_EMAC->RXCONSUMEINDEX = consume_idx;
        *rx_length = 0;
        return false;
    }

    // Calculate packet size (status info includes CRC, so subtract 4)
    packet_size = (status_info & 0x7FF) + 1;
    if (packet_size > 4) packet_size -= 4;

    // Cap the size if it exceeds our destination buffer
    if (packet_size > rx_buffer_capacity) {
        packet_size = rx_buffer_capacity;
    }

    *rx_length = packet_size;
    dma_buffer_ptr = (uint8_t *)RX_DESC_PACKET_PTRS[consume_idx * 2];

    // Copy data from DMA buffer to application buffer
    for (i = 0; i < packet_size; i++) {
        rx_buffer[i] = dma_buffer_ptr[i];
    }

    // Increment and wrap the consumer index to inform hardware that we are done
    if (++consume_idx == (LPC_EMAC->RXDESCRIPTORNUM + 1)) consume_idx = 0;
    LPC_EMAC->RXCONSUMEINDEX = consume_idx;

    return true;
}

// Initialize GPIOs for SW2 keypad
void SW2_Keypad_Init(void)
{
    PINSEL4 &= ~(3u << (KBD_ROW1_BIT*2));             // Set Row pin to GPIO
    PINSEL5 &= ~(3u << ((KBD_COL1_BIT-16u)*2));       // Set Col pin to GPIO
    FIO2DIR &= ~KBD_ROW1_MASK;    // Configure Row as input
    FIO2DIR |=  KBD_COL1_MASK;    // Configure Col as output
    FIO2SET  =  KBD_COL1_MASK;    // Default column to high
}

// Check if SW2 is pressed
uint32_t SW2_Pressed(void)
{
    FIO2CLR = KBD_COL1_MASK;      // Drive column low to scan
    delay_us(200);                // Small delay to let line settle
    uint32_t pressed = ((FIO2PIN & KBD_ROW1_MASK) == 0u); // Check if Row is low
    FIO2SET = KBD_COL1_MASK;      // Release column (reset to high)
    return pressed;
}

// Initialize GPIOs for SW6 keypad
void SW6_Keypad_Init(void)
{
    PINSEL4 &= ~(3u << (KBD_ROW2_MASK*2));
    PINSEL5 &= ~(3u << ((KBD_COL1_BIT-16u)*2));

    FIO2DIR &= ~KBD_ROW2_MASK;    // Configure Row as input
    FIO2DIR |=  KBD_COL1_MASK;    // Configure Col as output

    FIO2SET  =  KBD_COL1_MASK;
}

// Check if SW6 is pressed
uint32_t SW6_Pressed(void)
{
    FIO2CLR = KBD_COL1_MASK;      // Drive column low
    delay_us(200);                // Settle time
    uint32_t pressed = ((FIO2PIN & KBD_ROW2_MASK) == 0u); // Check for low state
    FIO2SET = KBD_COL1_MASK;      // Reset line
    return pressed;
}

// Timer0 Interrupt Service Routine
void Timer0_irq(void) __attribute__((interrupt("IRQ")));
volatile unsigned int timer_cnt = 0;

void Timer0_irq(void)
{
    T0IR = 0x01;    // Clear the interrupt flag
    timer_cnt++;    // Increment count
}

// Setup Timer0 hardware
void Timer0_init()
{
    T0TCR = 0;                  // Disable timer
    T0PR = 0x0;                 // No prescaling
    T0MR0 = 6000;               // Set match value for interval
    T0MCR = 0x03;               // Enable interrupt and reset on match
    T0CCR = 0;                  // Disable capture
    T0EMR = 0;                  // Disable external match output

    // Setup VIC (Vectored Interrupt Controller)
    VICVectAddr4 = (unsigned long)Timer0_irq; // Point to our ISR
    VICVectCntl4 = 0x20 | 4;                  // Enable slot and map to Timer0
    VICIntEnable = (1 << 4);                  // Unmask interrupt

    T0TCR=0x00;                               // Keep disabled until needed
    T0IR = 0x01;                              // Clear pending interrupts
}

// Blocking timer-based delay
void timer_delay(unsigned int time)
{
    T0IR = 0x01;      // Clear flags
    T0TCR = 0x01;     // Enable timer
    timer_cnt = 0x00; // Reset counter
    // Wait until counter reaches target time
    while(timer_cnt != time)
    {
        timer_cnt++;
    }
    T0TCR=0x00;       // Stop timer
}
