#include "uart0.h"

void UART0_Init(void) {
    // 1. Power up UART0 (Usually enabled by default, but best practice)
    PCONP |= (1UL << 3);

    // 2. Configure P0.2 as TXD0 and P0.3 as RXD0
    // Clear bits 4, 5, 6, 7 then set bits 4 and 6 to '1' (Function 01)
    PINSEL0 = (PINSEL0 & ~0x000000F0) | 0x00000050;

    // 3. Configure UART0 Protocol: 8-bit Data, 1 Stop bit, No Parity, Enable DLAB
    U0LCR = 0x83; // 0x80 (DLAB) | 0x03 (8-bit)

    // 4. Calculate Baud Rate for 115200
    // PCLK = 12 MHz (48MHz CPU / 4).
    // Formula: PCLK / (16 * Baud) = DL + (DIVADDVAL / MULVAL)
    // 12000000 / (16 * 115200) = 6.5104
    // We choose DL = 4. Remainder is ~2.51, so (1 + 5/8) gives 1.625.
    // 4 * 1.625 = 6.5. Actual baud = 115384 (Only 0.16% Error, which is perfect).

    U0DLM = 0;
    U0DLL = 4;

    // Set Fractional Divider: MULVAL = 8, DIVADDVAL = 5
    U0FDR = (8 << 4) | 5;

    // 5. Disable DLAB (Lock in the baud rate)
    U0LCR = 0x03;

    // 6. Enable and Reset TX and RX FIFOs
    U0FCR = 0x07;
}

void UART0_SendChar(char c) {
    // Wait until the Transmit Holding Register is Empty (THRE bit 5)
    while (!(U0LSR & (1 << 5)));
    U0THR = c;
}

void UART0_Print(const char *str) {
    while (*str) {
        UART0_SendChar(*str++);
    }
}
void UART0_PrintHex(unsigned int value)
{
    char hex[] = "0123456789ABCDEF";

    UART0_Print("0x");

    for (int i = 28; i >= 0; i -= 4)
    {
        UART0_SendChar(hex[(value >> i) & 0xF]);
    }
}

void UART0_PrintDec(unsigned int value)
{
    char buf[11];
    int i = 0;

    if (value == 0)
    {
        UART0_SendChar('0');
        return;
    }

    while (value)
    {
        buf[i++] = (value % 10) + '0';
        value /= 10;
    }

    while (i--)
    {
        UART0_SendChar(buf[i]);
    }
}
