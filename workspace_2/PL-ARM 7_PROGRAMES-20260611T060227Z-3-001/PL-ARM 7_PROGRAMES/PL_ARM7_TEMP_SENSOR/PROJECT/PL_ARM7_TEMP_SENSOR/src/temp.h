/*
 * temp.h
 *
 *  Created on: 29-Sep-2025
 *      Author: ADMIN
 */

/*
 * temp35.h
 *
 *  Created on: 29-Sep-2025
 *      Author: ADMIN
 */

#ifndef TEMP_H
#define TEMP_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* LM35 / TEMP35: 10 mV/°C (0 V at 0°C)
 * Integer-only math (no floats), Redlib friendly.
 * Uses your internal ADC driver on ADC0.0 (P0.23).
 */

#ifndef TEMP_DEFAULT_VREF_MV
#define TEMP_DEFAULT_VREF_MV   3300u   /* 3.300 V reference */
#endif

#ifndef TEMP_DEFAULT_SAMPLES
#define TEMP_DEFAULT_SAMPLES   8u      /* averaging for stability */
#endif

/* Init temperature input (calls ADC0_InitCh0). */
void TEMP_Init(void);

/* Read raw 10-bit ADC code (0..1023). */
uint16_t TEMP_ReadRaw(void);

/* Read averaged ADC code (N >= 1). */
uint16_t TEMP_ReadRawAvg(uint8_t samples);

/* Convert raw -> millivolts using Vref (mV), rounded. */
uint32_t TEMP_RawToMilliVolts(uint16_t raw, uint16_t vref_mv);

/* Convenience: read (avg) and return Celsius x10 (deci-°C).
 * For LM35, deci-°C == millivolts (10 mV per °C, 1 mV per 0.1°C). */
uint16_t TEMP_Read_C10(uint16_t vref_mv, uint8_t samples);

/* Build exactly 21 chars for GLCD row:
 *   "TEMP = xx.x*C_________"  (spaces pad to 21) */
void TEMP_FormatLine(char *dst21, uint16_t vref_mv, uint8_t samples);

#ifdef __cplusplus
}
#endif

#endif /* TEMP_H */



