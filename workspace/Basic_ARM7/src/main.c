#include "LPC24XX.h" // Ensure you have the correct CMSIS header included

int main(void) {
    // 1. Configure P0.10 as an output
    // FIODIR register: 1 = output, 0 = input
    FIO0DIR |= (1 << 10);

    while(1) {
        // 2. Turn LED ON
        FIO0SET = (1 << 10);

        // 3. Simple delay loop
        for(int i = 0; i < 1000000; i++);

        // 4. Turn LED OFF
        FIO0CLR = (1 << 10);

        // 5. Simple delay loop
        for(int i = 0; i < 1000000; i++);
    }
    return 0;
}
