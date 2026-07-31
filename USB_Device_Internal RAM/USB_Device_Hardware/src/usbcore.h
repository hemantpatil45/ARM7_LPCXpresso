#ifndef __USBCORE_H
#define __USBCORE_H

#include <stdint.h>

typedef struct {
    uint8_t  bmRequestType;
    uint8_t  bRequest;
    uint16_t wValue;
    uint16_t wIndex;
    uint16_t wLength;
} USB_SETUP_PACKET;

// Command Block Wrapper (31 bytes from Host)
typedef struct {
    uint32_t dCBWSignature;          // 0x43425355 ("USBC")
    uint32_t dCBWTag;                // Echoed back in CSW
    uint32_t dCBWDataTransferLength; // Expected data length
    uint8_t  bmCBWFlags;             // Direction flag
    uint8_t  bCBWLUN;                // Logical Unit Number
    uint8_t  bCBWCBLength;           // Length of SCSI CDB
    uint8_t  CBWCB[16];              // SCSI Command Data Block
} __attribute__((packed)) USB_CBW;

// Command Status Wrapper (13 bytes to Host)
typedef struct {
    uint32_t dCSWSignature;          // 0x53425355 ("USBS")
    uint32_t dCSWTag;                // Must match dCBWTag
    uint32_t dCSWDataResidue;        // Difference in expected vs sent data
    uint8_t  bCSWStatus;             // 0 = Passed, 1 = Failed, 2 = Phase Error
} __attribute__((packed)) USB_CSW;

#define USB_REQUEST_GET_STATUS          0x00
#define USB_REQUEST_CLEAR_FEATURE       0x01
#define USB_REQUEST_SET_FEATURE         0x03
#define USB_REQUEST_SET_ADDRESS         0x05
#define USB_REQUEST_GET_DESCRIPTOR      0x06
#define USB_REQUEST_SET_DESCRIPTOR      0x07
#define USB_REQUEST_GET_CONFIGURATION   0x08
#define USB_REQUEST_SET_CONFIGURATION   0x09

extern USB_SETUP_PACKET SetupPacket;
extern void USB_EndPoint0(uint32_t event);
extern void Process_Bulk_OUT(void);
extern void Init_RAM_Disk(void);

#endif
