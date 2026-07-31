#include "usb_host_td.h"
#include "uart0.h"
#include <stddef.h>

// 16-Byte Aligned OHCI Descriptors in USB RAM (0x7FD00000)
volatile HCED *EDCtrl    = (volatile HCED *)0x7FD00100;
volatile HCED *EDBulkIn  = (volatile HCED *)0x7FD00110;
volatile HCED *EDBulkOut = (volatile HCED *)0x7FD00120;
volatile HCTD *TDHead    = (volatile HCTD *)0x7FD00130;
volatile HCTD *TDTail    = (volatile HCTD *)0x7FD00140;

// Data Buffers mapped to safe USB RAM space
volatile USB_INT08U *TDBuffer    = (volatile USB_INT08U *)0x7FD00200;
volatile USB_INT08U *SetupBuffer = (volatile USB_INT08U *)0x7FD00280;

USB_INT32S Host_ProcessTD(volatile HCED *ed,
                          USB_INT32U token,
                          volatile USB_INT08U *buffer,
                          USB_INT32U buffer_len)
{
    UART0_Print("\r\n=====================================\r\n");
    UART0_Print("[DEBUG] Entering Host_ProcessTD\r\n");
    UART0_Print("[DEBUG] Token = ");
    UART0_PrintHex(token);
    UART0_Print("\r\n");
    UART0_Print("[DEBUG] Length = ");
    UART0_PrintHex(buffer_len);
    UART0_Print("\r\n");

    volatile USB_INT32U status;
    volatile USB_INT32U timeout = 500000; // ~1 second optimized timeout

    TDTail->Control    = 0;
    TDTail->CurrBufPtr = 0;
    TDTail->NextTd     = 0;
    TDTail->BufEnd     = 0;

    TDHead->Control = (1 << 18) | (token << 19) | (7 << 21) | (0xF << 28);

    if (token == TD_SETUP)
    {
        TDHead->Control |= (2 << 24); // Force DATA0 for SETUP
    }
    else if (ed == EDCtrl && buffer_len == 0)
    {
        TDHead->Control |= (3 << 24); // Force DATA1 for Control Status Phase
    }

    TDHead->CurrBufPtr = (USB_INT32U)buffer;
    TDHead->BufEnd     = (buffer_len == 0) ? 0 : (USB_INT32U)(buffer + buffer_len - 1);
    TDHead->NextTd     = (USB_INT32U)TDTail;

    ed->NextEd = 0;
    ed->HeadTd = (USB_INT32U)TDHead | (ed->HeadTd & 0x00000002);
    ed->TailTd = (USB_INT32U)TDTail;

    if (ed == EDCtrl)
    {
        UART0_Print("\r\n===== CONTROL LIST ENABLE =====\r\n");
        HC_CONTROL |= (1 << 4);
        HC_CMD_STAT |= (1 << 1);
    }
    else
    {
        UART0_Print("\r\n===== BULK LIST ENABLE =====\r\n");

        // Statically link Out to In for smooth traversal
        EDBulkOut->NextEd = (USB_INT32U)EDBulkIn;
        EDBulkIn->NextEd  = 0;

        if ((HC_CONTROL & (1 << 5)) == 0) {
            HC_BULK_HEAD_ED = (USB_INT32U)EDBulkOut;
        }

        HC_CONTROL |= (1 << 5);
        HC_CMD_STAT |= (1 << 2);
    }

    UART0_Print("[DEBUG] Polling starts...\r\n");

    static USB_INT32U counter = 0;

    do
    {
        status = ed->HeadTd;
        counter++;

        if ((counter % 100000) == 0)
        {
            UART0_Print("[DEBUG] HeadTD = ");
            UART0_PrintHex(status);
            UART0_Print(" HC_INT_STAT = ");
            UART0_PrintHex(HC_INT_STAT);
            UART0_Print("\r\n");
        }

        if (HC_INT_STAT & 0x10)
        {
            UART0_Print("[TD_ERR] UNRECOVERABLE HARDWARE ERROR\r\n");
            HC_INT_STAT = 0x10;
            return 1;
        }

        if (status & 0x01)
        {
            UART0_Print("[TD_ERR] Endpoint HALTED. CC: ");
            USB_INT32U err_code = (TDHead->Control >> 28) & 0x0F;
            if (err_code < 10) UART0_SendChar(err_code + '0');
            else UART0_SendChar((err_code - 10) + 'A');
            UART0_Print("\r\n");

            ed->HeadTd &= ~0x01;
            return 1;
        }

        if (--timeout == 0)
        {
            UART0_Print("[TD_ERR] SOFTWARE TIMEOUT - Device NAK'd too long\r\n");
            // Clean up ED head on timeout so it doesn't lock up next attempts
            ed->HeadTd = (USB_INT32U)TDTail | (ed->HeadTd & 0x00000002);
            return 1;
        }

        for (volatile int i = 0; i < 50; i++);

    } while ((status & 0xFFFFFFF0) != (USB_INT32U)TDTail);

    UART0_Print("[DEBUG] TD Completed\r\n");
    status = (TDHead->Control >> 28) & 0x0F;

    if (status != 0)
    {
        UART0_Print("[TD_ERR] TD Error\r\n");
        return 1;
    }

    UART0_Print("[DEBUG] Host_ProcessTD SUCCESS\r\n");
    return 0;
}

USB_INT32S Host_CtrlRecv(USB_INT08U bmRequestType, USB_INT08U bRequest, USB_INT16U wValue, USB_INT16U wIndex, USB_INT16U wLength, volatile USB_INT08U *buffer) {
    UART0_Print("\r\n=== Host_CtrlRecv ===\r\n");

    SetupBuffer[0] = bmRequestType;
    SetupBuffer[1] = bRequest;
    SetupBuffer[2] = wValue & 0xFF;
    SetupBuffer[3] = (wValue >> 8) & 0xFF;
    SetupBuffer[4] = wIndex & 0xFF;
    SetupBuffer[5] = (wIndex >> 8) & 0xFF;
    SetupBuffer[6] = wLength & 0xFF;
    SetupBuffer[7] = (wLength >> 8) & 0xFF;

    if (Host_ProcessTD(EDCtrl, TD_SETUP, SetupBuffer, 8) != 0) return 1;

    if (wLength > 0) {
        if (bmRequestType & 0x80) {
            if (Host_ProcessTD(EDCtrl, TD_IN, buffer, wLength) != 0) return 1;
        } else {
            if (Host_ProcessTD(EDCtrl, TD_OUT, (volatile USB_INT08U*)buffer, wLength) != 0) return 1;
        }
    }

    if (wLength == 0) {
        if (Host_ProcessTD(EDCtrl, TD_IN, NULL, 0) != 0) return 1;
    } else if (bmRequestType & 0x80) {
        if (Host_ProcessTD(EDCtrl, TD_OUT, NULL, 0) != 0) return 1;
    } else {
        if (Host_ProcessTD(EDCtrl, TD_IN, NULL, 0) != 0) return 1;
    }

    return 0;
}
