/*
 * flashwrite.h
 *
 *  Created on: 08-Oct-2025
 *      Author: ADMIN
 */

#ifndef FLASHWRITE_H
#define FLASHWRITE_H

#include <stdint.h>

/* Bring up EMC Static CS0 for x16 NOR and return NOR to read-array mode */
void EMC_Init_CS0_x16(void);   /* used by your main */
void nor_reset(void);          /* used by your main */

/* SST39VF3202C (x16) primitives */
int  flash_sector_erase(uint32_t byte_addr);           /* 0=OK */
int  flash_program_word(uint32_t byte_addr, uint16_t data);

/* Optional helpers if you want buffer write (not required by your main) */
int  flash_write_buf(uint32_t byte_addr, const uint16_t *src, uint32_t words);

/* Keypad: Col1=P2.25, Row1=P2.0(SW2), Row2=P2.1(SW6) */
void   Keypad_Col1Row12_Init(void);
uint8_t SW2_Pressed(void);   /* debounced, returns 1 when pressed (active low) */
uint8_t SW6_Pressed(void);   /* debounced, returns 1 when pressed (active low) */

#endif /* FLASHWRITE_H */

