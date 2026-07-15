#ifndef SYSTEM_INIT_H_
#define SYSTEM_INIT_H_

#include <stdint.h>

/* --- Toolchain Intrinsics (GCC Compatible) --- */
#define __enable_interrupt()  __asm volatile ("mrs r0, cpsr\n\t" "bic r0, r0, #0x80\n\t" "msr cpsr_c, r0" ::: "r0")
#define __disable_interrupt() __asm volatile ("mrs r0, cpsr\n\t" "orr r0, r0, #0x80\n\t" "msr cpsr_c, r0" ::: "r0")
/* ========================================================================== */
/* GLCD / TFT Control Pin Macros for system_init.h            */
/* ========================================================================== */

// RS (Register Select) on P0.4
#define M_TFT_R0LCD_RS_ON      (FIO0SET = (1 << 4))
#define M_TFT_R0LCD_RS_OFF     (FIO0CLR = (1 << 4))

// RW (Read/Write) on P0.5
#define M_TFT_R1LCD_RW_ON      (FIO0SET = (1 << 5))
#define M_TFT_R1LCD_RW_OFF     (FIO0CLR = (1 << 5))

// E (Enable) on P1.19
#define M_TFT_G0LCD_E_ON       (FIO1SET = (1 << 19))
#define M_TFT_G0LCD_E_OFF      (FIO1CLR = (1 << 19))

// CS1 (Chip Select 1 - Left) on P1.26
#define M_TFT_B4LCD_CS1_ON     (FIO1SET = (1 << 26))
#define M_TFT_B4LCD_CS1_OFF    (FIO1CLR = (1 << 26))

// CS2 (Chip Select 2 - Right) on P1.28
#define M_TFT_B6LCD_CS2_ON     (FIO1SET = (1 << 28))
#define M_TFT_B6LCD_CS2_OFF    (FIO1CLR = (1 << 28))

// RST (Reset) on P1.29
#define M_TFT_B7LCD_RST_ON     (FIO1SET = (1 << 29))
#define M_TFT_B7LCD_RST_OFF    (FIO1CLR = (1 << 29))
/* --- Backward Compatibility Mappings --- */
void sys_init_without_TFT_32bit_SDRAM(void);
void GLCD_init(void);
void clear_LCD(void);
void row_wr(unsigned char row, char *msg);
void delay_us(uint32_t us);
void delay_cycles(uint32_t cycles);

/* --- System Operations --- */
void PLL_Init(void);
void system_Init(void);
void Init_timer0(void);
void Init_VIC_Interrupt(void);
unsigned int Read_Switches(void);
void SDRAM_init_32BIT(void);
void Uart_0_init_115200_WITH_PLL(void);
void VERIABLE_INIT_FUN(unsigned int ver1, unsigned int ver2, unsigned int ver3);
void transmit_byte(unsigned char bt);
void kbd_scan(void);

#endif /* SYSTEM_INIT_H_ */
