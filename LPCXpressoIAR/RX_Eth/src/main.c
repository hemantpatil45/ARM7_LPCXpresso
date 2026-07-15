#include <stdio.h>
#include "LPC24xx.h"       // Rely strictly on your provided header
#include "system_init.h"
#include "PL_ARM_7_FUN.C"  // Ensure this is updated with the macros below

// --- GCC Equivalents for IAR Intrinsics ---
static inline void __disable_interrupt(void) {
    __asm volatile ("mrs r0, cpsr \n\t orr r0, r0, #0x80 \n\t msr cpsr_c, r0");
}

static inline void __enable_interrupt(void) {
    __asm volatile ("mrs r0, cpsr \n\t bic r0, r0, #0x80 \n\t msr cpsr_c, r0");
}

// IRQ exception handler using GCC attributes
void IRQ_Handler(void) __attribute__ ((interrupt ("IRQ")));
void IRQ_Handler(void)
{
    void (*interrupt_function)();
    unsigned int vector;

    // Get interrupt vector using standard LPC24xx.h definitions
    vector = VICVectAddr;
    interrupt_function = (void(*)())vector;

    // Call vectored interrupt function
    (*interrupt_function)();

    // Clear interrupt in VIC
    VICVectAddr = 0;
}

int main(void)
{
    __disable_interrupt();                  // First disable interrupts

    sys_init_without_TFT_32bit_SDRAM();     // Call to System init function
    Init_timer0();                          // Init TIMER 0
    Init_VIC_Interrupt();
    GLCD_init();                            // Initialization of Graphics LCD
    clear_LCD();                            // Clear All garbage data from GLCD
    SDRAM_init_32BIT();
    Uart_0_init_115200_WITH_PLL();

    __enable_interrupt();                   // Enable interrupt

    VERIABLE_INIT_FUN((unsigned int)&key, (unsigned int)&INTER_ADC, (unsigned int)&AD7492_DATA);

    row_wr(0x00, "WELCOME TO AKADEMIKA");   // Display on 0th position on GLCD
    row_wr(0x02, "      PL-ARM7       ");   // Display on 2nd position on GLCD
    row_wr(0x04, "ETHERNET RX         ");   // Display on 4th position on GLCD
    row_wr(0x06, "                    ");   // Display on 6th position on GLCD

    T0TCR = 1;

    row_wr(7, "PRESS SW2 TO RESET  ");
    while(key != 0x30)
    {
    }

    EMAC_init();

    row_wr(5, "                     ");
    row_wr(6, "                     ");
    row_wr(7, "    EMAC INIT OK     ");

    delay(10000);

    row_wr(5, "SEND DATA FROM TX   ");
    row_wr(6, "OPEN HYPERTERMINAL  ");
    row_wr(7, "THEN PRESS KEY SW10 ");

    while(key != 0x32)
    {
    }

    transmit_byte(0X0D);
    transmit_byte(0X0A);

    RXDESBUFFER_1 = (unsigned char*)(RX_BUF_1[0]);
    for(int xyxyx = 0; xyxyx < 14; xyxyx++)
        RXDESBUFFER_1++;

    for(int xyxyx = 0; xyxyx <= 57; xyxyx++)
    {
        transmit_byte(*RXDESBUFFER_1);
        RXDESBUFFER_1++;
        delay(1000);
    }

    return 0;
}
