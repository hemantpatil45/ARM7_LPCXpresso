#ifndef USBHOST_INC_H
#define USBHOST_INC_H

#include <stdint.h>

#ifndef DWORD
typedef uint32_t DWORD;
#endif

#ifndef BYTE
typedef uint8_t BYTE;
#endif

#ifndef OK
#define OK      0
#endif

#ifndef ERROR
#define ERROR   1
#endif

/* Core USB Host & UART Function Prototypes */
void USBHostInit(void);
void OHCIInit(void);
void OHCIPortOpen(uint32_t portNum);
DWORD Host_EnumDev(void);
uint32_t MS_Init(void);
void delayMs(uint32_t port, uint32_t ms);
void UART0_Print(const char *str);
void UART0_PrintHex(uint32_t val);

#endif /* USBHOST_INC_H */
