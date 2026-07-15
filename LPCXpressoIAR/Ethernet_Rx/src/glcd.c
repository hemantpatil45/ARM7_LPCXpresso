#include "ethernet_reg.h"
#include "glcd.h"
#include "glcd_font5x7.h"

#define DB_P0_MASK   ((1u << 7) | (1u << 8) | (1u << 9))
#define DB_P1_MASK   ((1u << 20) | (1u << 21) | (1u << 23) | (1u << 24) | (1u << 25))

#define CMD_DISPLAY_ON    0x3F
#define CMD_DISPLAY_OFF   0x3E
#define CMD_SET_Y         0x40
#define CMD_SET_X         0xB8
#define CMD_START_LINE    0xC0

static inline void cs_none(void)  { M_TFT_B4LCD_CS1_OFF; M_TFT_B6LCD_CS2_OFF; }
static inline void cs_left(void)  { M_TFT_B6LCD_CS2_OFF; M_TFT_B4LCD_CS1_ON;  }
static inline void cs_right(void) { M_TFT_B4LCD_CS1_OFF; M_TFT_B6LCD_CS2_ON;  }
static inline void cs_both(void)  { M_TFT_B4LCD_CS1_ON;  M_TFT_B6LCD_CS2_ON;  }

static inline void glcd_set_data(uint8_t d) {
    FIO0CLR = DB_P0_MASK;
    FIO1CLR = DB_P1_MASK;

    if (d & 0x01) FIO0SET = (1u << 7);
    if (d & 0x02) FIO0SET = (1u << 8);
    if (d & 0x04) FIO0SET = (1u << 9);
    if (d & 0x08) FIO1SET = (1u << 20);
    if (d & 0x10) FIO1SET = (1u << 21);
    if (d & 0x20) FIO1SET = (1u << 23);
    if (d & 0x40) FIO1SET = (1u << 24);
    if (d & 0x80) FIO1SET = (1u << 25);
}

static inline void glcd_enable_pulse(void) {
    M_TFT_G0LCD_E_ON;
    delay(80);
    M_TFT_G0LCD_E_OFF;
    delay(80);
}

static void glcd_write(uint8_t rs, uint8_t data_val, uint8_t to_left, uint8_t to_right) {
    if (rs) { M_TFT_R0LCD_RS_ON; } else { M_TFT_R0LCD_RS_OFF; }
    M_TFT_R1LCD_RW_OFF;
    glcd_set_data(data_val);

    if (to_left)  { cs_left();  glcd_enable_pulse(); }
    if (to_right) { cs_right(); glcd_enable_pulse(); }
    cs_none();
}

void GLCD_Init(void) {
    M_TFT_B7LCD_RST_OFF; delay(2000);
    M_TFT_B7LCD_RST_ON;  delay(2000);
    cs_both();
    glcd_write(0, CMD_DISPLAY_ON, 1, 1);
    glcd_write(0, CMD_SET_Y | 0,  1, 1);
    glcd_write(0, CMD_SET_X | 0,  1, 1);
    glcd_write(0, CMD_START_LINE | 0, 1, 1);
    cs_none();
}

void GLCD_Clear(void) {
    for (uint8_t page = 0; page < GLCD_PAGES; ++page) {
        glcd_write(0, (uint8_t)(CMD_SET_X | (page & 7)), 1, 0);
        glcd_write(0, (uint8_t)(CMD_SET_Y | 0), 1, 0);
        for (uint8_t x = 0; x < 64; ++x) glcd_write(1, 0x00, 1, 0);

        glcd_write(0, (uint8_t)(CMD_SET_X | (page & 7)), 0, 1);
        glcd_write(0, (uint8_t)(CMD_SET_Y | 0), 0, 1);
        for (uint8_t x = 0; x < 64; ++x) glcd_write(1, 0x00, 0, 1);
    }
}

void GLCD_SetXY(uint8_t x, uint8_t page) {
    if (x < 64) {
        glcd_write(0, (uint8_t)(CMD_SET_X | (page & 7)), 1, 0);
        glcd_write(0, (uint8_t)(CMD_SET_Y | (x & 63)),   1, 0);
    } else {
        uint8_t y = (uint8_t)(x - 64);
        glcd_write(0, (uint8_t)(CMD_SET_X | (page & 7)), 0, 1);
        glcd_write(0, (uint8_t)(CMD_SET_Y | (y & 63)),   0, 1);
    }
}

void GLCD_PutChar(uint8_t x, uint8_t page, char c) {
    if ((uint8_t)c < 32 || (uint8_t)c > 127) c = ' ';
    const uint8_t* g = font5x7[(uint8_t)c - 32];

    if (x < 64) {
        glcd_write(0, (uint8_t)(CMD_SET_X | (page & 7)), 1, 0);
        glcd_write(0, (uint8_t)(CMD_SET_Y | (x & 63)),   1, 0);
        for (int i = 0; i < 6; i++) glcd_write(1, g[i], 1, 0);
    } else {
        uint8_t y = (uint8_t)(x - 64);
        glcd_write(0, (uint8_t)(CMD_SET_X | (page & 7)), 0, 1);
        glcd_write(0, (uint8_t)(CMD_SET_Y | (y & 63)),   0, 1);
        for (int i = 0; i < 6; i++) glcd_write(1, g[i], 0, 1);
    }
}

void GLCD_PutString(uint8_t x, uint8_t page, const char* s) {
    uint8_t cx = x;
    while (*s) {
        GLCD_PutChar(cx, page, *s++);
        cx = (uint8_t)(cx + 6);
        if (cx >= GLCD_WIDTH) break;
    }
}

/* Backward-compatible row wrapper matching original main.c execution */
void row_wr(unsigned char row, const char msg[]) {
    char clean_msg[21];
    int i;
    for(i = 0; i < 20 && msg[i] != '\0'; i++) {
        clean_msg[i] = msg[i];
    }
    clean_msg[i] = '\0';
    GLCD_PutString(0, row, clean_msg);
}
