#include <stdint.h>
#include "LPC24xx.h"
#include "glcd.h"
#include "system_init.h"

void dac8512_ssp0_init();
void dac8512_write(uint16_t code12);
void timer0_init_us();
void timer0_wait_match0();
