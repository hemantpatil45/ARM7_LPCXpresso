#ifndef USB_HOST_LPC2478_H_
#define USB_HOST_LPC2478_H_

#include "LPC24xx.h"
#include "type.h"

// OHCI & HCCA Alignment Constraints
#define USB_OHCI_BASE_ADDR   0x7FD00000    // USB SRAM Base Address
#define HCCA_BLOCK_SIZE      256

// OHCI Timing & Power Constants
#define FM_INTERVAL_FSMPS    0x2374
#define FM_INTERVAL_FI       0x2EDF
#define PERIODIC_START       0x3E67
#define LS_THRESHOLD         0x0628
#define PWRON_TO_PWRGOOD     50

// Status Flags
#define USB_HOST_OK          0
#define USB_HOST_ERR         1

// Function Prototypes
void USBHost_HardwareInit(void);
DWORD USBHost_OHCIInit(void);
BOOL USBHost_IsDeviceConnected(void);
void USBHost_ResetPort(void);

#endif /* USB_HOST_LPC2478_H_ */
