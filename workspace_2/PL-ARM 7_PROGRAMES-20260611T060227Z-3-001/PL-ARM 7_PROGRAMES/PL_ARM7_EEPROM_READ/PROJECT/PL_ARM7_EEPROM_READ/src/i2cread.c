/*
 * i2cread.c
 *
 *  Created on: 09-Oct-2025
 *      Author: ADMIN
 */



#include "LPC24xx.h"
#include "i2cread.h"

/* ---------------- I2C register map ---------------- */
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

#define I2C_AA   0x04
#define I2C_SI   0x08
#define I2C_STO  0x10
#define I2C_STA  0x20
#define I2C_I2EN 0x40

#define KBD_COL1_P2BIT   25u
#define KBD_ROW1_P2BIT   0u
#define KBD_COL1_MASK    (1u<<KBD_COL1_P2BIT)
#define KBD_ROW1_MASK    (1u<<KBD_ROW1_P2BIT)

/* ---------------- Tiny delays ---------------- */
static inline void tiny_delay(volatile uint32_t n){ while(n--) __asm__("nop"); }

/* ==================== I2C helpers ==================== */
static inline void i2c0_wait_SI(void){ while((I2C0CONSET & I2C_SI)==0){} }

static void i2c0_start(void){ I2C0CONSET = I2C_I2EN | I2C_STA; i2c0_wait_SI(); }
static void i2c0_stop(void){  I2C0CONSET = I2C_STO; I2C0CONCLR = I2C_SI; }
static void i2c0_write(uint8_t b){ I2C0DAT=b; I2C0CONCLR=I2C_STA|I2C_SI; i2c0_wait_SI(); }

static uint8_t i2c0_read_ack(void){
  I2C0CONSET = I2C_AA;
  I2C0CONCLR = I2C_SI;
  i2c0_wait_SI();
  return (uint8_t)I2C0DAT;
}

static uint8_t i2c0_read_nack(void){
  I2C0CONCLR = I2C_AA | I2C_SI;
  i2c0_wait_SI();
  return (uint8_t)I2C0DAT;
}

void I2C0_Init_100k(void){
  PCONP |= (1u<<7);   /* Power I2C0 */
  PINSEL1 &= ~((3u<<22)|(3u<<24));
  PINSEL1 |=  ((1u<<22)|(1u<<24));
  PINMODE1 &= ~((3u<<22)|(3u<<24));
  PINMODE1 |=  ((2u<<22)|(2u<<24));
  I2C0CONCLR = I2C_AA|I2C_SI|I2C_STA;
  I2C0CONSET = I2C_I2EN;
  I2C0SCLH = 90; I2C0SCLL = 90;   /* ~100kHz if PCLK=18MHz */
}

uint8_t EE_ReadByte(uint16_t addr){
  uint8_t v;
  i2c0_start();
  i2c0_write((AT24C512_ADDR7<<1)|0u);   /* SLA+W */
  i2c0_write((uint8_t)(addr>>8));       /* high */
  i2c0_write((uint8_t)(addr&0xFF));     /* low */
  I2C0CONSET = I2C_STA; I2C0CONCLR = I2C_SI; i2c0_wait_SI();
  i2c0_write((AT24C512_ADDR7<<1)|1u);   /* SLA+R */
  v = i2c0_read_nack();
  i2c0_stop();
  return v;
}

void EE_ReadBuffer(uint16_t start, uint8_t *dst, uint32_t nbytes){
  if(!nbytes) return;
  i2c0_start();
  i2c0_write((AT24C512_ADDR7<<1)|0u);
  i2c0_write((uint8_t)(start>>8));
  i2c0_write((uint8_t)(start&0xFF));
  I2C0CONSET = I2C_STA; I2C0CONCLR = I2C_SI; i2c0_wait_SI();
  i2c0_write((AT24C512_ADDR7<<1)|1u);
  while(nbytes>1){ *dst++=i2c0_read_ack(); --nbytes; }
  *dst++=i2c0_read_nack();
  i2c0_stop();
}

void SW2_Keypad_Init(void){
  PINSEL4 &= ~(3u << (KBD_ROW1_P2BIT*2));
  PINSEL5 &= ~(3u << ((KBD_COL1_P2BIT-16u)*2));
  FIO2DIR |=  KBD_COL1_MASK;
  FIO2DIR &= ~KBD_ROW1_MASK;
  FIO2SET  =  KBD_COL1_MASK;   /* idle high */
}

uint32_t SW2_Pressed(void){
  FIO2CLR = KBD_COL1_MASK;
  tiny_delay(300);
  uint32_t pressed = ((FIO2PIN & KBD_ROW1_MASK)==0u);
  FIO2SET = KBD_COL1_MASK;
  return pressed;
}
