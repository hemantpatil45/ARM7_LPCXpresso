#include "system_init.h"
#include "LPC24xx.h"
#include "glcd.h"
#define EMC_BASE_ADDR   0xFFE0C000
#define LPC_EMC         ((LPC_EMC_TypeDef *) EMC_BASE_ADDR)
#define CCLK_MHZ  72u

/* --- Backward Compatibility Wrappers --- */
void sys_init_without_TFT_32bit_SDRAM(void) {
    system_Init();
}

void GLCD_init(void) {
    GLCD_Init();
}

void clear_LCD(void) {
    GLCD_Clear();
}

void row_wr(unsigned char row, char *msg) {
    GLCD_RowWriteMargin(row, msg);
}

void delay_us(uint32_t us) {
    volatile uint32_t n = us * CCLK_MHZ;
    while(n--) __asm__("nop");
}

void delay_cycles(uint32_t cycles) {
    while(cycles--) __asm__("nop");
}

/* Base initialization from LPCXpresso template */
void PLL_Init(void)
{
    SCS |= 0x20;
    while((SCS & 0x40) == 0);
    CLKSRCSEL = 0x01;
    PLLCFG = 0x0000000B;
    PLLFEED = 0xAA;
    PLLFEED = 0x55;
    PLLCON = 0x01;
    PLLFEED = 0xAA;
    PLLFEED = 0x55;
    while((PLLSTAT & (1 << 26)) == 0);
}

void system_Init(void) {
    PLL_Init();
    /* Pin configurations, I/O directions, and custom peripheral clockings */
}
void Init_timer0(void)
{
    /* Enable power to Timer 0 */
    PCONP |= (1 << 1);

    /* Set PCLK_TIMER0 = CCLK (72 MHz) */
    PCLKSEL0 &= ~(3 << 2);
    PCLKSEL0 |=  (1 << 2);

    /* Reset Timer 0 and set count controls */
    T0TCR = 0x02;
    T0CTCR = 0x00;

    /* Prescale for 1us tick (72MHz / 72 = 1MHz) */
    T0PR = 71;

    /* Match Register for 1ms interval (1000us) */
    T0MR0 = 1000;

    /* Interrupt and Reset on match */
    T0MCR = (1 << 0) | (1 << 1);

    /* Clear any pending interrupts */
    T0IR = 0xFF;

    /* Start Timer 0 */
    T0TCR = 0x01;
}
void SDRAM_init_32BIT(void)
{
    /* 1. Enable power to EMC */
    PCONP |= (1 << 11);

    /* 2. Configure Pin Select for EMC pins
     * (Ensure these PINSEL settings match your hardware schematic!) */
    PINSEL2 = 0x50150105;
    PINSEL3 = 0x00000005;

    /* 3. EMC Control Enable */
    LPC_EMC->Control = 0x01;

    /* 4. Set timing for the specific SDRAM chip being used */
    LPC_EMC->DynamicReadConfig = 0x01;
    LPC_EMC->DynamicRP         = 0x01;
    LPC_EMC->DynamicRAS        = 0x02;
    LPC_EMC->DynamicSREX       = 0x05;
    LPC_EMC->DynamicAPR        = 0x01;
    LPC_EMC->DynamicDAL        = 0x04;
    LPC_EMC->DynamicWR         = 0x01;
    LPC_EMC->DynamicRC         = 0x05;
    LPC_EMC->DynamicRFC        = 0x05;
    LPC_EMC->DynamicXSR        = 0x05;
    LPC_EMC->DynamicRRD        = 0x01;
    LPC_EMC->DynamicMRD        = 0x02;

    /* 5. Set SDRAM Configuration (Example: 32-bit SDRAM) */
    LPC_EMC->DynamicConfig0    = 0x00000580;

    /* 6. Issue SDRAM Commands
     * (NOTE: You usually need to add the sequence to issue Precharge, Refresh,
     * and Mode Register set commands here for the SDRAM to actually initialize.) */
}
