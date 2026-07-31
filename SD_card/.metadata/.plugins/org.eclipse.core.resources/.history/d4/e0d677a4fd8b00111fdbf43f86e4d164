#ifndef MFRC522_H_
#define MFRC522_H_

#include <stdint.h>

/* Single cascade-level UID (4 bytes) -- covers MIFARE Classic
 * Mini/1K/4K, the same tag types the reference Arduino sketch
 * filters for. 7/10-byte UID cards (cascade level 2/3) are not
 * handled here. */
typedef struct {
    uint8_t size;          /* always 4 in this driver */
    uint8_t uidByte[4];
    uint8_t sak;           /* Select Acknowledge byte from the PICC */
} MFRC522_UID;

/* Resets and configures the reader (SPI, antenna, timer). Call once. */
void PCD_Init(void);

/* Returns non-zero if a PICC answered REQA (a card is in the field). */
uint8_t PICC_IsNewCardPresent(void);

/* Runs anticollision + select for a single card, fills *uid.
 * Returns non-zero on success. Call only after PICC_IsNewCardPresent()
 * returned true. */
uint8_t PICC_ReadCardSerial(MFRC522_UID *uid);

/* Puts the currently selected PICC back to HALT state. */
uint8_t PICC_HaltA(void);

#endif /* MFRC522_H_ */
