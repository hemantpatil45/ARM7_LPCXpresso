#include "LPC24xx.h"
#include "system_init.h"
#include "glcd.h"
#include "uartrx.h"

int main(void)
{
   // PLL_Init();
    system_Init();

    GLCD_Init();
    GLCD_Clear();

    UARTRX_Init(12000000u, 9600u);

    while (1) {
        UARTRX_Task();
    }
}
