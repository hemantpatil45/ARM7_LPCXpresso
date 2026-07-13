/*

* STEPPER_MOTOR.c
 *
 *  Created on: Sep 30, 2025
 *      Author: divya-devdhe
 */

#include "LPC24xx.h"
#include "stdint.h"
#include "system_init.h"
#include "glcd.h"
#include "STEPPER_MOTOR.h"

/* Utility function: Delay */
//void delay_cycles(uint32_t cycles){ while(cycles--) __asm__("nop"); }

/* Configure PWM pins using PINSEL2 */
void PWM_PinConfig(void) {
    // P1[2]=PWM0[1], P1[3]=PWM0[2], P1[5]=PWM0[3], P1[6]=PWM0[4]
    PINSEL2 |= (0x3 << 4)   // P1.2 PWM0[1]
            | (0x3 << 6)    // P1.3 PWM0[2]
            | (0x3 << 10)   // P1.5 PWM0[3]
            | (0x3 << 12);  // P1.6 PWM0[4]
}

/* Initialize PWM peripheral */
void PWM_Init(void) {
    PWM0TCR = 0x02; // Reset PWM TC and PR
    PWM0PR = 0x0; // No prescaler, use PCLK as is

    // Set PWM period
    PWM0MR0 = 10000; // Period (adjust as needed)

    // Set initial duty cycles for motor phases
    PWM0MR1 = 0;     // Phase A
    PWM0MR2 = 0;     // Phase B
    PWM0MR3 = 0;     // Phase C
    PWM0MR4 = 0;     // Phase D

    // Reset on MR0 match
    PWM0MCR = (1 << 1);

    // Enable PWM output on channels 1 to 4
    PWM0PCR = (1 << 9) | (1 << 10) | (1 << 11) | (1 << 12);

    // Enable latch for MR0-MR4
    PWM0LER = (1 << 0) | (1 << 1) | (1 << 2) | (1 << 3) | (1 << 4);

    // Start counters and enable PWM mode
    PWM0TCR = (1 << 0) | (1 << 3);
}

/* Set active phase (one coil at a time) */
void MotorStep(uint8_t step){
    switch(step) {
        case 0: // IN1 (Phase A)
            PWM0MR1 = 5000; // Duty cycle (ON)
            PWM0MR2 = 0;
            PWM0MR3 = 0;
            PWM0MR4 = 0;
            break;
        case 1: // IN2 (Phase B)
            PWM0MR1 = 0;
            PWM0MR2 = 5000;
            PWM0MR3 = 0;
            PWM0MR4 = 0;
            break;
        case 2: // IN3 (Phase C)
            PWM0MR1 = 0;
            PWM0MR2 = 0;
            PWM0MR3 = 5000;
            PWM0MR4 = 0;
            break;
        case 3: // IN4 (Phase D)
            PWM0MR1 = 0;
            PWM0MR2 = 0;
            PWM0MR3 = 0;
            PWM0MR4 = 5000;
            break;
    }
    PWM0LER = (1 << 1) | (1 << 2) | (1 << 3) | (1 << 4);
}
