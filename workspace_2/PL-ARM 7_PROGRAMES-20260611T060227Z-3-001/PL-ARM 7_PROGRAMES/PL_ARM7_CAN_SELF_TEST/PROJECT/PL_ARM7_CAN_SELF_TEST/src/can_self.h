
#include <stdint.h>
#include <stdio.h>
#include "LPC24xx.h"
#include "glcd.h"
#include "system_init.h"

#ifndef CAN_SELF_H_
#define CAN_SELF_H_

void SW2_Keypad_Init(void);
void SW6_Keypad_Init(void);
uint32_t SW2_Pressed(void);
uint32_t SW6_Pressed(void);
void CAN_CONTROLLER_SELF_TEST_INIT(void);
void CAN1_TX1_frame(unsigned short CAN_11_ID, unsigned char CAN_DATA[]);
void delay(volatile uint32_t count);
void Timer0_irq(void);
void Timer0_init();
void timer_delay(unsigned int time);

#endif /* CAN_SELF_H_ */
