#include <stdint.h>

int __sys_istty(int fd) {
    return 0;  // no terminal
}

int __sys_flen(int file) {
    return 0;  // stub
}

int __sys_seek(int handle, int pos) {
    return -1; // not supported
}

int __sys_write(int iFileHandle, char *pcBuffer, int iLength)
{
    // Implement sending characters here (e.g., UART)
    // For now, just pretend all bytes were written successfully
    return iLength;
}


void __sys_appexit(void) {
    while (1);  // infinite loop on exit
}
