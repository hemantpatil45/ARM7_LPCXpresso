#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "LPC24xx.h"
#include "system_init.h"
#include "ethernet.h"
#include "ethernet_rx.h"
#include "ethernet_reg.h"

/* Global Shared Hardware Variables */
volatile unsigned char key = 0xFF;
unsigned int INTER_ADC;
unsigned int AD7492_DATA;

/**************************************************************************
* Function Name : IRQ_Handler
* Attribute     : __attribute__((interrupt("IRQ")))
* Description   : Dynamic vector router. Under GCC, it saves context,
* executes the registered ISR, and restores the CPU state.
***************************************************************************/
void IRQ_Handler(void) __attribute__((interrupt("IRQ")));

void IRQ_Handler(void)
{
    void (*interrupt_function)(void);
    unsigned int vector;

    vector = VICADDRESS;
    interrupt_function = (void(*)(void))vector;

    if (interrupt_function != NULL) {
        (*interrupt_function)();
    }

    VICADDRESS = 0; // Complete service cycle
}

/**************************************************************************
* Function Name : Init_VIC_Interrupt
* Description   : Mounts the keyboard scan subroutine directly into the VIC
***************************************************************************/
void Init_VIC_Interrupt(void)
{
    VICPROTECTION = 0;
    VICINTENCLEAR = 0xFFFFFFFF;
    VICADDRESS = 0;

    /* Map Timer 0 interrupt flag to route through standard IRQ */
    VICINTSELECT &= ~VIC_TIMER0_bit;

    /* Direct Keyboard Scan to Slot 4 within the exception lookup table */
    VICVECTADDR4 = (unsigned int)&kbd_scan;

    /* Prioritize Timer 0 as critical (Priority 0 is highest) */
    VICVECTPRIORITY4 = 0;

    VICINTENABLE |= VIC_TIMER0_bit;
}

int main(void)
{
    __disable_interrupt();                      // Clear Global Interrupts

    sys_init_without_TFT_32bit_SDRAM();         // Clock Initialization
    Init_timer0();                              // Timer0 configuration
    Init_VIC_Interrupt();                       // Exception setup
    GLCD_init();                                // Display setup
    clear_LCD();
    SDRAM_init_32BIT();                         // External SRAM bus configuration
    Uart_0_init_115200_WITH_PLL();              // Local UART setup

    __enable_interrupt();                       // Enable Interrupts

    /* Connect variables to lower peripheral driver layers */
    VERIABLE_INIT_FUN((unsigned int)&key, (unsigned int)&INTER_ADC, (unsigned int)&AD7492_DATA);

    row_wr(0x00, "WELCOME TO AKADEMIKA");
    row_wr(0x02, "      PL-ARM7       ");
    row_wr(0x04, "ETHERNET RX         ");
    row_wr(0x06, "                    ");

    T0TCR = 1;                                  // Trigger Timer 0
    row_wr(7, "PRESS SW2 TO RESET  ");

    while(key != 0x30);                         // Wait for SW2 press

    EMAC_init();                                // Fire up MAC and physical transceivers

    row_wr(5, "                     ");
    row_wr(6, "                     ");
    row_wr(7, "    EMAC INIT OK     ");

    delay_us(1000000);                          // 1 Second Delay

    row_wr(5, "SEND DATA FROM TX   ");
    row_wr(6, "OPEN HYPERTERMINAL  ");
    row_wr(7, "THEN PRESS KEY SW10 ");

    while(key != 0x32);                         // Wait for SW10 press

    /* Extract received frame data and transmit the payload out via UART0 */
    transmit_ethernet_payload();

    while(1) {
        // Master Loop Execution Frame
    }

    return 0;
}
