/*
 * flashread.h
 *
 *  Created on: 08-Oct-2025
 *      Author: ADMIN
 */

#ifndef FLASHREAD_H
#define FLASHREAD_H

#include <stdint.h>

/* Bring up EMC Static CS0 for x16 NOR and return NOR to read-array mode */
void EMC_Init_CS0_x16(void);   /* same name as in your main */
void nor_reset(void);

/* Read one 16-bit word at word offset (i.e., 0 = base, 1 = +2 bytes, …) */
uint16_t flash_read_word(uint32_t word_off);

/* Keypad (same pins so your main can gate reading with SW2) */
void   Keypad_Col1Row12_Init(void);
uint8_t SW2_Pressed(void);
uint8_t SW6_Pressed(void);   /* present for API symmetry */

#endif /* FLASHREAD_H */

