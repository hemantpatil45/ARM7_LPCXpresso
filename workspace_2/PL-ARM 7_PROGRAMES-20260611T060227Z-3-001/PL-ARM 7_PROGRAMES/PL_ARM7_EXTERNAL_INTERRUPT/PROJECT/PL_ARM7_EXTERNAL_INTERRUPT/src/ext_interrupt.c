#include <stdint.h>
#include "LPC24xx.h"
#include "system_init.h"   /* uses LED2_ON/LED2_OFF, optional macros */
#include "ext_interrupt.h"

/* --- Local masks (for readback / DIR only) --- */
#define LED2_P2_21_MASK   (1u << 21)
#define EINT0_P2_10_MASK  (1u << 10)

/* --------- IRQ handler (ARM7 GCC attribute) --------- */
void EINT0_IRQHandler(void) __attribute__((interrupt("IRQ")));
void EINT0_IRQHandler(void)
{
    /* Toggle LED2 (active-LOW) by reading current pin state */
    if (FIO2PIN & LED2_P2_21_MASK) {
        LED2_ON;   /* drive low -> ON */
    } else {
        LED2_OFF;  /* drive high -> OFF */
    }

    /* Clear external interrupt flag and ack VIC */
    EXTINT = (1u << 0);      /* write 1 to clear EINT0 */
    VICVectAddr = 0x00;      /* end of interrupt */
}

/* --------- Public init ---------
 * - P2.21 -> GPIO output, OFF (active-LOW)
 * - P2.10 -> EINT0 function (PINSEL4[21:20]=01), input, falling edge
 * - VIC slot 14 configured for EINT0
 */
void EINT0_Init(void)
{
    /* ================= LED2 (P2.21) =================
       Set P2.21 to GPIO (PINSEL5[11:10]=00), output, OFF (drive HIGH)
    */
    PINSEL5 &= ~(3UL << 10);         /* P2.21 function = 00 (GPIO) */
    FIO2DIR |=  LED2_P2_21_MASK;     /* output */
    LED2_OFF;                        /* active-LOW OFF = drive HIGH */

    /* ================= EINT0 (P2.10) =================
       P2.10 -> EINT0: PINSEL4[21:20] = 01
       Also make it input and (optionally) enable pull-up.
    */
    PINSEL4 &= ~(3UL << 20);
    PINSEL4 |=  (1UL << 20);         /* select EINT0 function */

    FIO2DIR &= ~EINT0_P2_10_MASK;    /* ensure input direction */

    /* Optional but recommended: pull-up on P2.10 so falling edge = button press */
    PINMODE4 &= ~(3UL << 20);        /* 00 = pull-up */

    /* If you want to reflect the pin idle/drive using your macros, you *may*
       leave it alone since pin is now in alternate function (EINT0). These
       macros affect FIO state only; they don't change the PINSEL function. */
    /* M_ISPM_EINT0_ON;  // NOT required; kept commented intentionally */

    /* ============ EINT0 edge/polarity and flags ============ */
    EXTMODE  |=  (1u << 0);          /* edge-sensitive */
    EXTPOLAR &= ~(1u << 0);          /* falling edge */

    EXTINT = (1u << 0);              /* clear pending flag (if any) */

    /* Ensure EINT0 is routed to IRQ (not FIQ) */
    VICIntSelect &= ~(1u << 14);

    /* Vector slot 14 is EINT0 on LPC24xx */
    VICVectAddr14 = (unsigned long)EINT0_IRQHandler;
    VICVectCntl14 = 0x20 | 14;       /* enable this vector slot + source #14 */

    /* Unmask/enable EINT0 in VIC */
    VICIntEnable  = (1u << 14);
}



