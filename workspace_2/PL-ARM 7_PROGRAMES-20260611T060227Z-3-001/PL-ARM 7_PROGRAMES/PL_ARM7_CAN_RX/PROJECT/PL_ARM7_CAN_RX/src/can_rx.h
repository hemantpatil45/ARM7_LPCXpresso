/*
 * can_rx.h
 *
 *  Created on: Oct 10, 2025
 *      Author: divya-devdhe
 */

#ifndef CAN_RX_H_
#define CAN_RX_H_
#include <stdint.h>
#include <stdio.h>
#include "LPC24xx.h"
#include "glcd_font5x7.h"
#include "glcd.h"
#include "system_init.h"

#define KBD_ROW1_BIT    0u
#define KBD_COL1_BIT   25u
#define KBD_ROW1_MASK  (1u << KBD_ROW1_BIT)
#define KBD_COL1_MASK  (1u << KBD_COL1_BIT)

#define KBD_ROW2_BIT    1u
#define KBD_ROW2_MASK  (1u << KBD_ROW2_BIT)

#define CAN1RDA_D1 (*(volatile uint8_t *)0xE0044028)
#define CAN1RDA_D2 (*(volatile uint8_t *)0xE0044029)
#define CAN1RDA_D3 (*(volatile uint8_t *)0xE004402A)
#define CAN1RDA_D4 (*(volatile uint8_t *)0xE004402B)
#define CAN1RDB_D5 (*(volatile uint8_t *)0xE004402C)
#define CAN1RDB_D6 (*(volatile uint8_t *)0xE004402D)
#define CAN1RDB_D7 (*(volatile uint8_t *)0xE004402E)
#define CAN1RDB_D8 (*(volatile uint8_t *)0xE004402F)

void SW2_Keypad_Init(void);
void SW6_Keypad_Init(void);
uint32_t SW2_Pressed(void);
uint32_t SW6_Pressed(void);
void CAN_CONTROLLER_NORMAL_INIT(void);
void Timer0_irq(void);
void Timer0_init();
void timer_delay(unsigned int time);

#endif /* CAN_RX_H_ */
