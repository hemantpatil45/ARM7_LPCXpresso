#include "LPC24xx.h"
#include "sdcard.h"

// Chip Select Macro for P1.12[cite: 1]
#define SD_CS_LOW()  (FIO1CLR = (1 << 12))
#define SD_CS_HIGH() (FIO1SET = (1 << 12))

// Standard SPI SD Commands
#define CMD0   0  // GO_IDLE_STATE
#define CMD8   8  // SEND_IF_COND
#define CMD17  17 // READ_SINGLE_BLOCK
#define CMD24  24 // WRITE_BLOCK
#define CMD55  55 // APP_CMD
#define ACMD41 41 // SD_SEND_OP_COND

static uint8_t SPI_Transfer(uint8_t data) {
    S0SPDR = data;
    while (!(S0SPSR & (1 << 7))); // Wait for SPIF (Transfer Complete)
    return S0SPDR;
}

static uint8_t SD_Command(uint8_t cmd, uint32_t arg, uint8_t crc) {
    SD_CS_LOW();
    SPI_Transfer(0xFF); // Dummy byte
    SPI_Transfer(0x40 | cmd);
    SPI_Transfer((arg >> 24) & 0xFF);
    SPI_Transfer((arg >> 16) & 0xFF);
    SPI_Transfer((arg >> 8) & 0xFF);
    SPI_Transfer(arg & 0xFF);
    SPI_Transfer(crc);

    uint8_t response;
    for (int i = 0; i < 10; i++) {
        response = SPI_Transfer(0xFF);
        if ((response & 0x80) == 0) break;
    }
    return response;
}

bool SD_Init(void) {
    // 1. Configure SPI0 Pins on Port 0: P0.15 (SCK), P0.17 (MISO), P0.18 (MOSI)[cite: 1]
    PINSEL0 &= ~(3UL << 30); PINSEL0 |= (3UL << 30);
    PINSEL1 &= ~0x0000003C;  PINSEL1 |= 0x0000003C;

    // 2. Configure Chip Select Pin (P1.12 as GPIO Output)[cite: 1]
    PINSEL2 &= ~(3UL << 24);
    FIO1DIR |= (1 << 12);
    SD_CS_HIGH();

    // 3. Initialize SPI0 Peripheral at low speed (~400kHz) for initialization
    PCONP |= (1 << 8); // Power up SPI0
    PCLKSEL0 &= ~(3 << 16); PCLKSEL0 |= (1 << 16); // SPI PCLK = CCLK
    S0SPCCR = 180; // Divider for ~400kHz at 72MHz CPU
    S0SPCR = (1 << 5); // Master mode, 8-bit, CPOL=0, CPHA=0

    // 4. Send 80+ clock cycles with CS high to wake up the card
    for (int i = 0; i < 10; i++) {
        SPI_Transfer(0xFF);
    }

    // 5. Send CMD0 to enter SPI mode
    if (SD_Command(CMD0, 0, 0x95) != 0x01) {
        SD_CS_HIGH();
        return false;
    }

    // 6. Send CMD8 for voltage check
    SD_Command(CMD8, 0x000001AA, 0x87);

    // 7. Loop ACMD41 until the card is out of idle state
    uint16_t timeout = 0xFFFF;
    while (timeout--) {
        SD_Command(CMD55, 0, 0xFF);
        if (SD_Command(ACMD41, 0x00000000, 0xFF) == 0x00) break;
    }
    SD_CS_HIGH();

    if (timeout == 0) {
        return false; // Initialization timeout
    }

    // 8. Ramp up SPI clock to full speed for high-speed data transfers
    S0SPCCR = 4;
    return true;
}

bool SD_Read_Block(uint32_t lba, uint8_t *buffer) {
    // Standard SD cards use byte addressing (LBA * 512 bytes)
    if (SD_Command(CMD17, lba * 512, 0xFF) != 0x00) {
        SD_CS_HIGH();
        return false;
    }

    // Wait for the data token (0xFE)
    while (SPI_Transfer(0xFF) != 0xFE);

    // Read 512 bytes of sector data
    for (int i = 0; i < 512; i++) {
        buffer[i] = SPI_Transfer(0xFF);
    }

    // Discard the 2-byte CRC checksum
    SPI_Transfer(0xFF);
    SPI_Transfer(0xFF);

    SD_CS_HIGH();
    return true;
}

bool SD_Write_Block(uint32_t lba, uint8_t *buffer) {
    // Standard SD cards use byte addressing (LBA * 512 bytes)
    if (SD_Command(CMD24, lba * 512, 0xFF) != 0x00) {
        SD_CS_HIGH();
        return false;
    }

    SPI_Transfer(0xFF); // Dummy byte before data token
    SPI_Transfer(0xFE); // Data start token

    // Write 512 bytes of sector data
    for (int i = 0; i < 512; i++) {
        SPI_Transfer(buffer[i]);
    }

    // Send 2-byte dummy CRC
    SPI_Transfer(0xFF);
    SPI_Transfer(0xFF);

    // Verify data response token (Accepted response format is xxx0101b -> 0x05)
    if ((SPI_Transfer(0xFF) & 0x1F) != 0x05) {
        SD_CS_HIGH();
        return false;
    }

    // Wait while card writes data internally (returns 0x00 while busy)
    while (SPI_Transfer(0xFF) == 0x00);

    SD_CS_HIGH();
    return true;
}
