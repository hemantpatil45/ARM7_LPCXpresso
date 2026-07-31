#include "LPC24xx.h"
#include "system_init.h"
#include "usbhost_inc.h"
#include "ff.h"

FATFS fs;

int main(void)
{
    /* 1. System Clocks & Pins */
    PLL_Init();
    system_Init();

    /* 2. Terminal Debugging */
    UART0_Init(115200);
    delayMs(1, 500);
    UART0_Print("\r\n==================================\r\n");
    UART0_Print("[BOOT] LPC2478 USB Host Booting...\r\n");
    UART0_Print("==================================\r\n");

    /* 3. Initialize USB Host Hardware & OHCI Controller */
    UART0_Print("[MAIN] Initializing USB RAM and OHCI Controller...\r\n");
    USBHostInit();
    OHCIInit();

    /* 4. Open Port 2 (Mapped to physical USB socket) */
    OHCIPortOpen(2);

    UART0_Print("[MAIN] Controller Ready. Scanning Port 2 for Flash Drive...\r\n");

    while (1)
    {
        /* Poll Port 2 for device enumeration */
        if (Host_EnumDev() == OK)
        {
            UART0_Print("\r\n>>> [SUCCESS] Flash Drive Enumerated on Port 2! <<<\r\n");

            /* Initialize Mass Storage Interface */
            UART0_Print("[MAIN] Attempting SCSI Mass Storage Initialization...\r\n");
            if (MS_Init() == 0)
            {
                UART0_Print(">>> [SUCCESS] Mass Storage Initialized! <<<\r\n");

                /* Mount the FAT file system */
                UART0_Print("[MAIN] Attempting to Mount FAT File System...\r\n");
                if (f_mount(&fs, "0:", 1) == FR_OK)
                {
                    UART0_Print("\r\n########################################\r\n");
                    UART0_Print(">>> [FAT] DRIVE SUCCESSFULLY MOUNTED! <<<\r\n");
                    UART0_Print("########################################\r\n");

                    while(1); /* Drive is ready. Replace this with file read/write operations */
                }
                else
                {
                    UART0_Print("[ERROR] f_mount failed. Check drive format (FAT16/FAT32).\r\n");
                }
            }
            else
            {
                UART0_Print("[ERROR] MS_Init failed. Device may not be Mass Storage Class.\r\n");
            }
        }

        /* Pause before polling again */
        delayMs(1, 500);
    }
}
