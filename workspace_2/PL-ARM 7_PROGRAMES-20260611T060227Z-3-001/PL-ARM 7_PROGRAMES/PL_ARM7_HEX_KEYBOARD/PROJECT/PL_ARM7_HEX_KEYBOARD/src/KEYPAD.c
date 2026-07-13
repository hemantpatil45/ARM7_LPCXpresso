/*
 * KEYPAD.c
 *
 *  Created on: Sep 30, 2025
 *      Author: divya-devdhe
 */

/* ==== Initialize keypad GPIO pins ==== */
/* ===== keypad mapping (row-major order) ===== */
#include <stdint.h>
#include "LPC24xx.h"
#include "glcd.h"
#include "system_init.h"
#include "KEYPAD.h"
char key;

// KEYPAD.h
extern char keymap[4][4] = {
  {'0','4','8','C'},
  {'1','5','9','D'},
  {'2','6','A','E'},
  {'3','7','B','F'}
};

void keypad_init(void)
{
    FIO2DIR &= ~ROW_MASK_P2;                  // Input direction
    FIO2DIR |= COL_MASK_P2;                   // Output direction
    FIO0DIR |= (1u<<COL3_P0_BIT);

    PINMODE4 &= ~((3u << (ROW0_P2_BIT*2)) | (3u << (ROW1_P2_BIT*2)));
    PINMODE4 |= ((2u << (ROW0_P2_BIT*2)) | (2u << (ROW1_P2_BIT*2)));
    PINMODE5 &= ~((3u << ((ROW2_P2_BIT-16)*2)) | (3u << ((ROW3_P2_BIT-16)*2)) | (3u << ((COL0_P2_BIT-16)*2)) | (3u << ((COL1_P2_BIT-16)*2)) | (3u << ((COL2_P2_BIT-16)*2)));
    PINMODE5 |= ((2u << ((ROW2_P2_BIT-16)*2)) | (2u << ((ROW3_P2_BIT-16)*2)) | (2u << ((COL0_P2_BIT-16)*2)) | (2u << ((COL1_P2_BIT-16)*2)) | (2u << ((COL2_P2_BIT-16)*2)));
    PINMODE0 &= ~(3u << (COL3_P0_BIT*2));
    PINMODE0 |= (2u << (COL3_P0_BIT*2));   // Disable pull-up/down on column output

    FIO2SET = COL_MASK_P2;
    FIO0SET = (1u<<COL3_P0_BIT);
}

/* ==== drive all columns HIGH (inactive) ==== */
void columns_all_high(void)
{
    FIO2SET = COL_MASK_P2;
    FIO0SET = (1u<<COL3_P0_BIT);
}

/* ==== drive single column LOW ==== */
void drive_column_low(uint8_t col)
{
    columns_all_high();
    switch(col)
    {
        case 0: FIO2CLR = (1u << COL0_P2_BIT); break;
        case 1: FIO2CLR = (1u << COL1_P2_BIT); break;
        case 2: FIO2CLR = (1u << COL2_P2_BIT); break;
        case 3: FIO0CLR = (1u << COL3_P0_BIT); break;
        default: break;
    }
}

/* ==== read rows, return 4-bit mask, bit=1 means PRESSED ==== */
uint8_t read_rows_pressed(void)
{
    uint32_t p2 = FIO2PIN;
    uint8_t r0 = ((p2 & (1u << ROW0_P2_BIT)) == 0) ? 1u: 0u;
    uint8_t r1 = ((p2 & (1u << ROW1_P2_BIT)) == 0) ? 1u: 0u;
    uint8_t r2 = ((p2 & (1u << ROW2_P2_BIT)) == 0) ? 1u: 0u;
    uint8_t r3 = ((p2 & (1u << ROW3_P2_BIT)) == 0) ? 1u: 0u;

    return (r0 | (r1 << 1) | (r2 << 2) | (r3 << 3));
}

/* ==== find index of first set bit (0..3), else 255 ==== */
uint8_t first_set4(uint8_t m)
{
    if (m & 1u) return 0;
    if (m & 2u) return 1;
    if (m & 4u) return 2;
    if (m & 8u) return 3;
    return 255;
}

/* ==== scan keypad once, returns pressed key char or 0 ==== */
char keypad_scan_once(void)
{
    for (uint8_t col = 0; col < 4; col++)
    {
        drive_column_low(col);
        delay_us(5);                         // settling time

        uint8_t rows = read_rows_pressed();
        if (rows)
        {
            delay_us(200);   // debounce delay ~20ms
            uint8_t rows2 = read_rows_pressed();
            if ((rows2 & rows) != 0)
            {
                uint8_t row = first_set4(rows2);
                if (row != 255)
                {
                    while (read_rows_pressed()) delay_us(100);  // wait until release
                    columns_all_high();
                    return keymap[row][col];
                }
            }
        }
    }
    columns_all_high();
    return 0;
}
