/*
 * flashwrite.c
 *
 *  Created on: 08-Oct-2025
 *      Author: ADMIN
 */

#include "LPC24xx.h"
#include <stdint.h>
#include "flashwrite.h"

/* ====== NOR base / word access ====== */
#ifndef NOR_BASE
#define NOR_BASE   (0x80000000UL)
#endif
#define F16(off)   (*((volatile uint16_t *)(NOR_BASE) + (off)))   /* off in WORDS */

/* ====== EMC window ====== */
#ifndef EMC_BASE_ADDR
#define EMC_BASE_ADDR (0xFFE08000UL)
#endif
#define EMCControl             (*(volatile unsigned long *)(EMC_BASE_ADDR + 0x000))
#define EMCConfig              (*(volatile unsigned long *)(EMC_BASE_ADDR + 0x008))
#define EMCStaticConfig0       (*(volatile unsigned long *)(EMC_BASE_ADDR + 0x200))
#define EMCStaticWaitWen0      (*(volatile unsigned long *)(EMC_BASE_ADDR + 0x204))
#define EMCStaticWaitOen0      (*(volatile unsigned long *)(EMC_BASE_ADDR + 0x208))
#define EMCStaticWaitRd0       (*(volatile unsigned long *)(EMC_BASE_ADDR + 0x20C))
#define EMCStaticWaitPage0     (*(volatile unsigned long *)(EMC_BASE_ADDR + 0x210))
#define EMCStaticWaitWr0       (*(volatile unsigned long *)(EMC_BASE_ADDR + 0x214))
#define EMCStaticWaitTurn0     (*(volatile unsigned long *)(EMC_BASE_ADDR + 0x218))

/* ====== tiny delays ====== */
static inline void dly_loops(volatile uint32_t n){ while(n--) __asm volatile("nop"); }
static inline void dly_ms(uint32_t ms){ while(ms--) dly_loops(72000); } /* ~1ms @~72MHz */

/* ====== keypad (Col1=P2.25, Row1=P2.0, Row2=P2.1) ====== */
#define KBD_COL1_BIT  25u
#define KBD_ROW1_BIT   0u
#define KBD_ROW2_BIT   1u
#define KBD_COL1_MASK (1u<<KBD_COL1_BIT)
#define KBD_ROW1_MASK (1u<<KBD_ROW1_BIT)
#define KBD_ROW2_MASK (1u<<KBD_ROW2_BIT)

__attribute__((weak)) void Keypad_Col1Row12_Init(void)
{
    /* P2.0, P2.1 rows as GPIO inputs */
    PINSEL4 &= ~(3u << (KBD_ROW1_BIT*2));
    PINSEL4 &= ~(3u << (KBD_ROW2_BIT*2));
    /* P2.25 column as GPIO output */
    PINSEL5 &= ~(3u << ((KBD_COL1_BIT-16u)*2));
    FIO2DIR &= ~(KBD_ROW1_MASK | KBD_ROW2_MASK);
    FIO2DIR |=  KBD_COL1_MASK;
    /* idle HIGH on column */
    FIO2SET = KBD_COL1_MASK;
}

static inline uint32_t read_rows_when_col_low(void)
{
    FIO2CLR = KBD_COL1_MASK;  /* drive column LOW */
    dly_loops(300);
    uint32_t p = FIO2PIN;
    FIO2SET = KBD_COL1_MASK;  /* release HIGH */
    return p;
}

static uint8_t debounced_low(uint32_t mask)
{
    uint8_t a = ((read_rows_when_col_low() & mask) == 0u);
    dly_ms(2);
    uint8_t b = ((read_rows_when_col_low() & mask) == 0u);
    return (a && b) ? 1u : 0u;
}

__attribute__((weak)) uint8_t SW2_Pressed(void){ return debounced_low(KBD_ROW1_MASK); }
__attribute__((weak)) uint8_t SW6_Pressed(void){ return debounced_low(KBD_ROW2_MASK); }

/* ====== EMC bring-up (Static CS0, x16) ====== */
__attribute__((weak)) void EMC_Init_CS0_x16(void)
{
    /* Power EMC */
    PCONP |= (1u << 11);

    /* Data P3.0..P3.15 -> func 01 */
    for (int b = 0; b < 32; b += 2)
        PINSEL6 = (PINSEL6 & ~(3u << b)) | (1u << b);

    /* Address P4.1..P4.22 -> func 01 */
    for (int p = 1; p <= 15; ++p)
        PINSEL8 = (PINSEL8 & ~(3u << (p*2))) | (1u << (p*2));
    for (int p = 16; p <= 22; ++p)
        PINSEL9 = (PINSEL9 & ~(3u << ((p-16)*2))) | (1u << ((p-16)*2));

    /* Control: OE=P4.24, WE=P4.25, CS0=P4.30 -> 01 */
    PINSEL9 = (PINSEL9 & ~(3u << ((24-16)*2))) | (1u << ((24-16)*2));
    PINSEL9 = (PINSEL9 & ~(3u << ((25-16)*2))) | (1u << ((25-16)*2));
    PINSEL9 = (PINSEL9 & ~(3u << ((30-16)*2))) | (1u << ((30-16)*2));

    EMCControl = 1; EMCConfig = 0;

    /* Conservative timings for ~70 ns NOR */
    EMCStaticConfig0   = (1u << 7) | 0x00000001u; /* PB=1, x16 */
    EMCStaticWaitWen0  = 2;
    EMCStaticWaitOen0  = 3;
    EMCStaticWaitRd0   = 7;
    EMCStaticWaitPage0 = 0;
    EMCStaticWaitWr0   = 7;
    EMCStaticWaitTurn0 = 3;

    dly_loops(5000);
}

/* ====== SST39VF3202C command set (x16) ====== */
#define UNLOCK1  (0x555u)  /* word offsets in x16 */
#define UNLOCK2  (0x2AAu)

__attribute__((weak)) void nor_reset(void)
{
    F16(0) = 0x00F0u;
    dly_loops(50);
}

static void wait_toggle(uint32_t woff)
{
    uint16_t a,b;
    do { a = F16(woff); b = F16(woff); } while ((a ^ b) & (1u<<6)); /* DQ6 toggles while busy */
}

int flash_sector_erase(uint32_t byte_addr)
{
    uint32_t w = (byte_addr & ~1u) >> 1;
    F16(UNLOCK1)=0x00AAu; F16(UNLOCK2)=0x0055u; F16(UNLOCK1)=0x0080u;
    F16(UNLOCK1)=0x00AAu; F16(UNLOCK2)=0x0055u; F16(w)=0x0030u;
    wait_toggle(w);
    return 0;
}

int flash_program_word(uint32_t byte_addr, uint16_t data)
{
    uint32_t w = (byte_addr & ~1u) >> 1;
    F16(UNLOCK1)=0x00AAu; F16(UNLOCK2)=0x0055u; F16(UNLOCK1)=0x00A0u;
    F16(w) = data;
    wait_toggle(w);
    return 0;
}

int flash_write_buf(uint32_t byte_addr, const uint16_t *src, uint32_t words)
{
    for (uint32_t i=0;i<words;i++){
        flash_program_word(byte_addr + (i<<1), src[i]);
    }
    return 0;
}

