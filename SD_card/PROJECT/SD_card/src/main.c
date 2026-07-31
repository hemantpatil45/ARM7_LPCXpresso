
#include "LPC24xx.h"
#include "glcd.h"
#include "system_init.h"
#include "SPI.h"
#include "ff.h"
#include <stdint.h>
#include <string.h>

// Required by FatFs for file timestamps
DWORD get_fattime(void) {
    return ((DWORD)(2024 - 1980) << 25) | ((DWORD)1 << 21) | ((DWORD)1 << 16);
}

int main(void)
{
    // FatFs Variables
    FATFS fs;
    FIL fil;
    FRESULT fr;
    UINT bw;                  // Bytes written
    UINT br;                  // Bytes read

   // Data Buffers
    const char write_buffer[] = "Hello from akademika";
    char read_buffer[64];     // Buffer to hold the text from the SD card
    char display_row[24];     // Temporary buffer to format text for the GLCD

   //Hardware Initialization
    PLL_Init();
    system_Init();
    GLCD_Init();
    GLCD_Clear();
    spi_init();

    GLCD_RowWriteMargin(0, "AKADEMIKA FATFS TEST");
    GLCD_RowWriteMargin(1, "MOUNTING DRIVE...   ");

    //Mount the SD card
    fr = f_mount(&fs, "", 1);
    if (fr != FR_OK) {
        GLCD_RowWriteMargin(2, "ERR: MOUNT FAILED   ");
        for (;;) ; // Halt
    }

    //Write to the file
    GLCD_RowWriteMargin(2, "WRITING DATA...     ");
    fr = f_open(&fil, "TEST.TXT", FA_WRITE | FA_CREATE_ALWAYS);

    if (fr == FR_OK) {
        f_write(&fil, write_buffer, strlen(write_buffer), &bw);
        f_close(&fil); // Always close to save changes
    } else {
        GLCD_RowWriteMargin(3, "ERR: WRITE FAILED   ");
        for (;;) ; // Halt
    }

    //Read from the file
    GLCD_RowWriteMargin(3, "READING DATA...     ");
    fr = f_open(&fil, "TEST.TXT", FA_READ);

    if (fr == FR_OK) {
        // Clear the buffer with zeros so we don't print garbage memory
        memset(read_buffer, 0, sizeof(read_buffer));

        // Read the data back
        fr = f_read(&fil, read_buffer, sizeof(read_buffer) - 1, &br);
        f_close(&fil);
    } else {
        GLCD_RowWriteMargin(4, "ERR: FILE NOT FOUND ");
        for (;;) ; // Halt
    }

    // display on the glcd
    if (fr == FR_OK && br > 0) {
        GLCD_RowWriteMargin(4, "FILE CONTENTS:      ");

        // Print the first 20 characters to Row 5
        memset(display_row, 0, sizeof(display_row));
        strncpy(display_row, read_buffer, 20);
        GLCD_RowWriteMargin(5, display_row);

        // Print the remaining characters to Row 6 (if any)
        memset(display_row, 0, sizeof(display_row));
        if (br > 20) {
            strncpy(display_row, read_buffer + 20, 20);
        }
        GLCD_RowWriteMargin(6, display_row);

    } else {
        GLCD_RowWriteMargin(5, "ERR: READ EMPTY     ");
    }


     //  6. HALT EXECUTION

    for (;;) {
        // Do nothing. Wait for hardware reset.
    }
}
