#include "ff.h"
#include "diskio.h"
#include "SPI.h"
#include "LPC24xx.h" // For clock speed register

// Card status flag
static volatile DSTATUS Stat = STA_NOINIT;

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/
DSTATUS disk_status (BYTE pdrv)
{
    if (pdrv != 0) return STA_NOINIT; // We only support Drive 0
    return Stat;
}

/*-----------------------------------------------------------------------*/
/* Initialize a Drive                                                    */
/*-----------------------------------------------------------------------*/
DSTATUS disk_initialize (BYTE pdrv)
{
    uint8_t result;

    if (pdrv != 0) return STA_NOINIT;

    // Call your custom SD initialization routine
    result = sd_init();

    if (result == 0x00) {
        // SUCCESS: The card is initialized!
        // Now, we MUST increase the SPI clock speed for data transfer.
        // PCLK / 2 is usually safe. (Change 254 back to 2 or 4).
        SSP0CPSR = 4;

        Stat &= ~STA_NOINIT; // Clear the NOINIT flag
        return Stat;
    }

    return STA_NOINIT; // Failed to initialize
}

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/
DRESULT disk_read (BYTE pdrv, BYTE* buff, LBA_t sector, UINT count)
{
    if (pdrv != 0 || (Stat & STA_NOINIT)) return RES_NOTRDY;

    // FatFs might ask for multiple sectors at once. We loop through them.
    while (count > 0) {
        if (sd_read_sector(sector, buff) != 0) {
            return RES_ERROR;
        }
        sector++;
        buff += 512;
        count--;
    }
    return RES_OK;
}

/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/
DRESULT disk_write (BYTE pdrv, const BYTE* buff, LBA_t sector, UINT count)
{
    if (pdrv != 0 || (Stat & STA_NOINIT)) return RES_NOTRDY;

    // Loop through multiple sectors
    while (count > 0) {
        if (sd_write_sector(sector, buff) != 0) {
            return RES_ERROR;
        }
        sector++;
        buff += 512;
        count--;
    }
    return RES_OK;
}

/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/
DRESULT disk_ioctl (BYTE pdrv, BYTE cmd, void* buff)
{
    if (pdrv != 0 || (Stat & STA_NOINIT)) return RES_NOTRDY;

    // CTRL_SYNC is required for write operations to ensure data is flushed
    if (cmd == CTRL_SYNC) {
        return RES_OK;
    }

    return RES_PARERR;
}
