#include "SDRAM.h"
#include <stdio.h>
#include <stdint.h>
#include "system_init.h"
#include "LPC24xx.h"

#define UART0_BAUD 115200
#define HCLK_MHZ   72  // Set system and peripheral clock to 72 MHz

// Define the global variables
unsigned int *SDRAM_pointer;
unsigned int test1234[256];
char buffer[64];

// --- UART0 Functions for 72 MHz ---
void UART0_Init(uint32_t baudrate) {
    uint32_t Fdiv;

    PCONP |= (1 << 3);          // Enable power for UART0

    // Set peripheral clock (PCLK_UART0) to equal CCLK (72 MHz)
    PCLKSEL0 &= ~(3 << 6);
    PCLKSEL0 |=  (1 << 6);

    // Configure P0.2 as TXD0 and P0.3 as RXD0
    PINSEL0 &= ~0x000000F0;
    PINSEL0 |=  0x00000050;

    U0LCR = 0x83;               // 8 bits, no Parity, 1 Stop bit, DLAB = 1

    // Calculate divisor for 72 MHz PCLK:
    // Fdiv = 72000000 / (16 * 115200) = 39.0625 -> DLL = 39, DLM = 0
    Fdiv = (72000000 / 16) / baudrate;
    U0DLM = (Fdiv >> 8) & 0xFF;
    U0DLL = Fdiv & 0xFF;

    U0FDR = 0x10;               // Fractional divider: MULVAL = 1, DIVADDVAL = 0
    U0LCR = 0x03;               // DLAB = 0 (Lock baud rate registers)
    U0FCR = 0x07;               // Enable and clear TX/RX FIFOs
}

void UART0_SendChar(char ch) {
    while (!(U0LSR & 0x20));
    U0THR = ch;
}

void UART0_Print(const char *str) {
    while (*str) {
        UART0_SendChar(*str++);
    }
}

void UART0_PrintHex(uint32_t val) {
    char hex[] = "0123456789ABCDEF";
    UART0_Print("0x");
    for (int i = 28; i >= 0; i -= 4) {
        UART0_SendChar(hex[(val >> i) & 0xF]);
    }
}

void delay_ms(uint32_t ms) {
    for (volatile uint32_t i = 0; i < (ms * (72000 / 4)); i++); // Approximate delay for 72MHz
}

// --- Wire up weak stubs to active UART0 implementation ---
void transmit_byte(char byte) {
    UART0_SendChar(byte);
}

void transmit_array(char* str) {
    UART0_Print(str);
}

// Weak stub for GLCD row write (keep if GLCD is optional/unused)
__attribute__((weak)) void row_wr(unsigned int row, char* str) {
    // Implement or leave blank if using only serial terminal feedback
}

// --- SDRAM Initialization (72 MHz Timings) ---
void SDRAM_init_32BIT()
{
    UART0_Print("\r\n[INFO] Initializing SDRAM at 72 MHz...\r\n");

    PCONP |= (1 << 21);          // Enable EMC power (PCEMC is bit 21)
    EMC_CTRL = 1;                // Enable EMC
    EMC_DYN_RD_CFG = 1;          // Dynamic memory read configuration

    // CAS = 3, RAS = 3
    EMC_DYN_RASCAS0 = 0x0000000F;

    EMC_DYN_RP = P2C(SDRAM_TRP);
    EMC_DYN_RAS = P2C(SDRAM_TRAS);
    EMC_DYN_SREX = P2C(SDRAM_TXSR);
    EMC_DYN_APR = SDRAM_TAPR;
    EMC_DYN_DAL = SDRAM_TDAL + P2C(SDRAM_TRP);
    EMC_DYN_WR = SDRAM_TWR;
    EMC_DYN_RC = P2C(SDRAM_TRC);
    EMC_DYN_RFC = P2C(SDRAM_TRFC);
    EMC_DYN_XSR = P2C(SDRAM_TXSR);
    EMC_DYN_RRD = P2C(SDRAM_TRRD);
    EMC_DYN_MRD = SDRAM_TMRD;
    EMC_DYN_CFG0 = 0x00004680;   // 13 row, 9 - col, SDRAM

    // JEDEC General SDRAM Initialization Sequence
    EMC_DYN_CTRL = 0x0183;

    for(volatile int i = 200 * 30; i; i--);

    // PALL (Precharge All) command -> I = 2
    EMC_DYN_CTRL = (EMC_DYN_CTRL & ~(3 << 7)) | (2 << 7);
    EMC_DYN_RFSH = 1;

    for(volatile int i = 128; i; --i); // > 128 clk
    EMC_DYN_RFSH = P2C(SDRAM_REFRESH) >> 4;

    // COMM (Mode register set) command -> I = 1
    EMC_DYN_CTRL = (EMC_DYN_CTRL & ~(3 << 7)) | (1 << 7);

    // Burst 4, Sequential, CAS-3
    volatile unsigned long Dummy = *(SDRAM_pointer + 0x32);
    (void)Dummy; // Suppress unused variable warning

    // NORM (Normal mode) -> I = 0
    EMC_DYN_CTRL = 0x0000;
    EMC_DYN_CFG0 |= (1 << 19);   // Enable buffer

    for(volatile int i = 10000; i; i--);

    UART0_Print("[INFO] SDRAM Initialization Complete.\r\n");
}

void SDRAM_write(unsigned int No_of_data, unsigned int SDRAM_DAT[])
{
    unsigned int a;
    UART0_Print("[INFO] Starting SDRAM Write Operation...\r\n");
    SDRAM_pointer = (unsigned int*)0xA0000000;

    for(a = 0; a < No_of_data; a++)
    {
        *SDRAM_pointer = SDRAM_DAT[a];
        while((EMC_STAT & 0x01) == 0x01) // Check if EMC is busy
        {
        }
        SDRAM_pointer++;
    }
    UART0_Print("[INFO] SDRAM Write Finished Successfully.\r\n");
}

void SDRAM_read(unsigned int No_of_data)
{
    int a, b;
    UART0_Print("[INFO] Starting SDRAM Read and Verification...\r\n");
    SDRAM_pointer = (unsigned int*)0xA0000000;

    for(a = 0; a < No_of_data; a++)
    {
        b = *SDRAM_pointer;
        while((EMC_STAT & 0x01) == 0x01) // Check if EMC is busy
        {
        }
        test1234[a] = b;
        SDRAM_pointer++;
    }

    SDRAM_pointer = (unsigned int*)0xA0000000;
    for(a = 0; a < No_of_data; a++)
    {
        sprintf(buffer, "Addr: 0x%x = Data: 0x%x\r\n", (unsigned int)SDRAM_pointer, test1234[a]);
        row_wr(4, buffer);
        transmit_array(buffer);
        SDRAM_pointer++;
    }
    UART0_Print("[INFO] SDRAM Read Operation Complete.\r\n");
}
