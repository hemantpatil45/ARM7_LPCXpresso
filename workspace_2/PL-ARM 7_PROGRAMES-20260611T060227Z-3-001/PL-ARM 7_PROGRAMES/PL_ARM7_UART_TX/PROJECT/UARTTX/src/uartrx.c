
#include "LPC24xx.h"
#include "uartrx.h"
#include "system_init.h"
#include "glcd.h"

/*==================== Local Delay ====================*/
static inline void delay_cycles(uint32_t n){ while(n--) __asm volatile("nop"); }
static void delay_ms(uint32_t ms){ while(ms--) delay_cycles(72000); }  // ~72 MHz

/*==================== UART0 (Polling) ====================*/
static void uart0_init(uint32_t pclk_hz, uint32_t baud)
{
    (void)pclk_hz; (void)baud;
    PCONP |= (1u << 3);                     // Power UART0
    PCLKSEL0 &= ~(3u << 6);                 // PCLK = CCLK/4

    // Pin select for TXD0/RXD0 (P0.2/P0.3)
    PINSEL0 = (PINSEL0 & ~((3u<<4)|(3u<<6))) | ((1u<<4)|(1u<<6));

    // 8-bit, no parity, 1 stop, enable DLAB
    U0LCR = 0x83;
    U0DLM = 0;
    U0DLL = 4;                              // 115200 @12 MHz
    U0FDR = (8u<<4) | 5u;
    U0LCR = 0x03;                           // clear DLAB
    U0FCR = 0x07;                           // enable FIFO
}

static inline int  uart0_rx_ready(void){ return (U0LSR & 1u); }
static inline char uart0_getc(void){ while(!uart0_rx_ready()){} return (char)U0RBR; }
static inline void uart0_putc(char c){ while(!(U0LSR & (1u<<5))){} U0THR = c; }
static void uart0_puts(const char* s){ while(*s) uart0_putc(*s++); }

/*==================== Switch Helper ====================*/
static inline uint8_t sw_pressed(uint32_t state){ return (state == 0) ? 1u : 0u; }
static uint8_t debounce(uint32_t state){ if(!sw_pressed(state)) return 0; delay_ms(5); return sw_pressed(state); }

/*==================== Initialize ====================*/
void UARTRX_Init(uint32_t pclk_hz, uint32_t baud)
{
    uart0_init(pclk_hz, baud);

    GLCD_Clear();
    GLCD_RowWriteMargin(0, "WELCOME TO AKADEMIKA");
    GLCD_RowWriteMargin(2, "      PL-ARM7       ");
    GLCD_RowWriteMargin(4, "UART SWITCH         ");
    GLCD_RowWriteMargin(6, "UART TRANSMIT       ");

    uart0_puts("\r\n=== UART0 READY (115200bps) ===\r\n");
    uart0_puts("SW0, SW1, SW2, SW3\r\n");
}

/*==================== Task Loop ====================*/
void UARTRX_Task(void)
{
    static uint8_t sw0_lat=0, sw1_lat=0, sw2_lat=0, sw3_lat=0;

    /* Echo back any received UART data */
    if (uart0_rx_ready()) {
        char c = uart0_getc();
        uart0_putc(c);
    }

    if (!sw0_lat && debounce(Switch0_STATE==0)) {
        sw0_lat = 1;
        GLCD_RowWriteMargin(5, "SW0: HELLO FROM LPC ");
        uart0_puts("\r\nHELLO FROM LPC2478\r\n");
    } else if (sw0_lat && (Switch0_STATE!=0)) sw0_lat = 0;

    /* -------- SW2 (P4.27) : AKADEMIKA  PL-ARM EXPT -------- */
    if (!sw2_lat && debounce(Switch2_STATE==0)) {
        sw2_lat = 1;
        GLCD_RowWriteMargin(5, "SW2: AKADEMIKA EXPT ");
        uart0_puts("\r\nAKADEMIKA  PL-ARM EXPT\r\n");
    } else if (sw2_lat && (Switch2_STATE!=0)) sw2_lat = 0;

    /* -------- SW3 (P4.31) : UARTTX DATA -------- */
    if (!sw3_lat && debounce(Switch3_STATE==0)) {
        sw3_lat = 1;
        GLCD_RowWriteMargin(5, "SW3: UARTTX DATA    ");
        uart0_puts("\r\nUARTTX DATA\r\n");
    } else if (sw3_lat && (Switch3_STATE!=0)) sw3_lat = 0;

    /* -------- SW4 (P4.26) : DATA TRANSMIT SUCCESFULLY -------- */
    if (!sw1_lat && debounce(Switch1_STATE==0)) {
        sw1_lat = 1;
        GLCD_RowWriteMargin(5, "SW4: DATA TX OK     ");
        uart0_puts("\r\nDATA TRANSMIT  SUCCESFULLY\r\n");
    } else if (sw1_lat && (Switch1_STATE!=0)) sw1_lat = 0;
}
