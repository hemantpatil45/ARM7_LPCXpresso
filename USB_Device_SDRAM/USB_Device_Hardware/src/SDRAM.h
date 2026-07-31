#ifndef SDRAM_H_
#define SDRAM_H_

#include <stdint.h>

#define EMC_CLK_FREQ 48000000

extern void SDRAM_init_32BIT(void);
extern void timer_delay(unsigned int us);

#endif
