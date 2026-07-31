#include "LPC24xx.h"
#include "usbhost_inc.h"

/*****************************************************************************
** Function name:       OHCIPortOpen
** Descriptions:        Open designated USB port and apply VBUS power
*****************************************************************************/
void OHCIPortOpen(DWORD portNum)
{
    if (portNum == 1) {
        HC_RH_PORT_STAT1 = 0x00000100; /* Set Port Power */
    }
    else if (portNum == 2) {
        HC_RH_PORT_STAT2 = 0x00000100; /* Set Port Power (PPS) for Port 2 */
        UART0_Print("[USB-DBG] OHCIPortOpen: Port 2 Power Applied (Bit 8)\r\n");
    }
    delayMs(1, 100);
}

/*****************************************************************************
** Function name:       Host_EnumDev
** Descriptions:        USB host device enumeration routine for Port 2
*****************************************************************************/
DWORD Host_EnumDev(void)
{
    DWORD port_stat = HC_RH_PORT_STAT2;

    /* Check if Connect Status Change (Bit 16) is asserted on Port 2 */
    if (port_stat & (1 << 16))
    {
        UART0_Print("\r\n[USB-DBG] Event: Device connection change detected!\r\n");

        /* 1. Clear Connect Status Change */
        HC_RH_PORT_STAT2 = (1 << 16);
        UART0_Print("[USB-DBG] Action: Cleared Connect Change Flag (Bit 16).\r\n");
        delayMs(1, 200); // Allow hardware to stabilize

        /* 2. Issue Port Reset (Set Bit 4) */
        HC_RH_PORT_STAT2 = (1 << 4);
        UART0_Print("[USB-DBG] Action: Issued Port Reset (Bit 4).\r\n");
        delayMs(1, 200); // Wait for reset to complete

        /* 3. Clear Reset Change (Bit 20) */
        HC_RH_PORT_STAT2 = (1 << 20);
        UART0_Print("[USB-DBG] Action: Cleared Reset Change Flag (Bit 20).\r\n");
        delayMs(1, 200);
    }

    /* Check if Port Enable Status (Bit 1) is active on Port 2 */
    port_stat = HC_RH_PORT_STAT2;
    if (port_stat & (1 << 1))
    {
        /* Return OK only if the port successfully locked its enable state */
        return OK;
    }

    return ERROR;
}
