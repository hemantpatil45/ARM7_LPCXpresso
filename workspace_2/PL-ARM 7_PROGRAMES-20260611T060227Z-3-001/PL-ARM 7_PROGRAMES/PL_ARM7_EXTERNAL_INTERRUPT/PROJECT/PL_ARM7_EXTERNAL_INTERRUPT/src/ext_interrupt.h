/*
 * ext_interrupt.h
 *
 *  Created on: 30-Sep-2025
 *      Author: ADMIN
 */

#ifndef EXT_INTERRUPT_H_
#define EXT_INTERRUPT_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Configure P2.10 as EINT0 (falling edge), set LED2 as output OFF, and enable VIC. */
void EINT0_Init(void);

#ifdef __cplusplus
}
#endif



#endif /* EXT_INTERRUPT_H_ */
