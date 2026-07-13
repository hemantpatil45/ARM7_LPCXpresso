/*
 * ldr.h
 *
 *  Created on: 29-Sep-2025
 *      Author: ADMIN
 */

#ifndef LDR_H
#define LDR_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ===== LDR on LPC2478 (via ADC0 CH0, P0.23) =====
 * - Uses your existing ADC driver (adc.h/adc.c)
 * - Integer math only (no floats) for Redlib
 * - GLCD-friendly formatters (21 chars wide)
 */

/* Defaults (override at compile time if needed) */
#ifndef LDR_DEFAULT_VREF_MV
#define LDR_DEFAULT_VREF_MV  3300u   /* 3.300 V */
#endif
#ifndef LDR_DEFAULT_SAMPLES
#define LDR_DEFAULT_SAMPLES  8u      /* averaging count */
#endif

/* Init LDR input (internally calls ADC0_InitCh0). */
void LDR_Init(void);

/* Read a single raw 10-bit code (0..1023). */
uint16_t LDR_ReadRaw(void);

/* Read an averaged 10-bit code (N >= 1). */
uint16_t LDR_ReadRawAvg(uint8_t samples);

/* Convert a raw code to millivolts using Vref (mV). */
uint32_t LDR_RawToMilliVolts(uint16_t raw, uint16_t vref_mv);

/* ===== GLCD-friendly formatters (build 21-char ASCII lines) =====
 *
 * 1) LDR_FormatLine_V_fromRaw:
 *    Input: raw (0..1023), vref (mV)
 *    Output: dst21 = "LDR VTG = X.YYYYV_____"
 *
 * 2) LDR_ReadAndFormatLine_V:
 *    Does an averaged ADC read internally and formats the line.
 */
void LDR_FormatLine_V_fromRaw(char *dst21, uint16_t raw, uint16_t vref_mv);

/* Read (avg=samples) and build the line in one call. */
void LDR_ReadAndFormatLine_V(char *dst21, uint16_t vref_mv, uint8_t samples);

#ifdef __cplusplus
}
#endif

#endif /* LDR_H */

