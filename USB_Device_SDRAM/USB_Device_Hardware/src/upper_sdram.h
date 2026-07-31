#ifndef UPPER_SDRAM_H
#define UPPER_SDRAM_H

#include <stdint.h>
#include <stdbool.h>

void UpperSDRAM_Init(void);
void Upper16_Write(uint32_t index, uint16_t data);
uint16_t Upper16_Read(uint32_t index);
void UpperSDRAM_ReadBlock(uint32_t lba, uint8_t *buffer);
void UpperSDRAM_WriteBlock(uint32_t lba, const uint8_t *buffer);

#endif
