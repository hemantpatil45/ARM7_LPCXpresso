#include "ff.h"
#include "diskio.h"
#include "usbhost_ms.h"

#define DEV_USB 0

DSTATUS disk_status (BYTE pdrv)
{
    if (pdrv != DEV_USB) return STA_NOINIT;
    return 0;
}

DSTATUS disk_initialize (BYTE pdrv)
{
    if (pdrv != DEV_USB) return STA_NOINIT;
    return 0;
}

DRESULT disk_read (BYTE pdrv, BYTE *buff, LBA_t sector, UINT count)
{
    if (pdrv != DEV_USB) return RES_PARERR;

    if (MS_BulkRecv(sector, count, buff) == 0) {
        return RES_OK;
    }
    return RES_ERROR;
}

DRESULT disk_write (BYTE pdrv, const BYTE *buff, LBA_t sector, UINT count)
{
    if (pdrv != DEV_USB) return RES_PARERR;

    if (MS_BulkSend(sector, count, (BYTE*)buff) == 0) {
        return RES_OK;
    }
    return RES_ERROR;
}

DRESULT disk_ioctl (BYTE pdrv, BYTE cmd, void *buff)
{
    if (pdrv != DEV_USB) return RES_PARERR;

    switch (cmd) {
        case CTRL_SYNC:
            return RES_OK;
        case GET_SECTOR_SIZE:
            *(WORD*)buff = 512;
            return RES_OK;
        case GET_BLOCK_SIZE:
            *(DWORD*)buff = 1;
            return RES_OK;
        case GET_SECTOR_COUNT:
            return RES_OK;
    }
    return RES_PARERR;
}

DWORD get_fattime (void)
{
    return ((DWORD)(44) << 25) | ((DWORD)1 << 21) | ((DWORD)1 << 16);
}
