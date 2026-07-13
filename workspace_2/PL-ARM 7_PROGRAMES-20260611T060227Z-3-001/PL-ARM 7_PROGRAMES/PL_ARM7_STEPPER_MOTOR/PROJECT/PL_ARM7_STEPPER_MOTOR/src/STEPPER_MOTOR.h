/*
 * STEPPER_MOTOR.h
 *
 *  Created on: Sep 30, 2025
 *      Author: divya-devdhe
 */

#ifndef STEPPER_MOTOR_H_
#define STEPPER_MOTOR_H_

/* Motor control macros */
#define M_MOTOR_EN_A_STATE  FIO0DIR |= (1 << 21);
#define M_MOTOR_EN_B_STATE  FIO0DIR |= (1 << 24);
#define M_MOTOR_EN_A_ON     FIO0SET |= (1 << 21);
#define M_MOTOR_EN_B_ON     FIO0SET |= (1 << 24);

/* PWM Pins for motor */
#define PWM_PHASE_A      PWM0MR1
#define PWM_PHASE_B      PWM0MR2
#define PWM_PHASE_C      PWM0MR3
#define PWM_PHASE_D      PWM0MR4

void PWM_PinConfig(void);
void PWM_Init(void);
void MotorStep(uint8_t step);


#endif /* STEPPER_MOTOR_H_ */
