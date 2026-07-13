/*
 * glcd.c
 *
 *  Created on: 29-Sep-2025
 *      Author: ADMIN
 */
#include "LPC24xx.h"
#include <stdint.h>
#include "system_init.h"         /* your pin macros (uses FIOx) */
#include "glcd.h"
#include "glcd_font5x7.h"        /* extern const uint8_t font5x7[96][5]; */
#include "KEYPAD.h"

/* ===== timing ===== */
#ifndef GLCD_CPU_MHZ
#define GLCD_CPU_MHZ 72u
#endif

//void delay_cycles(uint32_t n){ while(n--) __asm__("nop"); }
//void delay_us(uint32_t us){ volatile uint32_t n = us * GLCD_CPU_MHZ; while(n--) __asm__("nop"); }

/* ===== data bus bit masks (per your wiring) =====
 * D0..D2 on P0.7..P0.9, D3..D7 on P1.20..P1.25
 */
#define DB_P0_MASK   ((1u<<7)|(1u<<8)|(1u<<9))
#define DB_P1_MASK   ((1u<<20)|(1u<<21)|(1u<<23)|(1u<<24)|(1u<<25))

/* ===== KS0108 commands ===== */
#define CMD_DISPLAY_ON    0x3F
#define CMD_DISPLAY_OFF   0x3E
#define CMD_SET_Y         0x40  /* + 0..63 column (inside chip) */
#define CMD_SET_X         0xB8  /* + 0..7  page */
#define CMD_START_LINE    0xC0  /* + 0..63 first scanline */

/* ===== chip selection helpers (active-HIGH CS) ===== */
static inline void cs_none(void)  { M_TFT_B4LCD_CS1_OFF; M_TFT_B6LCD_CS2_OFF; }
static inline void cs_left(void)  { M_TFT_B6LCD_CS2_OFF; M_TFT_B4LCD_CS1_ON;  }
static inline void cs_right(void) { M_TFT_B4LCD_CS1_OFF; M_TFT_B6LCD_CS2_ON;  }
static inline void cs_both(void)  { M_TFT_B4LCD_CS1_ON;  M_TFT_B6LCD_CS2_ON;  }

/* ===== low-level: put 8-bit on data bus ===== */
static inline void glcd_set_data(uint8_t d)
{
    /* clear old bits */
    FIO0CLR = DB_P0_MASK;
    FIO1CLR = DB_P1_MASK;

    /* D0..D2 -> P0.7..P0.9 */
    if (d & 0x01) FIO0SET = (1u<<7);
    if (d & 0x02) FIO0SET = (1u<<8);
    if (d & 0x04) FIO0SET = (1u<<9);

    /* D3..D7 -> P1.20..P1.25 */
    if (d & 0x08) FIO1SET = (1u<<20);
    if (d & 0x10) FIO1SET = (1u<<21);
    if (d & 0x20) FIO1SET = (1u<<23);
    if (d & 0x40) FIO1SET = (1u<<24);
    if (d & 0x80) FIO1SET = (1u<<25);
}

/* ===== pulse E (write latch) ===== */
static inline void glcd_enable_pulse(void)
{
    M_TFT_G0LCD_E_ON;               /* E = 1 */
    delay_cycles(80);
    M_TFT_G0LCD_E_OFF;              /* E = 0 */
    delay_cycles(80);
}

/* ===== write a byte (cmd/data) to selected chip(s) =====
 * rs=0 -> command, rs=1 -> data
 */
static void glcd_write(uint8_t rs, uint8_t data, uint8_t to_left, uint8_t to_right)
{
    /* RS */
    if (rs) {
        M_TFT_R0LCD_RS_ON;
    } else {
        M_TFT_R0LCD_RS_OFF;
    }

    /* RW = 0 (write) */
    M_TFT_R1LCD_RW_OFF;

    glcd_set_data(data);

    if (to_left) {
        cs_left();
        glcd_enable_pulse();
    }
    if (to_right) {
        cs_right();
        glcd_enable_pulse();
    }
    cs_none();
}

/* ==== public: initialize KS0108 ==== */
void GLCD_Init(void)
{
    /* Hardware reset: low -> high */
    M_TFT_B7LCD_RST_OFF;  /* RST=0 */
    delay_us(2000);
    M_TFT_B7LCD_RST_ON;   /* RST=1 */
    delay_us(2000);

    cs_both();
    /* Display ON, origin (0,0), start line 0 */
    glcd_write(0, CMD_DISPLAY_ON, 1, 1);
    glcd_write(0, CMD_SET_Y | 0,  1, 1);
    glcd_write(0, CMD_SET_X | 0,  1, 1);
    glcd_write(0, CMD_START_LINE | 0, 1, 1);
    cs_none();
}

/* ==== public: clear whole screen ==== */
void GLCD_Clear(void)
{
    for (uint8_t page = 0; page < GLCD_PAGES; ++page)
    {
        /* left half */
        glcd_write(0, (uint8_t)(CMD_SET_X | (page & 7)), 1, 0);
        glcd_write(0, (uint8_t)(CMD_SET_Y | 0),           1, 0);
        for (uint8_t x = 0; x < 64; ++x) glcd_write(1, 0x00, 1, 0);

        /* right half */
        glcd_write(0, (uint8_t)(CMD_SET_X | (page & 7)), 0, 1);
        glcd_write(0, (uint8_t)(CMD_SET_Y | 0),           0, 1);
        for (uint8_t x = 0; x < 64; ++x) glcd_write(1, 0x00, 0, 1);
    }
}

/* ==== set XY (logical x: 0..127, page: 0..7) ==== */
void GLCD_SetXY(uint8_t x, uint8_t page)
{
    if (x < 64) {
        glcd_write(0, (uint8_t)(CMD_SET_X | (page & 7)), 1, 0);
        glcd_write(0, (uint8_t)(CMD_SET_Y | (x & 63)),   1, 0);
    } else {
        uint8_t y = (uint8_t)(x - 64);
        glcd_write(0, (uint8_t)(CMD_SET_X | (page & 7)), 0, 1);
        glcd_write(0, (uint8_t)(CMD_SET_Y | (y & 63)),   0, 1);
    }
}

/* ==== draw one 5x7 char + 1px space ==== */
/* ==== draw one 5x7 char ==== */
void GLCD_PutChar(uint8_t x, uint8_t page, char c) {
    if ((uint8_t)c < 32 || (uint8_t)c > 127) c = ' ';
    const uint8_t* g = font5x7[(uint8_t)c - 32];

    if (x < 64) {
        glcd_write(0, (uint8_t)(CMD_SET_X | (page & 7)), 1, 0);
        glcd_write(0, (uint8_t)(CMD_SET_Y | (x & 63)),   1, 0);
        for (int i=0; i<6; i++) glcd_write(1, g[i], 1, 0);
        // optional spacing: glcd_write(1, 0x00, 1, 0);
    } else {
        uint8_t y = (uint8_t)(x - 64);
        glcd_write(0, (uint8_t)(CMD_SET_X | (page & 7)), 0, 1);
        glcd_write(0, (uint8_t)(CMD_SET_Y | (y & 63)),   0, 1);
        for (int i=0; i<6; i++) glcd_write(1, g[i], 0, 1);
        // optional spacing: glcd_write(1, 0x00, 0, 1);
    }
}

/* ==== draw string starting at (x,page) ==== */
void GLCD_PutString(uint8_t x, uint8_t page, const char* s) {
    uint8_t cx = x;
    while (*s) {
        GLCD_PutChar(cx, page, *s++);
        cx = (uint8_t)(cx + 6);   // 5px char + 1px space
        if (cx >= GLCD_WIDTH) break;
    }
}


/* ==== write string with 4px left/right margin ==== */
void GLCD_RowWriteMargin(uint8_t page, const char* s)
{
    uint8_t cx = 4;      // start drawing after left 4px
    uint8_t count = 0;

    /* --- clear first 4 pixel columns --- */
    GLCD_SetXY(0, page);
    for (uint8_t i=0; i<4; i++) {
        glcd_write(1, 0x00, 1, 0);  // if x<64 → left chip
    }

    /* --- print string (max chars that fit before last 4px) --- */
    while (*s && cx <= (GLCD_WIDTH - 8))  // stop before last 4 px
    {
        GLCD_PutChar(cx, page, *s++);
        cx = (uint8_t)(cx + 6);   // 5px font + 1px space
        count++;
        if (count >= 20) break;   // optional hard limit
    }

    /* --- clear last 4 pixel columns --- */
    GLCD_SetXY(GLCD_WIDTH - 4, page);
    for (uint8_t i=0; i<4; i++) {
        glcd_write(1, 0x00, 0, 1);  // right chip
    }
}
