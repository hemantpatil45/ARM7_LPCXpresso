/*
 * i2cread.h
 *
 *  Created on: 09-Oct-2025
 *      Author: ADMIN
 */



#ifndef I2C_READ_H
#define I2C_READ_H

#include <stdint.h>

/* ======== EEPROM parameters ======== */
#define AT24C512_ADDR7   0x50u
#define AT24C512_PAGE_SZ 128u

/* ======== Public API ======== */

void I2C0_Init_100k(void);

uint8_t EE_ReadByte(uint16_t addr);

void EE_ReadBuffer(uint16_t start, uint8_t *dst, uint32_t nbytes);

void SW2_Keypad_Init(void);
uint32_t SW2_Pressed(void);

#endif /* I2C_READ_H */
