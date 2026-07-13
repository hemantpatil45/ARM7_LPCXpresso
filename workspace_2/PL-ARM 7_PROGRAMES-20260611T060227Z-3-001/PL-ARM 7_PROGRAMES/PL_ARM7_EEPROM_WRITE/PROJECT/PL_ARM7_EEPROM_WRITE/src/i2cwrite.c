/*
 * i2cwrite.c
 *
 *  Created on: 03-Oct-2025
 *      Author: ADMIN
 */


/*
 * i2cwrite.c
 * Implements AT24C512 EEPROM write using I2C0 + SW2 trigger
 */

#include "LPC24xx.h"
#include "i2cwrite.h"
#include "glcd.h"

/* ---------------- I2C registers ---------------- */
#ifndef I2C0CONSET
  #define I2C0CONSET  (*(volatile uint32_t*)0xE001C000u)
#endif
#ifndef I2C0STAT
  #define I2C0STAT    (*(volatile uint32_t*)0xE001C004u)
#endif
#ifndef I2C0DAT
  #define I2C0DAT     (*(volatile uint32_t*)0xE001C008u)
#endif
#ifndef I2C0SCLH
  #define I2C0SCLH    (*(volatile uint32_t*)0xE001C010u)
#endif
#ifndef I2C0SCLL
  #define I2C0SCLL    (*(volatile uint32_t*)0xE001C014u)
#endif
#ifndef I2C0CONCLR
  #define I2C0CONCLR  (*(volatile uint32_t*)0xE001C018u)
#endif

/* ---------------- I2C control bits ---------------- */
#define I2C_AA   0x04
#define I2C_SI   0x08
#define I2C_STO  0x10
#define I2C_STA  0x20
#define I2C_I2EN 0x40

/* ---------------- Keypad SW2 pins ----------------
 * SW2 = Row1/Col1
 * Row1 → P2.0  (input with pull-up)
 * Col1 → P2.25 (output driven)
 */
#define KBD_COL1_P2BIT   25u
#define KBD_ROW1_P2BIT   0u
#define KBD_COL1_MASK    (1u<<KBD_COL1_P2BIT)
#define KBD_ROW1_MASK    (1u<<KBD_ROW1_P2BIT)

static inline void tiny_delay(volatile uint32_t n){ while(n--) __asm__("nop"); }

/* ================= I2C helpers ================== */
static inline void i2c0_wait_SI(void){ while((I2C0CONSET & I2C_SI)==0){} }
static void i2c0_start(void){ I2C0CONSET = I2C_I2EN|I2C_STA; i2c0_wait_SI(); }
static void i2c0_stop(void){  I2C0CONSET = I2C_STO; I2C0CONCLR = I2C_SI; }
static void i2c0_write(uint8_t b){ I2C0DAT=b; I2C0CONCLR=I2C_STA|I2C_SI; i2c0_wait_SI(); }

/* Public: I2C0 init @100kHz */
void I2C0_Init_100k(void){
  PCONP |= (1u<<7);   /* power I2C0 */
  PINSEL1 &= ~((3u<<22)|(3u<<24));
  PINSEL1 |=  ((1u<<22)|(1u<<24));
  PINMODE1 &= ~((3u<<22)|(3u<<24));
  PINMODE1 |=  ((2u<<22)|(2u<<24));
  I2C0CONCLR = I2C_AA|I2C_SI|I2C_STA;
  I2C0CONSET = I2C_I2EN;
  I2C0SCLH = 90; I2C0SCLL = 90;
}

/* Poll until device ACKs */
static void at24c512_ack_poll(void){
  for(;;){
    i2c0_start();
    i2c0_write((AT24C512_ADDR7<<1)|0u);
    if((I2C0STAT & 0xF8u)==0x18u) return;
    i2c0_stop();
    tiny_delay(30000);
  }
}

/* One page write */
static void at24c512_page_write(uint16_t addr, const uint8_t* data, uint16_t len){
  if(!len || len>AT24C512_PAGE_SZ) return;
  i2c0_start();
  i2c0_write((AT24C512_ADDR7<<1)|0u);
  i2c0_write((uint8_t)(addr>>8));
  i2c0_write((uint8_t)(addr&0xFF));
  for(uint16_t i=0;i<len;++i) i2c0_write(data[i]);
//  for(uint16_t i=127;i<len;i--) i2c0_write(data[i]);

  i2c0_stop();
  at24c512_ack_poll();
  i2c0_stop();
}

/* Public: split writes across pages */
void AT24C512_WriteBuffer(uint16_t start, const uint8_t* src, uint32_t nbytes){
  while(nbytes){
    uint16_t off   = start & (AT24C512_PAGE_SZ-1u);
    uint16_t space = AT24C512_PAGE_SZ - off;
    uint16_t chunk = (nbytes < space) ? (uint16_t)nbytes : space;
    at24c512_page_write(start, src, chunk);
    start  += chunk;
    src    += chunk;
    nbytes -= chunk;
  }
}

/* ================= Keypad SW2 =================== */
void SW2_Keypad_Init(void){
  PINSEL4 &= ~(3u << (KBD_ROW1_P2BIT*2));
  PINSEL5 &= ~(3u << ((KBD_COL1_P2BIT-16u)*2));
  FIO2DIR |=  KBD_COL1_MASK;
  FIO2DIR &= ~KBD_ROW1_MASK;
  FIO2SET = KBD_COL1_MASK;  /* idle HIGH */
}

uint32_t SW2_Pressed(void){
  FIO2CLR = KBD_COL1_MASK;
  tiny_delay(200);
  uint32_t pressed = ((FIO2PIN & KBD_ROW1_MASK) == 0u);
  FIO2SET = KBD_COL1_MASK;
  return pressed;
}
