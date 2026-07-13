/*
 * internaladc.c
 *
 *  Created on: 29-Sep-2025
 *      Author: ADMIN
 */


#include "LPC24xx.h"
#include <stdint.h>
#include "internaladc.h"

/* ===== Config ===== */
#ifndef ADC_CCLK_HZ
#define ADC_CCLK_HZ     (72000000u)  /* core clock (set by your PLL) */
#endif

#ifndef ADC_TARGET_HZ
#define ADC_TARGET_HZ   (4000000u)   /* aim <= 4.5 MHz */
#endif

#define _BV(b) (1u << (b))

void ADC0_InitCh0(void)
{
    /* Power up ADC0: PCONP bit 12 (PCADC) */
    PCONP |= _BV(12);

    /* PCLK for ADC0 = CCLK/1: PCLKSEL0[25:24] = 01 */
    PCLKSEL0 &= ~(3u << 24);
    PCLKSEL0 |=  (1u << 24);

    /* Pin mux: P0.23 → AD0.0 (PINSEL1[15:14] = 01) */
    PINSEL1  &= ~(3u << 14);
    PINSEL1  |=  (1u << 14);

    /* Disable pulls on the analog pin: PINMODE1[15:14] = 10 (neither) */
    PINMODE1 &= ~(3u << 14);
    PINMODE1 |=  (2u << 14);

    /* Optional: ensure GPIO direction is input */
    FIO0DIR  &= ~(1u << 23);

    /* ADC clock divider: ADCclk = PCLK / (CLKDIV + 1) */
    uint32_t pclk   = ADC_CCLK_HZ;               /* since PCLKSEL set to /1 */
    uint32_t clkdiv = pclk / ADC_TARGET_HZ;
    if (clkdiv > 0) clkdiv -= 1;
    if (clkdiv > 0xFFu) clkdiv = 0xFFu;

    /* AD0CR setup */
    AD0CR = (1u << 0)           /* select ch0 */
          | (clkdiv << 8)       /* set divider */
          | (0u << 16)          /* BURST=0 */
          | (0u << 17)          /* 10-bit */
          | (1u << 21)          /* PDN=1 */
          | (0u << 24);         /* START=000 */
}

uint16_t ADC0_ReadCh0(void)
{
    /* START now: START=001 */
    AD0CR &= ~(7u << 24);
    AD0CR |=  (1u << 24);

    /* Wait for DONE (bit31) */
    uint32_t gdr;
    do { gdr = AD0GDR; } while ((gdr & (1u << 31)) == 0);

    /* Stop (clear START) – optional when BURST=0 */
    AD0CR &= ~(7u << 24);

    /* Result bits [15:6] */
    return (uint16_t)((gdr >> 6) & 0x03FFu);
}

uint16_t ADC0_ReadCh0_Avg(uint8_t samples)
{
    if (samples == 0) samples = 1;
    uint32_t acc = 0;
    for (uint8_t i = 0; i < samples; ++i) acc += ADC0_ReadCh0();
    return (uint16_t)(acc / samples);
}

uint32_t ADC0_CodeToMilliVolts(uint16_t raw, uint16_t vref_mv)
{
    if (raw > 1023u) raw = 1023u;
    /* mV = raw * Vref / 1023 */
    return ((uint32_t)raw * (uint32_t)vref_mv) / 1023u;
}

uint32_t ADC0_CodeToMicroVolts(uint16_t raw, uint16_t vref_mv)
{
    if (raw > 1023u) raw = 1023u;
    /* uV = raw * Vref(mV) * 1000 / 1023, with rounding */
    return ((uint32_t)raw * (uint32_t)vref_mv * 1000u + 511u) / 1023u;
}

void ADC_Format4(uint16_t v, char *out)
{
    if (v > 1023u) v = 1023u;
    out[0] = (char)('0' + ((v/1000)%10));
    out[1] = (char)('0' + ((v/100)%10));
    out[2] = (char)('0' + ((v/10)%10));
    out[3] = (char)('0' + (v%10));
    out[4] = '\0';
}

/* Build exactly 21 chars for one GLCD row: "ADC=####  V=X.YYYYV____"
   (underscores here mean spaces; function pads with spaces to 21). */
static char* _u32_to_dec(char *out, uint32_t v){
    char tmp[11]; int i=0;
    if (v==0){ *out++='0'; *out='\0'; return out; }
    while (v){ tmp[i++] = (char)('0' + (v%10)); v/=10; }
    while (i--) *out++ = tmp[i];
    *out = '\0';
    return out;
}

void ADC_FormatLine_ADC_V(char *dst21, uint16_t raw, uint16_t vref_mv)
{
    uint32_t uV   = ADC0_CodeToMicroVolts(raw, vref_mv);
    uint32_t vint = uV / 1000000u;             /* integer volts */
    uint32_t frac = (uV % 1000000u) / 100u;    /* 4 decimals (0..9999) */

    char *p = dst21;

    /* "ADC=" + 4-digit raw */
    *p++='A'; *p++='D'; *p++='C'; *p++='=';
    char raw4[5]; ADC_Format4(raw, raw4);
    for (int i=0;i<4;i++) *p++ = raw4[i];

    /* "  V=" */
    *p++=' '; *p++=' '; *p++='V'; *p++='=';

    /* X.YYYYV */
    p = _u32_to_dec(p, vint);
    *p++='.';
    *p++=(char)('0'+(uint8_t)((frac/1000)%10));
    *p++=(char)('0'+(uint8_t)((frac/100)%10));
    *p++=(char)('0'+(uint8_t)((frac/10)%10));
    *p++=(char)('0'+(uint8_t)( frac%10));
    *p++='V';

    /* pad to 21 chars */
    while ((p - dst21) < 21) *p++ = ' ';
    *p = '\0';
}
