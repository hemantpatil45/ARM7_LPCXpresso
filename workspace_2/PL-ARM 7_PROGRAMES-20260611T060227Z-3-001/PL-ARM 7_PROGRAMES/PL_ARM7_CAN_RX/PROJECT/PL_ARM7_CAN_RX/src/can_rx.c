/*
 * can_rx.c
 *
 *  Created on: Oct 10, 2025
 *      Author: divya-devdhe
 */

#include <stdint.h>
#include <stdio.h>
#include "LPC24xx.h"
#include "glcd_font5x7.h"
#include "glcd.h"
#include "system_init.h"
#include "can_rx.h"

unsigned char key;
char DATA_BUFFER[32];
unsigned char rx_data[8];

//unsigned char key;
//char DATA_BUFFER[32];
//
//#define KBD_ROW1_BIT    0u
//#define KBD_COL1_BIT   25u
//#define KBD_ROW1_MASK  (1u << KBD_ROW1_BIT)
//#define KBD_COL1_MASK  (1u << KBD_COL1_BIT)
//
//#define KBD_ROW2_BIT    1u
//#define KBD_ROW2_MASK  (1u << KBD_ROW2_BIT)
//unsigned char rx_data[8];
//
//#define CAN1RDA_D1 (*(volatile uint8_t *)0xE0044028)
//#define CAN1RDA_D2 (*(volatile uint8_t *)0xE0044029)
//#define CAN1RDA_D3 (*(volatile uint8_t *)0xE004402A)
//#define CAN1RDA_D4 (*(volatile uint8_t *)0xE004402B)
//#define CAN1RDB_D5 (*(volatile uint8_t *)0xE004402C)
//#define CAN1RDB_D6 (*(volatile uint8_t *)0xE004402D)
//#define CAN1RDB_D7 (*(volatile uint8_t *)0xE004402E)
//#define CAN1RDB_D8 (*(volatile uint8_t *)0xE004402F)

void SW2_Keypad_Init(void)
{
    PINSEL4 &= ~(3u << (KBD_ROW1_BIT*2));                 /* P2.0  -> GPIO */
    PINSEL5 &= ~(3u << ((KBD_COL1_BIT-16u)*2));           /* P2.25 -> GPIO */
    FIO2DIR &= ~KBD_ROW1_MASK;    /* row input  */
    FIO2DIR |=  KBD_COL1_MASK;    /* col output */
    FIO2SET  =  KBD_COL1_MASK;
}

void SW6_Keypad_Init(void)
{
    PINSEL4 &= ~(3u << (KBD_ROW2_MASK*2));                 /* P2.0  -> GPIO */
    PINSEL5 &= ~(3u << ((KBD_COL1_BIT-16u)*2));           /* P2.25 -> GPIO */

    FIO2DIR &= ~KBD_ROW2_MASK;    /* row input  */
    FIO2DIR |=  KBD_COL1_MASK;    /* col output */

    FIO2SET  =  KBD_COL1_MASK;
}

uint32_t SW2_Pressed(void)
{
    FIO2CLR = KBD_COL1_MASK;      /* scan: drive low */
    delay_us(200);              /* settle (not debounce) */
    uint32_t pressed = ((FIO2PIN & KBD_ROW1_MASK) == 0u);
    FIO2SET = KBD_COL1_MASK;      /* release column */
    return pressed;
}

uint32_t SW6_Pressed(void)
{
    FIO2CLR = KBD_COL1_MASK;      /* scan: drive low */
    delay_us(200);              /* settle (not debounce) */
    uint32_t pressed = ((FIO2PIN & KBD_ROW2_MASK) == 0u);
    FIO2SET = KBD_COL1_MASK;      /* release column */
    return pressed;
}

void CAN_CONTROLLER_NORMAL_INIT(void) {
    PCONP |= (1 << 13);       // Enable CAN1 power
    CAN1MOD = (1 << 0);           // Reset CAN1 (Reset mode)
    CAN1MOD &= ~(1 << 1);
    CAN1MOD &= ~(1 << 2);
    CAN1MOD &= ~(1 << 3);
    CAN1MOD &= ~(1 << 4);
    CAN1MOD &= ~(1 << 5);
    CAN1MOD &= ~(1 << 7);
    CAN1MOD &= ~(1 << 0);     // Exit reset mode, enable CAN1
    PINSEL0 &= ~((3 << 0) | (3 << 2));   // Clear bits for P0.0 and P0.1
    PINSEL0 |= (1 << 0) | (1 << 2);      // Set P0.0 and P0.1 to CAN1 RxD and TxD function respectively
}

void Timer0_irq(void) __attribute__((interrupt("IRQ")));
volatile unsigned int timer_cnt = 0;
void Timer0_irq(void)
{
	T0IR = 0x01;
	timer_cnt++;
}

void Timer0_init()
{
	T0TCR = 0; // Disable timer 0.
	T0PR = 0x0; // Prescaler is set to no division.
	T0MR0 = 6000; // Timer intervel
	T0MCR = 0x03;	// Interrupt and reset on MR0 match
	T0CCR = 0; // Capture is disabled.
	T0EMR = 0; // No external match output.
	VICVectAddr4 = (unsigned long)Timer0_irq; // Set ISR address
	VICVectCntl4 = 0x20 | 4;	//Enable slot and assign to timer 0
	VICIntEnable = (1 << 4);	//Enable timer 0 interrupt
    T0TCR=0x00;         // diable timer 0
    T0IR = 0x01;
}

void timer_delay(unsigned int time)
{
	T0IR = 0x01;
	T0TCR = 0x01;	// Enable timer0
	timer_cnt = 0x00;
	while(timer_cnt != time)
	{
		timer_cnt++;
	}
    T0TCR=0x00;         // diable timer 0
}
