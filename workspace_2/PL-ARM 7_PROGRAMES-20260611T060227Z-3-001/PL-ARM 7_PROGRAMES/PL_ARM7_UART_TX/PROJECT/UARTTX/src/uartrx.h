
#ifndef UARTRX_H
#define UARTRX_H

#include <stdint.h>


void UARTRX_Init(uint32_t pclk_hz, uint32_t baud);



void UARTRX_Task(void);

#endif /* UARTRX_H */
