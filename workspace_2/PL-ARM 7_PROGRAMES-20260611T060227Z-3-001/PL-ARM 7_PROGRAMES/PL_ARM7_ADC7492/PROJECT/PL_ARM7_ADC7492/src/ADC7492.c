#include "glcd.h"
#include "system_init.h"

#define VREF_mV      3300U

void u16_to_str(uint16_t v, char *buf)
{
    char tmp[6]; int i=0;
    if (v==0){ buf[0]='0'; buf[1]=0; return; }
    while (v>0 && i<6){ tmp[i++] = (char)('0' + (v%10)); v/=10; }
    int j=0; while(i--) buf[j++] = tmp[i]; buf[j]=0;
}

void delay_us(uint32_t t)
{
    volatile uint32_t n = t * 72U; // For 72MHz
    while (n--) __asm__("nop");
}

int ad7492_read(void)
{
    ADC_CONVST_LOW();
    delay_us(100);
    ADC_CONVST_HIGH();

    while (ADC_BUSY_IS_HIGH()) { }

    ADC_CS_LOW();
    ADC_RD_LOW();
    delay_us(100);
    uint16_t v = (uint16_t)ADC_DATA_IN;
    ADC_RD_HIGH();
    ADC_CS_HIGH();
    return v & 0x0FFF;
}
