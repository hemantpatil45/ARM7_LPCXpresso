#ifndef USB_HOST_TD_H_
#define USB_HOST_TD_H_

#include "LPC24xx.h"
#include "type.h"

// TD Token Types
#define TD_SETUP    0
#define TD_OUT      1
#define TD_IN       2

// OHCI Descriptor Structures (Must be 16-byte aligned in USB RAM)
typedef struct {
    volatile USB_INT32U Control;
    volatile USB_INT32U TailTd;
    volatile USB_INT32U HeadTd;
    volatile USB_INT32U NextEd;
} HCED;

typedef struct {
    volatile USB_INT32U Control;
    volatile USB_INT32U CurrBufPtr;
    volatile USB_INT32U NextTd;
    volatile USB_INT32U BufEnd;
} HCTD;

// Global Pointers used by usbhost_ms.c
extern volatile HCED *EDBulkIn;
extern volatile HCED *EDBulkOut;

// Function Prototypes
USB_INT32S Host_ProcessTD(volatile HCED *ed, USB_INT32U token, volatile USB_INT08U *buffer, USB_INT32U buffer_len);
USB_INT32S Host_CtrlRecv(USB_INT08U bmRequestType, USB_INT08U bRequest, USB_INT16U wValue, USB_INT16U wIndex, USB_INT16U wLength, volatile USB_INT08U *buffer);

#endif /* USB_HOST_TD_H_ */
