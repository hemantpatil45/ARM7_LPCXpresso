#include "LPC24xx.h"
#include "usbhw.h"
#include "usbcore.h"
#include "system_init.h"
#include <stdbool.h>

#define UART0_BAUD 115200

extern void PLL_Init(void);
extern void system_Init(void);
extern volatile bool usb_event_detected;

void Enable_Global_Interrupts(void) {
    __asm("mrs r0, cpsr");
    __asm("bic r0, r0, #0x80");
    __asm("msr cpsr_c, r0");
}

void UART0_Init(uint32_t baudrate) {
    uint32_t Fdiv;
    PCONP |= (1 << 3);

    // Set UART0 clock to CCLK/1 (48MHz)
    PCLKSEL0 &= ~(3 << 6);
    PCLKSEL0 |=  (1 << 6);

    PINSEL0 &= ~0x000000F0;
    PINSEL0 |=  0x00000050;
    U0LCR = 0x83;

    // Full 48MHz for calculation (48000000 / 16 = 3000000)
    Fdiv = 3000000 / baudrate;

    U0DLM = (Fdiv >> 8) & 0xFF;
    U0DLL = Fdiv & 0xFF;
    U0FDR = 0x10;
    U0LCR = 0x03;
    U0FCR = 0x07;
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

void delay_ms(uint32_t ms) {
    for (volatile uint32_t i = 0; i < (ms * 1000); i++);
}

int main(void) {
    PLL_Init();
    system_Init();
    UART0_Init(UART0_BAUD);

    delay_ms(3000);
    UART0_Print("\r\n[BOOT] LPC2478 USB Booting...\r\n");
    delay_ms(100);

    // Write the Boot Sector Signature to RAM before USB enumerates
    Init_RAM_Disk();
    UART0_Print("[MEM] 32KB RAM Disk Formatted.\r\n");

    Enable_Global_Interrupts();
    UART0_Print("[CPU] Global Interrupts Enabled.\r\n");

    UART0_Print("[USB] Initializing USB Engine...\r\n");
    USB_Init();

    UART0_Print("[USB] Connecting to Bus (D+ pull-up)...\r\n");
    USB_Connect(true);

    while (1) {
        if (usb_event_detected) {
            usb_event_detected = false;
        }
    }
}
