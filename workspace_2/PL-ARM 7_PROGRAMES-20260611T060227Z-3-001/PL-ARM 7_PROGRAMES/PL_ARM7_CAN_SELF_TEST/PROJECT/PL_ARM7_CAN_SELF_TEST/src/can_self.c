
#include <stdint.h>
#include <stdio.h>
#include "LPC24xx.h"
#include "glcd.h"
#include "system_init.h"
#include "can_self.h"

#define KBD_ROW1_BIT    0u
#define KBD_COL1_BIT   25u
#define KBD_ROW1_MASK  (1u << KBD_ROW1_BIT)
#define KBD_COL1_MASK  (1u << KBD_COL1_BIT)

#define KBD_ROW2_BIT    1u
#define KBD_ROW2_MASK  (1u << KBD_ROW2_BIT)
unsigned char rx_data[8];

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

void CAN_CONTROLLER_SELF_TEST_INIT(void) {
    PCONP |= (1 << 13);       // Enable CAN1 power
    CAN1MOD = (1 << 0);           // Reset CAN1 (Reset mode)
    CAN1MOD &= ~(1 << 1);
    CAN1MOD &= ~(1 << 2);
    CAN1MOD &= ~(1 << 3);
    CAN1MOD &= ~(1 << 4);
    CAN1MOD &= ~(1 << 5);
    CAN1MOD &= ~(1 << 7);
    CAN1MOD &= ~(1 << 0);     // Exit reset mode, enable CAN1
}

void CAN1_TX1_frame(unsigned short CAN_11bit_ID, unsigned char CAN_DATA_frame[]) {
    CAN1TFI1 = (8 << 16) | 1;            // DLC=8, priority=1
    CAN1TID1 = (CAN_11bit_ID & 0x7FF);      // Standard 11-bit ID
    CAN1TDA1 = CAN_DATA_frame[0] | (CAN_DATA_frame[1] << 8) | (CAN_DATA_frame[2] << 16) | (CAN_DATA_frame[3] << 24);
    CAN1TDB1 = CAN_DATA_frame[4] | (CAN_DATA_frame[5] << 8) | (CAN_DATA_frame[6] << 16) | (CAN_DATA_frame[7] << 24);
    CAN1CMR = 1;                         // Request Transmission (TR=1)
}

void delay(volatile uint32_t count) {
    while (count--) __asm__("nop");
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
