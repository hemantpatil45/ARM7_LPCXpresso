#include "LPC24xx.h"
#include "upper_sdram.h"
#include "SDRAM.h"

#define SECTOR_SIZE 512
#define WORDS_PER_SECTOR (SECTOR_SIZE / 2) // 256 16-bit words per 512-byte block

void UpperSDRAM_Init(void) {
    SDRAM_init_32BIT(); // Initialize EMC controller to clock the upper chip
}

void Upper16_Write(uint32_t index, uint16_t data) {
    volatile uint32_t *sdram = (volatile uint32_t *)0xA0000000;
    // Shift 16-bit data into upper half (D16-D31), leaving lower half to float
    sdram[index] = ((uint32_t)data << 16);
}

uint16_t Upper16_Read(uint32_t index) {
    volatile uint32_t *sdram = (volatile uint32_t *)0xA0000000;
    // Read 32-bit word, shift right to extract clean data from upper half
    uint32_t raw_val = sdram[index];
    return (uint16_t)(raw_val >> 16);
}

void UpperSDRAM_ReadBlock(uint32_t lba, uint8_t *buffer) {
    uint16_t *dest = (uint16_t *)buffer;
    uint32_t base_index = lba * WORDS_PER_SECTOR;
    for (int i = 0; i < WORDS_PER_SECTOR; i++) {
        dest[i] = Upper16_Read(base_index + i);
    }
}

void UpperSDRAM_WriteBlock(uint32_t lba, const uint8_t *buffer) {
    const uint16_t *src = (const uint16_t *)buffer;
    uint32_t base_index = lba * WORDS_PER_SECTOR;
    for (int i = 0; i < WORDS_PER_SECTOR; i++) {
        Upper16_Write(base_index + i, src[i]);
    }

    // Force-flush EMC write buffer by touching a distant address
    volatile uint32_t *sdram = (volatile uint32_t *)0xA0000000;
    sdram[4096] = 0xDEADBEEF;
}
