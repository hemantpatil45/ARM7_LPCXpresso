#ifndef CAN_TX_H_
#define CAN_TX_H_

#include <stdint.h>
#include "LPC24xx.h"
#include "glcd.h"
#include "system_init.h"
#include "CAN_regs.h"

#define CAN1CMR_STB1 (*(volatile uint8_t *)0xE0044009)
#define CAN1CMR_STB2 (*(volatile uint8_t *)0xE004400A)
#define CAN1CMR_STB3 (*(volatile uint8_t *)0xE004400B)
#define CAN1CMR_TR (*(volatile uint8_t *)0xE0044004)

#define CAN1AFMR (*(volatile uint32_t *)(0xE0044000UL))
#define CAN1GSR_TCS (*(volatile uint32_t *)(0xE004400B))

#define CAN1TDA1_D1 (*(volatile uint8_t *)0xE0044038)
#define CAN1TDA1_D2 (*(volatile uint8_t *)0xE0044039)
#define CAN1TDA1_D3 (*(volatile uint8_t *)0xE004403A)
#define CAN1TDA1_D4 (*(volatile uint8_t *)0xE004403B)
#define CAN1TDB1_D5 (*(volatile uint8_t *)0xE004403C)
#define CAN1TDB1_D6 (*(volatile uint8_t *)0xE004403D)
#define CAN1TDB1_D7 (*(volatile uint8_t *)0xE004403E)
#define CAN1TDB1_D8 (*(volatile uint8_t *)0xE004403F)

#define CAN1TFI1_PRIO (*(volatile uint8_t *)0xE0044030)
#define CAN1TFI1_DLC (*(volatile uint8_t *)0xE0044031)
#define CAN1TFI1_RTR  (*(volatile uint8_t *)0xE0044032)
#define CAN1TFI1_FF   (*(volatile uint8_t *)0xE0044033)

#define KBD_ROW1_BIT    0u
#define KBD_COL1_BIT   25u
#define KBD_ROW1_MASK  (1u << KBD_ROW1_BIT)
#define KBD_COL1_MASK  (1u << KBD_COL1_BIT)

#define KBD_ROW2_BIT    1u
#define KBD_ROW2_MASK  (1u << KBD_ROW2_BIT)

void SW2_Keypad_Init(void);
void SW6_Keypad_Init(void);
uint32_t SW2_Pressed(void);
uint32_t SW6_Pressed(void);
void CAN_CONTROLLER_NORMAL_INIT(void);
void CAN1_TX1_frame(unsigned short CAN_11bit_ID, unsigned char CAN_DATA_frame[]);
void Timer0_irq(void);
void Timer0_init();
void timer_delay(unsigned int time);
void CAN_Pins_Init(void);

#endif
