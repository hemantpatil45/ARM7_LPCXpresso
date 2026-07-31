#include "ff.h"
#include "diskio.h"
#include <stddef.h>
#include "type.h"
#include "usbhost_ms.h"
#include "usb_host_lpc2478.h"
#include "usb_host_td.h"
#include "uart0.h"

extern volatile DWORD g_USBConnected;
extern USB_INT32U MS_BlkSize;
extern volatile USB_INT08U *TDBuffer;
extern volatile HCED *EDCtrl;
extern volatile HCED *EDBulkIn;
extern volatile HCED *EDBulkOut;
extern void delay_ms(uint32_t ms);

// 512-byte Bridge Buffer in USB RAM
volatile USB_INT08U *USBSectorBuf = (volatile USB_INT08U *)0x7FD01000;

DSTATUS disk_status(BYTE pdrv) {
    if (pdrv != 0) return STA_NOINIT;
    if (!g_USBConnected) return STA_NODISK;
    return 0;
}

int USBHost_Enumerate(void) {
    UART0_Print("  [ENUM] Initializing Control Endpoint 0...\r\n");

    // Clear hardware lists and data toggles on hot-plug
    HC_CONTROL &= ~(1 << 5);
    HC_BULK_HEAD_ED = 0;

    EDBulkIn->HeadTd  = 0;
    EDBulkIn->TailTd  = 0;
    EDBulkIn->NextEd  = 0;

    EDBulkOut->HeadTd = 0;
    EDBulkOut->TailTd = 0;
    EDBulkOut->NextEd = 0;

    EDCtrl->Control = (64 << 16);
    EDCtrl->HeadTd  = 0;
    EDCtrl->TailTd  = 0;
    EDCtrl->NextEd  = 0;

    HC_CTRL_HEAD_ED = (USB_INT32U)EDCtrl;

    if (Host_CtrlRecv(0x00, 0x05, 1, 0, 0, NULL) != 0) return -1;
    delay_ms(50);

    EDCtrl->Control = (64 << 16) | 1;

    if (Host_CtrlRecv(0x80, 0x06, 0x0200, 0, 128, TDBuffer) != 0) return -1;
    if (Host_CtrlRecv(0x00, 0x09, 1, 0, 0, NULL) != 0) return -1;

    delay_ms(50);
    return 0;
}

DSTATUS disk_initialize(BYTE pdrv) {
    if (pdrv != 0) return STA_NOINIT;

    UART0_Print("[DISKIO] Resetting USB Port...\r\n");
    USBHost_ResetPort();
    delay_ms(100);

    UART0_Print("[DISKIO] Starting USB Enumeration...\r\n");
    if (USBHost_Enumerate() != 0) {
        UART0_Print("[DISKIO_ERR] Enumeration Failed!\r\n");
        return STA_NOINIT;
    }

    UART0_Print("[DISKIO] Parsing Configuration for MSC...\r\n");
    if (MS_ParseConfiguration() != OK) {
        UART0_Print("[DISKIO_ERR] MSC Configuration Parse Failed!\r\n");
        return STA_NOINIT;
    }

    UART0_Print("[DISKIO] Waiting for pendrive to wake up...\r\n");
    int retries = 20;
    while (MS_Init() != OK) {
        if (--retries == 0) {
            UART0_Print("[DISKIO_ERR] MSC SCSI Init Failed / Drive busy!\r\n");
            return STA_NOINIT;
        }
        UART0_Print("[DISKIO] Drive busy, retrying...\r\n");
        delay_ms(250);
    }

    UART0_Print("[DISKIO] Disk Initialized Successfully!\r\n");
    return 0;
}

DRESULT disk_read(BYTE pdrv, BYTE* buff, LBA_t sector, UINT count) {
    if (pdrv != 0 || !count) return RES_PARERR;

    for (UINT i = 0; i < count; i++) {
        if (MS_BulkRecv(sector + i, 1, (USB_INT08U*)USBSectorBuf) == OK) {
            for (int j = 0; j < 512; j++) {
                buff[(i * 512) + j] = USBSectorBuf[j];
            }
        } else {
            return RES_ERROR;
        }
    }
    return RES_OK;
}

DRESULT disk_write(BYTE pdrv, const BYTE* buff, LBA_t sector, UINT count) {
    if (pdrv != 0 || !count) return RES_PARERR;

    for (UINT i = 0; i < count; i++) {
        for (int j = 0; j < 512; j++) {
            USBSectorBuf[j] = buff[(i * 512) + j];
        }
        if (MS_BulkSend(sector + i, 1, (USB_INT08U*)USBSectorBuf) != OK) return RES_ERROR;
    }
    return RES_OK;
}

DRESULT disk_ioctl(BYTE pdrv, BYTE cmd, void* buff) {
    if (pdrv != 0) return RES_PARERR;
    switch (cmd) {
        case CTRL_SYNC:       return RES_OK;
        case GET_SECTOR_SIZE: *(WORD*)buff = (WORD)MS_BlkSize; return RES_OK;
        case GET_BLOCK_SIZE:  *(DWORD*)buff = 1; return RES_OK;
        default:              return RES_PARERR;
    }
}
