#include "LPC24xx.h"
#include "usbcore.h"
#include "usbhw.h"
#include "usbdesc.h"
#include "upper_sdram.h"
#include <stdbool.h>

#ifndef USB_REQUEST_SET_ADDRESS
#define USB_REQUEST_SET_ADDRESS        5
#endif
#ifndef USB_REQUEST_GET_DESCRIPTOR
#define USB_REQUEST_GET_DESCRIPTOR     6
#endif
#ifndef USB_REQUEST_SET_CONFIGURATION
#define USB_REQUEST_SET_CONFIGURATION  9
#endif

typedef struct __attribute__((packed)) _USB_SETUP_PACKET {
    uint8_t  bmRequestType;
    uint8_t  bRequest;
    uint16_t wValue;
    uint16_t wIndex;
    uint16_t wLength;
} USB_SETUP_PACKET;

typedef struct __attribute__((packed)) _USB_CBW {
    uint32_t dCBWSignature;
    uint32_t dCBWTag;
    uint32_t dCBWDataTransferLength;
    uint8_t  bmCBWFlags;
    uint8_t  bCBWLUN;
    uint8_t  bCBWCBLength;
    uint8_t  CBWCB[16];
} USB_CBW;

typedef struct __attribute__((packed)) _USB_CSW {
    uint32_t dCSWSignature;
    uint32_t dCSWTag;
    uint32_t dCSWDataResidue;
    uint8_t  bCSWStatus;
} USB_CSW;

#ifndef USB_ReadCmdData
extern uint32_t USB_ReadCmdData(uint32_t cmd);
#endif

extern void UART0_Print(const char *str);
extern void UART0_PrintHex(uint32_t val);

USB_SETUP_PACKET SetupPacket;
USB_CBW cbw;
USB_CSW csw;

static const uint8_t SCSI_Sense_Data[] = {0x70, 0, 0x05, 0, 0, 0, 0, 10, 0, 0, 0, 0, 0x30, 0x01, 0, 0, 0, 0};
static const uint8_t SCSI_Mode_Data[]  = {0x03, 0x00, 0x00, 0x00};

const uint8_t SCSI_Capacity_Data[8] = {
    0x00, 0x00, 0x7F, 0xFF, // Last LBA (32767 for 16MB)
    0x00, 0x00, 0x02, 0x00  // Block Length: 512 bytes
};

const uint8_t SCSI_Inquiry_Data[36] = {
    0x00, 0x80, 0x02, 0x02, 31, 0x00, 0x00, 0x00,
    'L', 'P', 'C', '2', '4', '7', '8', ' ',
    'U', 'P', 'P', 'E', 'R', '1', '6', 'B',
    'I', 'T', ' ', 'S', 'D', 'R', 'A', 'M',
    '1', '.', '0', '0'
};

volatile uint8_t *Bulk_Data_Ptr;
uint32_t Bulk_Data_Len;
bool Bulk_Sending_Data = false;

volatile uint8_t *Write_Data_Ptr;
bool Bulk_Expecting_Write_Data = false;
uint32_t Expected_Transfer_Len = 0;
uint32_t Actual_Transferred_Len = 0;
uint32_t Remaining_Bytes_To_Receive = 0;

// Internal SRAM staging buffer for incoming USB writes
#define SDRAM_DMA_BUFFER_SIZE (32 * 512) // 16KB buffer
static uint8_t SDRAM_DMA_BUFFER_INTERNAL[SDRAM_DMA_BUFFER_SIZE];
#define SDRAM_DMA_BUFFER ((volatile uint8_t *)SDRAM_DMA_BUFFER_INTERNAL)

static uint32_t current_target_lba = 0;

void USB_EndPoint0(uint32_t event) {
    uint8_t *pData;
    uint32_t descriptor_size;

    if (event == 0) {
        USB_WriteCmd(CMD_SEL_EP + 0);
        uint32_t ep_stat = USB_ReadCmdData(CMD_SEL_EP + 0);

        if (ep_stat & (1 << 5)) {
            USB_ReadEP(0x00, (uint8_t *)&SetupPacket);

            switch (SetupPacket.bRequest) {
                case USB_REQUEST_SET_ADDRESS:
                    USB_SetAddress(SetupPacket.wValue);
                    USB_WriteEP(0x80, 0, 0);
                    break;
                case USB_REQUEST_GET_DESCRIPTOR:
                    switch (SetupPacket.wValue >> 8) {
                        case 0x01: pData = (uint8_t *)USB_DeviceDescriptor; descriptor_size = 18; break;
                        case 0x02: pData = (uint8_t *)USB_ConfigDescriptor; descriptor_size = 32; break;
                        default: USB_SetStallEP(0x80); return;
                    }
                    if (descriptor_size > SetupPacket.wLength) descriptor_size = SetupPacket.wLength;
                    USB_WriteEP(0x80, pData, descriptor_size);
                    break;
                case USB_REQUEST_SET_CONFIGURATION:
                    USB_Configure(true);
                    USB_ConfigEP(0x81, 64); USB_EnableEP(0x81);
                    USB_ConfigEP(0x02, 64); USB_EnableEP(0x02);
                    EP_INT_EN |= (1 << 3) | (1 << 4);
                    USB_WriteEP(0x80, 0, 0);
                    break;
                case 0xFE:
                    if (SetupPacket.bmRequestType == 0xA1) {
                        uint8_t max_lun = 0; USB_WriteEP(0x80, &max_lun, 1);
                    } else USB_SetStallEP(0x80);
                    break;
                default: USB_SetStallEP(0x80); break;
            }
        } else {
            uint8_t dummy[64]; USB_ReadEP(0x00, dummy);
        }
    }
}

void BOT_Send_Next_Chunk(void) {
    if (Bulk_Data_Len > 0) {
        uint32_t chunk = (Bulk_Data_Len > 64) ? 64 : Bulk_Data_Len;
        if (Bulk_Data_Ptr != 0) {
            USB_WriteEP(0x81, (uint8_t *)Bulk_Data_Ptr, chunk);
            Bulk_Data_Ptr += chunk;
        } else {
            uint8_t zeroes[64] = {0};
            USB_WriteEP(0x81, zeroes, chunk);
        }
        Bulk_Data_Len -= chunk;
        Actual_Transferred_Len += chunk;
    } else if (Bulk_Sending_Data) {
        Bulk_Sending_Data = false;
        csw.dCSWDataResidue = Expected_Transfer_Len - Actual_Transferred_Len;
        USB_WriteEP(0x81, (uint8_t *)&csw, 13);
    }
}

void Process_Bulk_OUT(void) {
    if (Bulk_Expecting_Write_Data) {
        uint32_t count = USB_ReadEP(0x02, (uint8_t *)Write_Data_Ptr);

        Write_Data_Ptr += count;
        Remaining_Bytes_To_Receive -= count;
        Actual_Transferred_Len += count;

        uint32_t buffered_bytes = Write_Data_Ptr - SDRAM_DMA_BUFFER;
        uint32_t buffered_sectors = buffered_bytes / 512;

        // If buffer filled up with complete sectors, or transfer finished
        if (buffered_sectors > 0 || (Remaining_Bytes_To_Receive == 0 && buffered_bytes > 0)) {
            if (Remaining_Bytes_To_Receive == 0 && (buffered_bytes % 512 != 0)) {
                uint32_t remainder = 512 - (buffered_bytes % 512);
                for (uint32_t i = 0; i < remainder; i++) {
                    Write_Data_Ptr[i] = 0;
                }
                buffered_sectors++;
            }

            if (buffered_sectors > 0) {
                for (uint32_t s = 0; s < buffered_sectors; s++) {
                    UpperSDRAM_WriteBlock(current_target_lba + s, (const uint8_t *)&SDRAM_DMA_BUFFER[s * 512]);
                }
                current_target_lba += buffered_sectors;
                Write_Data_Ptr = SDRAM_DMA_BUFFER; // Reset buffer window
            }
        }

        if (Remaining_Bytes_To_Receive == 0) {
            Bulk_Expecting_Write_Data = false;
            csw.bCSWStatus = 0;
            csw.dCSWDataResidue = 0;
            USB_WriteEP(0x81, (uint8_t *)&csw, 13);
        }
        return;
    }

    uint32_t count = USB_ReadEP(0x02, (uint8_t *)&cbw);
    if (count == 31 && cbw.dCBWSignature == 0x43425355) {
        csw.dCSWSignature = 0x53425355;
        csw.dCSWTag = cbw.dCBWTag;
        csw.bCSWStatus = 0;
        Expected_Transfer_Len = cbw.dCBWDataTransferLength;
        Actual_Transferred_Len = 0;

        UART0_Print("[SCSI CMD] Opcode: ");
        UART0_PrintHex(cbw.CBWCB[0]);
        UART0_Print(" | Len: ");
        UART0_PrintHex(Expected_Transfer_Len);
        UART0_Print("\r\n");

        switch (cbw.CBWCB[0]) {
            case 0x00: // TEST UNIT READY
                csw.dCSWDataResidue = 0;
                USB_WriteEP(0x81, (uint8_t *)&csw, 13);
                break;
            case 0x03: // REQUEST SENSE
                Bulk_Data_Ptr = (volatile uint8_t *)SCSI_Sense_Data;
                Bulk_Data_Len = sizeof(SCSI_Sense_Data);
                Bulk_Sending_Data = true;
                BOT_Send_Next_Chunk();
                break;
            case 0x12: // INQUIRY
                Bulk_Data_Ptr = (volatile uint8_t *)SCSI_Inquiry_Data;
                Bulk_Data_Len = sizeof(SCSI_Inquiry_Data);
                Bulk_Sending_Data = true;
                BOT_Send_Next_Chunk();
                break;
            case 0x1A: // MODE SENSE (6)
            case 0x5A: // MODE SENSE (10)
                Bulk_Data_Ptr = (volatile uint8_t *)SCSI_Mode_Data;
                Bulk_Data_Len = sizeof(SCSI_Mode_Data);
                Bulk_Sending_Data = true;
                BOT_Send_Next_Chunk();
                break;
            case 0x1E: // PREVENT ALLOW MEDIUM REMOVAL
                csw.dCSWDataResidue = 0;
                csw.bCSWStatus = 0;
                USB_WriteEP(0x81, (uint8_t *)&csw, 13);
                break;
            case 0x25: // READ CAPACITY
                Bulk_Data_Ptr = (volatile uint8_t *)SCSI_Capacity_Data;
                Bulk_Data_Len = 8;
                Bulk_Sending_Data = true;
                BOT_Send_Next_Chunk();
                break;

            case 0x28: // READ (10) - Stream directly from SDRAM without internal buffer limits
            {
                uint32_t lba = (cbw.CBWCB[2] << 24) | (cbw.CBWCB[3] << 16) | (cbw.CBWCB[4] << 8) | cbw.CBWCB[5];
                uint32_t transfer_blocks = (cbw.CBWCB[7] << 8) | cbw.CBWCB[8];
                if (transfer_blocks == 0) transfer_blocks = 1;

                UART0_Print("[READ(10)] LBA: ");
                UART0_PrintHex(lba);
                UART0_Print(" | Blocks: ");
                UART0_PrintHex(transfer_blocks);
                UART0_Print("\r\n");

                Bulk_Data_Ptr = (volatile uint8_t *)(0xA1000000 + (lba * 512));
                Bulk_Data_Len = transfer_blocks * 512;
                Bulk_Sending_Data = true;
                BOT_Send_Next_Chunk();
                break;
            }

            case 0x2A: // WRITE (10) - Stream chunks dynamically
            {
                uint32_t lba = (cbw.CBWCB[2] << 24) | (cbw.CBWCB[3] << 16) | (cbw.CBWCB[4] << 8) | cbw.CBWCB[5];

                UART0_Print("[WRITE(10)] Target LBA: ");
                UART0_PrintHex(lba);
                UART0_Print(" | Len: ");
                UART0_PrintHex(cbw.dCBWDataTransferLength);
                UART0_Print("\r\n");

                current_target_lba = lba;
                Remaining_Bytes_To_Receive = cbw.dCBWDataTransferLength;
                Write_Data_Ptr = SDRAM_DMA_BUFFER;
                Bulk_Expecting_Write_Data = true;
                break;
            }

            case 0xA1: // ATA PASS-THROUGH
            {
                UART0_Print("[SCSI] Handling ATA Pass-Through (0xA1)\r\n");
                uint32_t transfer_len = cbw.dCBWDataTransferLength;
                if (transfer_len > 0) {
                    for (uint32_t i = 0; i < transfer_len && i < 512; i++) {
                        SDRAM_DMA_BUFFER[i] = 0;
                    }
                    Bulk_Data_Ptr = SDRAM_DMA_BUFFER;
                    Bulk_Data_Len = (transfer_len < 512) ? transfer_len : 512;
                    Bulk_Sending_Data = true;
                    BOT_Send_Next_Chunk();
                } else {
                    csw.dCSWDataResidue = 0;
                    csw.bCSWStatus = 0;
                    USB_WriteEP(0x81, (uint8_t *)&csw, 13);
                }
                break;
            }

            default:
                UART0_Print("[SCSI WARNING] Unhandled Opcode: ");
                UART0_PrintHex(cbw.CBWCB[0]);
                UART0_Print("\r\n");

                if (cbw.dCBWDataTransferLength > 0) {
                    for (uint32_t i = 0; i < cbw.dCBWDataTransferLength && i < 512; i++) {
                        SDRAM_DMA_BUFFER[i] = 0;
                    }
                    Bulk_Data_Ptr = SDRAM_DMA_BUFFER;
                    Bulk_Data_Len = cbw.dCBWDataTransferLength < 512 ? cbw.dCBWDataTransferLength : 512;
                    Bulk_Sending_Data = true;
                    BOT_Send_Next_Chunk();
                } else {
                    csw.dCSWDataResidue = 0;
                    csw.bCSWStatus = 0;
                    USB_WriteEP(0x81, (uint8_t *)&csw, 13);
                }
                break;
        }
    }
}
