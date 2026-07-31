#include "LPC24xx.h"
#include "UART.h"

// Initialize UART0 for serial communication
void UART0_Init(uint32_t baudrate) {
    uint32_t Fdiv;

    // 1. Enable power to UART0 peripheral
    PCONP |= (1 << 3);                              //[cite: 2]

    // 2. Set peripheral clock for UART0
    PCLKSEL0 &= ~(3 << 6);                          //[cite: 2]

    // 3. Set pins P0.2 (TX) and P0.3 (RX) for UART function
    PINSEL0 &= ~0x000000F0;                         //[cite: 2]
    PINSEL0 |=  0x00000050;                         //[cite: 2]

    // 4. Set 8-bit mode, 1 stop bit, and enable access to divisor latches (DLAB)
    U0LCR = 0x83;                                   //[cite: 2]

    // 5. Calculate and set baud rate divisor with rounding for better accuracy
    Fdiv = ((Fpclk / 16) + (baudrate / 2)) / baudrate;
    U0DLM = (Fdiv >> 8) & 0xFF;                     //[cite: 2]
    U0DLL = Fdiv & 0xFF;                            //[cite: 2]

    // 6. Disable fractional divider for standard baud rate stability
    U0FDR = 0x10;                                   //[cite: 2]

    // 7. Disable DLAB to access data registers
    U0LCR = 0x03;                                   //[cite: 2]

    // 8. Enable and clear UART transmit/receive FIFOs
    U0FCR = 0x07;                                   //[cite: 2]
}

// Send a single character over UART
void UART0_SendChar(char ch) {
    // Wait until the Transmit Holding Register is empty
    while (!(U0LSR & 0x20));                        //[cite: 2]
    U0THR = ch;                                     //[cite: 2]
}

// Send a null-terminated string over UART
void UART0_SendString(const char *str) {
    while (*str) {                                  //[cite: 2]
        UART0_SendChar(*str++);                     //[cite: 2]
    }
}

// Send a raw buffer of bytes over UART
void UART0_SendBuffer(const uint8_t *buf, uint32_t len) {
    for (uint32_t k = 0; k < len; k++) {            //[cite: 2]
        while (!(U0LSR & 0x20));                    //[cite: 2]
        U0THR = buf[k];                             //[cite: 2]
    }
}

// Check if data is available to read
bool UART0_DataReady(void) {
    return (U0LSR & 0x01); // Line Status Register bit 0 is RDR (Receive Data Ready)
}

// Receive a single character over UART (Blocking)
char UART0_ReceiveChar(void) {
    while (!UART0_DataReady()); // Wait until data arrives
    return U0RBR; // Return the character from the Receive Buffer Register
}

// Initialize Timer 0 once during startup
void Timer0_Init(void) {
    PCONP |= (1 << 1);      // Power up Timer 0
    T0CTCR = 0x00;          // Set Timer mode (not counter mode)

    // PCLK is 18 MHz. We want 1 tick per millisecond (1000 Hz)
    // 18,000,000 / 1000 = 18,000 cycles per millisecond
    T0PR = 18000 - 1;       // Set Prescaler to 18,000
}

// Perfect millisecond delay function
void Delay_ms(uint32_t ms) {
    T0TCR = 0x02;           // Reset Timer
    T0TCR = 0x01;           // Enable Timer
    while (T0TC < ms);      // Wait until Timer Counter reaches target ms
    T0TCR = 0x00;           // Disable Timer
}
