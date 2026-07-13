/*
 * INT_DAC.h
 *
 *  Created on: Sep 29, 2025
 *      Author: divya-devdhe
 */

#ifndef INT_DAC_H_
#define INT_DAC_H_

void delay_us();
void delay();
void dac_init();
void dac_write(uint16_t code10);
void timer0_init_us(void);
void timer0_wait_match0(void);
#endif /* INT_DAC_H_ */
