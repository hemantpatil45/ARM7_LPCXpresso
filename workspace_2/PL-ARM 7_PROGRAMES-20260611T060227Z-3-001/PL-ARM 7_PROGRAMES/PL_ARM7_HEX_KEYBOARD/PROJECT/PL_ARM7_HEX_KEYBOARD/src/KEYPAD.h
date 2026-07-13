/*
 * KEYPAD.h
 *
 *  Created on: Sep 30, 2025
 *      Author: divya-devdhe
 */

#ifndef KEYPAD_H_
#define KEYPAD_H_
// KEYPAD.h
extern char key;

// KEYPAD.h
extern char keymap[4][4];
void keypad_init(void);
void columns_all_high(void);
void drive_column_low(uint8_t col);
uint8_t read_rows_pressed(void);
uint8_t first_set4(uint8_t m);
char keypad_scan_once(void);

#endif /* KEYPAD_H_ */
