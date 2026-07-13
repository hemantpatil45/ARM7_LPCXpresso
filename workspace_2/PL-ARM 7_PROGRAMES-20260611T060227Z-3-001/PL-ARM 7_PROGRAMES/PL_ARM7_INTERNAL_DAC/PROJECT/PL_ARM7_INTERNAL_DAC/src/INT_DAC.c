#include <stdint.h>
#include "LPC24xx.h"
#include "glcd.h"
#include "system_init.h"
#include "INT_DAC.h"

#define CCLK_HZ 72000000u
#define CCLK_MHZ 72u
#define STEP_US 10u        // Update period for DAC, microseconds

/* ===================== tiny delays ===================== */
#define CCLK_MHZ 72u
void delay_us(uint32_t us){ volatile uint32_t n = us * CCLK_MHZ; while(n--) __asm__("nop"); }
/* Compatibility wrapper so you can call delay(1000000) == 1 second */
void delay(uint32_t us){ delay_us(us); }

void dac_init(void){
  /* P0.26 -> AOUT function: PINSEL1[21:20] = 0b10 */
  PINSEL1 &= ~(3u << 20);
  PINSEL1 |=  (2u << 20);
  /* midscale start */
  DACR = ((uint32_t)DAC_BIAS << 16) | (512u << 6);
}

void dac_write(uint16_t code10){
  if(code10 > 1023u) code10 = 1023u;
  DACR = ((uint32_t)DAC_BIAS << 16) | ((uint32_t)code10 << 6);
}
/* ===================== TIMER0: 1us tick ===================== */
void timer0_init_us(void){
  PCONP |= (1u << 1);             /* power TIMER0 */
  PCLKSEL0 &= ~(3u << 2);
  PCLKSEL0 |=  (1u << 2);         /* PCLK = CCLK */
  T0PR = (CCLK_HZ / 1000000u) - 1u;
  T0MR0 = STEP_US;
  T0MCR = (1u << 0) | (1u << 1);  /* flag + reset */
  T0TCR = (1u << 1);
  T0TCR = (1u << 0);
}
void timer0_wait_match0(void){
  while ((T0IR & 1u) == 0u) { /* wait */ }
  T0IR = 1u;
}
