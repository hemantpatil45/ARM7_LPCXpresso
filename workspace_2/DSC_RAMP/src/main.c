#include <stdint.h>
#include <math.h>
#include "LPC24xx.h"
#include "glcd.h"
#include "system_init.h"
#include "DAC8512.h"

// Sine wave lookup table with 256 samples
#define TABLE_SIZE 256
static uint16_t sine_table[TABLE_SIZE];

// Function to initialize the sine lookup table
void init_sine_table(void) {
    const uint16_t MAX_CODE = 2048u;  // 12-bit maximum
    const uint16_t MIN_CODE = 0;      // 12-bit minimum
    const uint16_t CENTER_CODE = 1024; // Mid-point (MAX_CODE/2)
    const uint16_t AMPLITUDE = 2048;   // Maximum swing from center

    for (int i = 0; i < TABLE_SIZE; i++) {
        // Calculate sine value from 0 to 2*PI
        double angle = (2.0 * 3.14159265 * i) / TABLE_SIZE;
        // Sine wave centered at MID_CODE, swinging between 0 and 4095
        // sin(angle) gives -1 to +1
        // Multiply by AMPLITUDE gives -2048 to +2048
        // Add CENTER_CODE gives 0 to 4095
        double sine_value = (sin(angle) * AMPLITUDE) + CENTER_CODE;
        sine_table[i] = (uint16_t)sine_value;
    }
}

int main(void)
{
    PLL_Init();
    system_Init();
    GLCD_Init();
    GLCD_Clear();

    GLCD_RowWriteMargin(0, "WELCOME TO AKADEMIKA");
    GLCD_RowWriteMargin(2, "      PL-ARM7       ");
    GLCD_RowWriteMargin(4, "  Generation of     ");
    GLCD_RowWriteMargin(6, "   SINE Wave on     ");
    GLCD_RowWriteMargin(8, "    DAC8512         ");

    delay_cycles(1000);
    delay_cycles(1000);

    GLCD_RowWriteMargin(4,"CONNECT DSO PROBE   ");
    GLCD_RowWriteMargin(6,"TO J70 & J71        ");

    M_DAC_CLR_OFF;
    dac8512_ssp0_init();
    timer0_init_us();

    // Initialize the sine lookup table
    init_sine_table();

    uint16_t index = 0;

    for (;;) {
        timer0_wait_match0(); // Wait for 10us
        dac8512_write(sine_table[index]); // Output sine wave sample

        index++; // Increment index for next sample
        if (index >= TABLE_SIZE) {
            index = 0; // Reset to 0 to restart the sine wave cycle
        }
    }
}
