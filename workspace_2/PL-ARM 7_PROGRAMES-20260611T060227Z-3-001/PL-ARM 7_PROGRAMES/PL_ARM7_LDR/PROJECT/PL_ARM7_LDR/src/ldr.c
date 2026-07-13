/*
 * ldr.c
 *
 *  Created on: 29-Sep-2025
 *      Author: ADMIN
 */
#include <stdint.h>
#include "internaladc.h"   /* reuses your ADC driver */
#include "ldr.h"

/* ---- Public API ---- */
void LDR_Init(void) {
    ADC0_InitCh0();     /* AD0.0 on P0.23 */
}

uint16_t LDR_ReadRaw(void) {
    return ADC0_ReadCh0();
}

uint16_t LDR_ReadRawAvg(uint8_t samples) {
    return ADC0_ReadCh0_Avg(samples ? samples : 1);
}

uint32_t LDR_RawToMilliVolts(uint16_t raw, uint16_t vref_mv) {
    return ADC0_CodeToMilliVolts(raw, vref_mv);
}

/* ---- tiny helpers (no printf) ---- */
static char* _u32_to_dec(char *out, uint32_t v){
    char tmp[11]; int i = 0;
    if (v == 0) { *out++ = '0'; *out = '\0'; return out; }
    while (v) { tmp[i++] = (char)('0' + (v % 10)); v /= 10; }
    while (i--) *out++ = tmp[i];
    *out = '\0';
    return out;
}

/* Build exactly 21 chars: "LDR VTG = X.YYYYV_____"  (spaces pad to 21) */
void LDR_FormatLine_V_fromRaw(char *dst21, uint16_t raw, uint16_t vref_mv)
{
    /* Use your ADC helper for rounded microvolts:
       uV = raw * Vref(mV) * 1000 / 1023  (rounded in adc.c) */
    uint32_t uV   = ADC0_CodeToMicroVolts(raw, vref_mv);
    uint32_t vint = uV / 1000000u;           /* integer volts */
    uint32_t frac = (uV % 1000000u) / 100u;  /* 4 decimals (0..9999) */

    char *p = dst21;

    /* prefix "LDR VTG = " */
    const char *pre = "LDR VTG = ";
    while (*pre) *p++ = *pre++;

    /* X */
    p = _u32_to_dec(p, vint);

    /* ".YYYY" */
    *p++ = '.';
    *p++ = (char)('0' + (uint8_t)((frac / 1000u) % 10u));
    *p++ = (char)('0' + (uint8_t)((frac / 100u ) % 10u));
    *p++ = (char)('0' + (uint8_t)((frac / 10u  ) % 10u));
    *p++ = (char)('0' + (uint8_t)( frac          % 10u));
    *p++ = 'V';

    /* pad with spaces to 21 chars total */
    while ((p - dst21) < 21) *p++ = ' ';
    *p = '\0';
}

void LDR_ReadAndFormatLine_V(char *dst21, uint16_t vref_mv, uint8_t samples)
{
    uint16_t raw = LDR_ReadRawAvg(samples ? samples : LDR_DEFAULT_SAMPLES);
    LDR_FormatLine_V_fromRaw(dst21, raw, vref_mv ? vref_mv : LDR_DEFAULT_VREF_MV);
}

