#include "SDRAM.h"
#include "LPC24xx.h"
#include  "system_init.h"
// Define the number of elements to test
#define TEST_DATA_LENGTH 10

int main(void)
{
	system_Init();
	PLL_Init();
    // 1. Initialize system clocks and peripherals (UART, Pins, etc.)
    // sys_init_without_TFT_32bit_SDRAM(); // Assuming you have this function based on your earlier code
    
    // 2. Prepare some test data to write to the SDRAM
    unsigned int data_to_write[TEST_DATA_LENGTH] = {
        0xDEADBEEF, 
        0x11223344, 
        0xAABBCCDD, 
        0x00000000, 
        0xFFFFFFFF, 
        0x12345678, 
        0x9ABCDEF0, 
        0x55555555, 
        0xAAAAAAAA, 
        0xCAFEBABE
    };

    // 3. Initialize the 32-bit SDRAM controller
    SDRAM_init_32BIT();
    delay_ms(1000);
    // 4. Write the test data to SDRAM
    SDRAM_write(TEST_DATA_LENGTH, data_to_write);

    // 5. Read the data back from SDRAM
    // This will populate the 'test1234' array and call your display/transmit functions
    SDRAM_read(TEST_DATA_LENGTH);

    // 6. Halt execution in an infinite loop
    while(1)
    {
        // System loop, watchdog reset, etc.
    }

    return 0;
}
