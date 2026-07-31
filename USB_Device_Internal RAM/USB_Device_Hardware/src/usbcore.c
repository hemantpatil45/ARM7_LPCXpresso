#include "LPC24xx.h"
#include "usbcore.h"
#include "usbhw.h"
#include "usbdesc.h"

USB_SETUP_PACKET SetupPacket;
USB_CBW cbw;
USB_CSW csw;

// 48 KB RAM Disk (Exactly 96 Sectors of 512 bytes)
uint8_t RAM_Disk[49152];

uint8_t *Bulk_Data_Ptr;
uint32_t Bulk_Data_Len;
bool Bulk_Sending_Data = false;

uint8_t *Write_Data_Ptr;
uint32_t Write_Data_Len;
bool Bulk_Expecting_Write_Data = false;

void Init_RAM_Disk(void) {
    // 1. Clear the physical 32 KB memory to zeroes
    for(int i = 0; i < 32768; i++) RAM_Disk[i] = 0;

    // 2. Build the FAT16 Boot Sector (Sector 0)
    RAM_Disk[0x00] = 0xEB; RAM_Disk[0x01] = 0x3C; RAM_Disk[0x02] = 0x90; // JMP
    RAM_Disk[0x03] = 'M';  RAM_Disk[0x04] = 'S';  RAM_Disk[0x05] = 'D';  RAM_Disk[0x06] = 'O';
    RAM_Disk[0x07] = 'S';  RAM_Disk[0x08] = '5';  RAM_Disk[0x09] = '.';  RAM_Disk[0x0A] = '0';

    // Bytes per sector: 512 (0x0200)
    RAM_Disk[0x0B] = 0x00; RAM_Disk[0x0C] = 0x02;
    // Sectors per cluster: 1
    RAM_Disk[0x0D] = 0x01;
    // Reserved sector count: 1 (The boot sector itself)
    RAM_Disk[0x0E] = 0x01; RAM_Disk[0x0F] = 0x00;
    // Number of FAT tables: 2
    RAM_Disk[0x10] = 0x02;
    // Maximum root directory entries: 512 (0x0200)
    RAM_Disk[0x11] = 0x00; RAM_Disk[0x12] = 0x02;
    // Total sectors: 8192 (0x2000)
    RAM_Disk[0x13] = 0x00; RAM_Disk[0x14] = 0x20;
    // Media descriptor (Fixed disk: 0xF8)
    RAM_Disk[0x15] = 0xF8;
    // Sectors per FAT: 32 (0x0020)
    RAM_Disk[0x16] = 0x20; RAM_Disk[0x17] = 0x00;

    // Boot signature at the end of Sector 0
    RAM_Disk[510] = 0x55;  RAM_Disk[511] = 0xAA;

    // 3. Initialize FAT Table 1 (Starts at Sector 1 -> Byte offset 512)
    RAM_Disk[512 + 0] = 0xF8;
    RAM_Disk[512 + 1] = 0xFF;
    RAM_Disk[512 + 2] = 0xFF;
    RAM_Disk[512 + 3] = 0xFF;

    // 4. Initialize FAT Table 2
    // (Starts after 1 Reserved + 32 FAT1 Sectors = Sector 33 -> Byte offset 16896)
    RAM_Disk[16896 + 0] = 0xF8;
    RAM_Disk[16896 + 1] = 0xFF;
    RAM_Disk[16896 + 2] = 0xFF;
    RAM_Disk[16896 + 3] = 0xFF;
}

const uint8_t SCSI_Inquiry_Data[36] = {
    0x00, 0x80, 0x02, 0x02, 31, 0x00, 0x00, 0x00,
    'L', 'P', 'C', '2', '4', '7', '8', ' ',
    'D', 'e', 'v', 'i', 'c', 'e', ' ', ' ',
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
    '1', '.', '0', '0'
};

// FAKE CAPACITY: 2.14 MB (4,192 sectors of 512 bytes)
const uint8_t SCSI_Capacity_Data[8] = {
    0x00, 0x00, 0x10, 0x5F, // Last LBA: 4191 (0x0000105F)
    0x00, 0x00, 0x02, 0x00  // Block Length: 512 bytes (0x00000200)
};

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
        if (Bulk_Data_Ptr != 0) { // If inside real RAM
            USB_WriteEP(0x81, Bulk_Data_Ptr, chunk);
            Bulk_Data_Ptr += chunk;
        } else { // Out of bounds: send fake empty zeroes
            uint8_t zeroes[64] = {0};
            USB_WriteEP(0x81, zeroes, chunk);
        }
        Bulk_Data_Len -= chunk;
    } else if (Bulk_Sending_Data) {
        Bulk_Sending_Data = false;
        USB_WriteEP(0x81, (uint8_t *)&csw, 13);
    }
}

void Process_Bulk_OUT(void) {
    // 1. DATA PHASE
    if (Bulk_Expecting_Write_Data) {
        uint8_t temp[64];
        uint32_t count;

        if (Write_Data_Ptr != 0) {
            count = USB_ReadEP(0x02, Write_Data_Ptr);
            Write_Data_Ptr += count;
        } else {
            count = USB_ReadEP(0x02, temp); // Dump out-of-bounds data into the void
        }

        if (Write_Data_Len > count) Write_Data_Len -= count;
        else Write_Data_Len = 0;

        if (Write_Data_Len == 0) {
            Bulk_Expecting_Write_Data = false;
            csw.bCSWStatus = 0;
            USB_WriteEP(0x81, (uint8_t *)&csw, 13);
        }
        return;
    }

    // 2. COMMAND PHASE
    uint32_t count = USB_ReadEP(0x02, (uint8_t *)&cbw);
    if (count == 31 && cbw.dCBWSignature == 0x43425355) {
        csw.dCSWSignature = 0x53425355;
        csw.dCSWTag = cbw.dCBWTag;
        csw.dCSWDataResidue = 0;
        csw.bCSWStatus = 0;

        switch (cbw.CBWCB[0]) {
            case 0x00: USB_WriteEP(0x81, (uint8_t *)&csw, 13); break;
            case 0x03:
                Bulk_Data_Ptr = (uint8_t[]){0x70, 0, 0x05, 0, 0, 0, 0, 10, 0, 0, 0, 0, 0x30, 0x01, 0, 0, 0, 0};
                Bulk_Data_Len = 18; Bulk_Sending_Data = true; BOT_Send_Next_Chunk(); break;
            case 0x12:
                Bulk_Data_Ptr = (uint8_t *)SCSI_Inquiry_Data; Bulk_Data_Len = sizeof(SCSI_Inquiry_Data);
                Bulk_Sending_Data = true; BOT_Send_Next_Chunk(); break;
            case 0x1A:
                Bulk_Data_Ptr = (uint8_t[]){0x03, 0x00, 0x00, 0x00}; Bulk_Data_Len = 4;
                Bulk_Sending_Data = true; BOT_Send_Next_Chunk(); break;
            case 0x25:
                Bulk_Data_Ptr = (uint8_t *)SCSI_Capacity_Data; Bulk_Data_Len = 8;
                Bulk_Sending_Data = true; BOT_Send_Next_Chunk(); break;

            case 0x28: // READ
            {
                uint32_t lba = (cbw.CBWCB[2] << 24) | (cbw.CBWCB[3] << 16) | (cbw.CBWCB[4] << 8) | cbw.CBWCB[5];
                uint16_t blocks = (cbw.CBWCB[7] << 8) | cbw.CBWCB[8];
                uint32_t byte_offset = lba * 512;

                if ((byte_offset + (blocks * 512)) <= sizeof(RAM_Disk)) Bulk_Data_Ptr = &RAM_Disk[byte_offset];
                else Bulk_Data_Ptr = 0; // Null pointer means out-of-bounds

                Bulk_Data_Len = blocks * 512;
                Bulk_Sending_Data = true;
                BOT_Send_Next_Chunk();
                break;
            }

            case 0x2A: // WRITE
            {
                uint32_t lba = (cbw.CBWCB[2] << 24) | (cbw.CBWCB[3] << 16) | (cbw.CBWCB[4] << 8) | cbw.CBWCB[5];
                uint16_t blocks = (cbw.CBWCB[7] << 8) | cbw.CBWCB[8];
                uint32_t byte_offset = lba * 512;

                if ((byte_offset + (blocks * 512)) <= sizeof(RAM_Disk)) Write_Data_Ptr = &RAM_Disk[byte_offset];
                else Write_Data_Ptr = 0; // Null pointer means drop data

                Write_Data_Len = blocks * 512;
                Bulk_Expecting_Write_Data = true;
                break;
            }

            default: USB_WriteEP(0x81, (uint8_t *)&csw, 13); break;
        }
    }
}
