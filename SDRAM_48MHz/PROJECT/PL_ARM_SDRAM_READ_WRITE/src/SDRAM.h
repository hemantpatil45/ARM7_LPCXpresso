#ifndef SDRAM_H
#define SDRAM_H

// Timing Configuration for SDRAM (assuming 72 MHz HCLK)
#define HCLK_MHZ          72
#define P2C(ns)           (((ns) * HCLK_MHZ + 999) / 1000)

#define SDRAM_TRP         20    // Row Precharge time (ns)
#define SDRAM_TRAS        45    // Active to Precharge command period (ns)
#define SDRAM_TXSR        70    // Exit Self-Refresh to Active command time (ns)
#define SDRAM_TAPR        2     // Last Data In to Active delay (cycles)
#define SDRAM_TDAL        3     // Data-In to Active Command Delay (cycles)
#define SDRAM_TWR         2     // Write Recovery Time (cycles)
#define SDRAM_TRC         65    // Row Cycle Time (ns)
#define SDRAM_TRFC        65    // Auto Refresh Period / Row Cycle Time (ns)
#define SDRAM_TRRD        15    // Active Bank A to Active Bank B Command Period (ns)
#define SDRAM_TMRD        2     // Load Mode Register to Active Command Time (cycles)
#define SDRAM_REFRESH     64000 // Refresh interval (64ms)

// Function Prototypes
void SDRAM_init_32BIT(void);
void SDRAM_write(unsigned int No_of_data, unsigned int SDRAM_DAT[]);
void SDRAM_read(unsigned int No_of_data);

#endif
