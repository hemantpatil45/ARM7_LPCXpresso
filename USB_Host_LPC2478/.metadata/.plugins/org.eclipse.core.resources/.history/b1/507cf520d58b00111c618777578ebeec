#include <stddef.h>
#include "LPC24xx.h"
#include "system_init.h"
#include "uart0.h"
#include "usb_host_lpc2478.h"
#include "ff.h"

extern volatile DWORD g_USBConnected;

FATFS FatFsObj;
FIL TestFile;

/* Dummy Time Function for FatFs */
DWORD get_fattime(void) {
    return ((DWORD)(2026 - 1980) << 25) | ((DWORD)1 << 21) | ((DWORD)1 << 16);
}

/* Simple Millisecond Software Delay */
void delay_ms(uint32_t ms) {
    volatile uint32_t i;
    while (ms--) {
        // At 48 MHz, ~12,000 loop iterations takes roughly 1 millisecond
        for (i = 0; i < 1200; i++) {
            __asm__ volatile ("nop");
        }
    }
}

int main(void) {
    UINT bytesWritten;
    FRESULT fr;

    // 1. Initialize System Clocks (48MHz)
    PLL_Init();
    system_Init();

    // ==========================================
    // HARDWARE STABILIZATION DELAY
    // Give power rails, PHY, and clocks 500ms to settle
    // ==========================================
    delay_ms(5000);

    // 2. Initialize UART0 immediately to start logging
    UART0_Init();
    UART0_Print("\r\n=================================\r\n");
    UART0_Print("[MAIN] LPC2478 System Booting...\r\n");

    // 3. Initialize USB Host Low-Level Hardware
    UART0_Print("[MAIN] Configuring USB PHY on Port 2...\r\n");
    USBHost_HardwareInit();

    // 4. Initialize OHCI Host Controller Block
    UART0_Print("[MAIN] Booting OHCI Host Controller...\r\n");
    if (USBHost_OHCIInit() != USB_HOST_OK) {
        UART0_Print("[MAIN_ERR] FATAL: OHCI Controller rejected initialization!\r\n");
        while (1);
    }

    UART0_Print("[MAIN] OHCI Active. Waiting for USB Flash Drive...\r\n");

    // Main Application Loop
    while (1) {
        if (g_USBConnected) {

            // Adding a small debounce delay after insertion is detected
            UART0_Print("\r\n[MAIN] >> USB DEVICE INSERTED (Debouncing...) <<\r\n");
            delay_ms(250);

            // Turn ON USB UP LED
            FIO0CLR = (1UL << 13);

            // Attempt to Mount the USB Drive via FatFs
            UART0_Print("[MAIN] Attempting to mount FatFs...\r\n");
            fr = f_mount(&FatFsObj, "0:", 1);

            if (fr == FR_OK) {
                UART0_Print("[MAIN] FatFs Mounted Successfully!\r\n");

                UART0_Print("[MAIN] Opening HOST_LOG.TXT...\r\n");
                fr = f_open(&TestFile, "0:/USBHOST_Test.txt", FA_WRITE | FA_OPEN_ALWAYS);

                if (fr == FR_OK) {
                    UART0_Print("[MAIN] Writing test string to file...\r\n");
                    f_write(&TestFile, "Welcome to Akademika Lab Solution USB host implemented successfully\r\n", 67, &bytesWritten);
                    f_close(&TestFile);
                    UART0_Print("[MAIN] Write complete and file closed.\r\n");
                } else {
                    UART0_Print("[MAIN_ERR] Failed to open file!\r\n");
                }
            } else {
                UART0_Print("[MAIN_ERR] FatFs Mount Failed!\r\n");
            }

            UART0_Print("[MAIN] Operations finished. Unmount device to continue.\r\n");

            // Unmount and wait until device is removed
            while (g_USBConnected);

            UART0_Print("\r\n[MAIN] >> USB DEVICE REMOVED <<\r\n");
            UART0_Print("[MAIN] Waiting for new device...\r\n");

            // Turn OFF USB UP LED
            FIO0SET = (1UL << 13);
            f_mount(NULL, "0:", 0);
        }
    }
    return 0;
}
