#ifndef SPI_H_
#define SPI_H_

#include <stdint.h>

/* ==================== SPI / SSP0 Core ==================== */

/*
 * Initializes the LPC2478 SSP0 peripheral for SPI Mode 0.
 * Configures pin multiplexing, clock speed, and the Chip Select GPIO.
 */
void spi_init(void);

/*
 * Transmits a single byte over MOSI and simultaneously returns
 * the received byte from MISO using polling mode.
 */
uint8_t spi_xfer(uint8_t out);


/* ==================== SD Card Interface ==================== */

/*
 * Sends a standard 6-byte command packet to the SD card.
 * cmd: The command index (e.g., 0 for CMD0)
 * arg: The 32-bit argument
 * crc: The pre-calculated CRC byte
 *
 * Returns the card's response byte (R1 format).
 */
uint8_t sd_send_command(uint8_t cmd, uint32_t arg, uint8_t crc);

#endif /* SPI_H_ */
