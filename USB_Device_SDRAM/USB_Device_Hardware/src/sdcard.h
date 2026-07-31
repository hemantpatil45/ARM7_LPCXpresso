#ifndef SDCARD_H
#define SDCARD_H

#include <stdint.h>
#include <stdbool.h>

bool SD_Init(void);
bool SD_Read_Block(uint32_t lba, uint8_t *buffer);
bool SD_Write_Block(uint32_t lba, uint8_t *buffer);

#endif
