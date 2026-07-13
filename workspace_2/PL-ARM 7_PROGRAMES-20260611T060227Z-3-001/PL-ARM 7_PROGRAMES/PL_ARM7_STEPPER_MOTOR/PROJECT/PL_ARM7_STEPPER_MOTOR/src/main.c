#include "LPC24xx.h"
#include "stdint.h"
#include "system_init.h"
#include "glcd.h"
#include "STEPPER_MOTOR.h"

int main(void) {
    PLL_Init();        // setup PLL & clock
    system_Init();     // system init
    GLCD_Init();
    GLCD_Clear();

    GLCD_RowWriteMargin(0, "WELCOME TO AKADEMIKA");
    GLCD_RowWriteMargin(2, "      PL-ARM7       ");
    GLCD_RowWriteMargin(4, "STEPPER MOTOR EXPT  ");
    GLCD_RowWriteMargin(5, "CONNECT STEPPER     ");
    GLCD_RowWriteMargin(6, "MOTOR TO J14        ");

    M_MOTOR_EN_A_STATE;
    M_MOTOR_EN_B_STATE;
    M_MOTOR_EN_A_ON;
    M_MOTOR_EN_B_ON;

    PWM_PinConfig();
    PWM_Init();

    while(1) {
        for(int i=0;i<4;i++) {
            MotorStep(i);
            delay_cycles(10000);
        }
    }
    return 0;
}
