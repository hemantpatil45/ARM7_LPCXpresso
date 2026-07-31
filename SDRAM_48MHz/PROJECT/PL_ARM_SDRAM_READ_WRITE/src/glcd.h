/*
 * glcd.h
 *
 *  Created on: 29-Sep-2025
 *      Author: ADMIN
 */
#include <stdio.h>
#include "LPC24xx.h"
#include <stdint.h>
#include "system_init.h"         /* your pin macros (uses FIOx) */
#include "glcd_font5x7.h"

#ifndef GLCD_H
#define GLCD_H

#ifdef __cplusplus
extern "C" {
#endif

/* Geometry */
#define GLCD_WIDTH   128u
#define GLCD_HEIGHT   64u
#define GLCD_PAGES    (GLCD_HEIGHT/8u)

/* Public API */
void GLCD_Init(void);
void GLCD_Clear(void);

void GLCD_SetXY(uint8_t x, uint8_t page);
void GLCD_PutChar(uint8_t x, uint8_t page, char c);
void GLCD_PutString(uint8_t x, uint8_t page, const char* s);

/* Convenience wrappers (match names you used earlier) */
void GLCD_RowWrite(uint8_t page, const char* s);                /* clear page + print at x=0 */
void GLCD_PutStringXY(uint8_t x, uint8_t page, const char* s);  /* alias of GLCD_PutString */
void GLCD_PutString20(uint8_t page, const char *s);
void GLCD_RowWriteMargin(uint8_t page, const char* s);

#ifdef __cplusplus
}
#endif

#endif /* GLCD_H */
