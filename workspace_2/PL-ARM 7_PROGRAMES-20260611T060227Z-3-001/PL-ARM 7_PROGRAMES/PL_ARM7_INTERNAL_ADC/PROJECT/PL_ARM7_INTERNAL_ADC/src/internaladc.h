/*
 * internaladc.h
 *
 *  Created on: 29-Sep-2025
 *      Author: ADMIN
 */

#ifndef INTERNALADC_H_
#define INTERNALADC_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ===== LPC2478 ADC0 (internal) – single-shot, polling =====
 * Channel: AD0.0 on P0.23
 * Resolution: 10-bit (0..1023)
 */

/* Initialize ADC0 CH0 (P0.23 → AD0.0), set PCLK, mux, mode, divider. */
void ADC0_InitCh0(void);

/* Single conversion on AD0.0; returns 10-bit code (0..1023). */
uint16_t ADC0_ReadCh0(void);

/* Average N samples (N >= 1). */
uint16_t ADC0_ReadCh0_Avg(uint8_t samples);

/* Convert code to millivolts using Vref (mV). */
uint32_t ADC0_CodeToMilliVolts(uint16_t raw, uint16_t vref_mv);

/* Convert code to microvolts using Vref (mV), rounded. */
uint32_t ADC0_CodeToMicroVolts(uint16_t raw, uint16_t vref_mv);

/* Format helpers (no floats, GLCD-friendly) */
void ADC_Format4(uint16_t v, char *out);                     /* "0000".."1023" */
void ADC_FormatLine_ADC_V(char *dst21, uint16_t raw, uint16_t vref_mv);
/* Produces exactly 21 chars: "ADC=####  V=X.YYYYV____" (spaces pad to 21) */

/* Back-compat aliases (if you used these names earlier) */
static inline void     adc0_init_ch0(void) { ADC0_InitCh0(); }
static inline uint16_t adc0_read_ch0(void) { return ADC0_ReadCh0(); }

#ifdef __cplusplus
}
#endif

#endif /* INTERNALADC_H_ */
