#ifndef USBCORE_H
#define USBCORE_H

#include <stdint.h>

void USB_EndPoint0(uint32_t event);
void Process_Bulk_OUT(void);

#endif
