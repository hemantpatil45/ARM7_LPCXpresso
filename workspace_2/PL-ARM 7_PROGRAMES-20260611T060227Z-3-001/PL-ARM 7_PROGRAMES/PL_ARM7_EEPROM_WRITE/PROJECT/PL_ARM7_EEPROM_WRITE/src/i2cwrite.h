/*
 * i2cwrite.h
 *
 *  Created on: 03-Oct-2025
 *      Author: ADMIN
 */

/*
 * i2cwrite.h
 * API for AT24C512 I2C EEPROM write with SW2 keypad trigger
 */

#ifndef I2CWRITE_H
#define I2CWRITE_H

#include <stdint.h>

/* ===== EEPROM constants ===== */
#define AT24C512_ADDR7   0x50u
#define AT24C512_PAGE_SZ 128u

#ifdef __cplusplus
extern "C" {
#endif

/* Init I2C0 @100kHz */
void I2C0_Init_100k(void);

/* High-level EEPROM write (splits across pages) */
void AT24C512_WriteBuffer(uint16_t start, const uint8_t *src, uint32_t nbytes);

/* Keypad SW2 setup + press detection */
void SW2_Keypad_Init(void);
uint32_t SW2_Pressed(void);

#ifdef __cplusplus
}
#endif

#endif /* I2CWRITE_H */
