void Init_VIC_Interrupt()
{
  VICProtection = 0;              // Setup interrupt controller.
  VICIntEnClr = 0xffffffff;       // Disable all interrupts. Clear bits in VICIntEnable.
  VICVectAddr = 0;                // Clear address.

  // Set the Interrupt Sources as IRQ (Timer 0 is Channel 4 in LPC24xx)
  VICIntSelect &= ~(1 << 4);      // IRQ on timer 0 line.

  // Set the address of the ISR.
  VICVectAddr4 = (unsigned int)&kbd_scan;

  // Set the IRQ priority for the interrupt.
  VICVectPriority4 = 0;           // Timer 0 Priority

  // Enable the Interrupts for respective sources.
  VICIntEnable |= (1 << 4);       // Enable timer 0 interrupt.
}
//************************* RELAY & BUZZER CONTROL lines ************************
#define RELAY_BUZZER_ON  (FIO0SET = (1 << 10))     // RELAY BUZZER ON.
#define RELAY_BUZZER_0FF (FIO0CLR = (1 << 10))     // RELAY BUZZER OFF.

//************************* STEPPER MOTOR CONTROL lines ************************
#define STEPPER_MOTOR_PHASE_A_1  (FIO1SET = (1 << 2))
#define STEPPER_MOTOR_PHASE_A_0  (FIO1CLR = (1 << 2))

#define STEPPER_MOTOR_PHASE_B_1  (FIO1SET = (1 << 3))
#define STEPPER_MOTOR_PHASE_B_0  (FIO1CLR = (1 << 3))

#define STEPPER_MOTOR_PHASE_C_1  (FIO1SET = (1 << 5))
#define STEPPER_MOTOR_PHASE_C_0  (FIO1CLR = (1 << 5))

#define STEPPER_MOTOR_PHASE_D_1  (FIO1SET = (1 << 6))
#define STEPPER_MOTOR_PHASE_D_0  (FIO1CLR = (1 << 6))

#define STEPPER_MOTOR_PHASE_A_EN_1  (FIO0SET = (1 << 21))
#define STEPPER_MOTOR_PHASE_A_EN_0  (FIO0CLR = (1 << 21))

#define STEPPER_MOTOR_PHASE_B_EN_1  (FIO0SET = (1 << 24))
#define STEPPER_MOTOR_PHASE_B_EN_0  (FIO0CLR = (1 << 24))

//*************************** OUTPUT LEDs lines ********************************
#define LED0_OFF  (FIO2SET = (1 << 19))
#define LED0_ON   (FIO2CLR = (1 << 19))

#define LED1_OFF  (FIO2SET = (1 << 21))
#define LED1_ON   (FIO2CLR = (1 << 21))

#define LED2_OFF  (FIO1SET = (1 << 13))
#define LED2_ON   (FIO1CLR = (1 << 13))

#define LED3_OFF  (FIO2SET = (1 << 15))
#define LED3_ON   (FIO2CLR = (1 << 15))

//*************************** Input switches lines ********************************
// Read exact pins using standard bitwise AND masking
#define SWITCH0 (FIO4PIN & (1 << 23))
#define SWITCH1 (FIO4PIN & (1 << 26))
#define SWITCH2 (FIO4PIN & (1 << 27))
#define SWITCH3 (FIO4PIN & (1 << 31))

//************************* ADC CONTROL lines ************************
#define ADC_CONVST_1  (FIO3SET = (1 << 28))
#define ADC_CONVST_0  (FIO3CLR = (1 << 28))

#define ADC_CS_1  (FIO3SET = (1 << 29))
#define ADC_CS_0  (FIO3CLR = (1 << 29))

#define ADC_RDY_1  (FIO3SET = (1 << 30))
#define ADC_RDY_0  (FIO3CLR = (1 << 30))

#define ADC_BUSY (FIO3PIN & (1 << 31))

//************************* DAC CONTROL lines ************************
#define DAC_CLR_1  (FIO1SET = (1 << 7))
#define DAC_CLR_0  (FIO1CLR = (1 << 7))

#define DAC_CS_1  (FIO2SET = (1 << 14))
#define DAC_CS_0  (FIO2CLR = (1 << 14))

#define DAC_LD_1  (FIO0SET = (1 << 25))
#define DAC_LD_0  (FIO0CLR = (1 << 25))
