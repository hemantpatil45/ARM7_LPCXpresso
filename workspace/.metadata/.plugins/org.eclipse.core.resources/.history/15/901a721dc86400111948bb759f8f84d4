/*
 * led.c
 *
 *  Created on: 30-Sep-2025
 *      Author: ADMIN
 */

#include "LPC24xx.h"
#include <stdint.h>
#include "SYSTEM_INIT.h"         /* your pin macros (uses FIOx) */
#include "glcd.h"
#include "glcd_font5x7.h"


unsigned int Read_Switches(void) {
    unsigned int sw = 0;

    if (Switch0_STATE) sw |= (1 << 0); // SW0 → bit 0
    if (Switch1_STATE) sw |= (1 << 1); // SW1 → bit 1
    if (Switch2_STATE) sw |= (1 << 2); // SW2 → bit 2
    if (Switch3_STATE) sw |= (1 << 3); // SW3 → bit 3

    return sw;
}
