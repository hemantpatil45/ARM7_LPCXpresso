#include "LPC24xx.h"
#include "ethernet.h"
#include "ethernet_reg.h"
#include "system_init.h"
#include <stdio.h>

unsigned int *Descriptor_pt;
unsigned int RX_BUF_1[4] = {0x7FE00064, 0x7FE00664, 0x7FE00C64, 0x7FE01264};
unsigned int TX_BUF[4]   = {0x7FE01864, 0x7FE01E64, 0x7FE02464};

extern volatile unsigned char key;

void write_PHY(unsigned char PhyRegW, unsigned short ValueW)
{
    EMAC_MCMD = 0x0000;
    EMAC_MADR = (0x01 << 8) | PhyRegW;
    EMAC_MWTD = ValueW;
    while (EMAC_MIND & 0x01);
}

unsigned short read_PHY(unsigned char PhyRegR)
{
    EMAC_MADR = (0x01 << 8) | PhyRegR;
    EMAC_MCMD = 0x0001;
    while (EMAC_MIND & 0x01);
    EMAC_MCMD = 0x0000;
    return (unsigned short)(EMAC_MRDD & 0xFFFF);
}

void rx_descr_init(void)
{
    unsigned int i;
    Descriptor_pt = (unsigned int*)0x7FE00000;

    for(i = 0; i < 4; i++) {
        *Descriptor_pt++ = RX_BUF_1[i];
        *Descriptor_pt++ = 0x80000000 | (1536 - 1);
    }

    Descriptor_pt = (unsigned int*)0x7FE00020;
    for(i = 0; i < 4; i++) {
        *Descriptor_pt++ = 0;
        *Descriptor_pt++ = 0;
    }

    EMAC_RXDESCRIPTOR    = 0x7FE00000;
    EMAC_RXSTATUS        = 0x7FE00020;
    EMAC_RXDESCRIPTORNUM = 4 - 1;
    EMAC_RXCONSUMEINDEX  = 0;
}

void tx_descr_init(void)
{
    unsigned int i;
    Descriptor_pt = (unsigned int*)0x7FE00040;

    for(i = 0; i < 3; i++) {
        *Descriptor_pt++ = TX_BUF[i];
        *Descriptor_pt++ = 0xF000004D;
    }

    Descriptor_pt = (unsigned int*)0x7FE00058;
    for(i = 0; i < 3; i++) {
        *Descriptor_pt++ = 0;
    }

    EMAC_TXDESCRIPTOR    = 0x7FE00040;
    EMAC_TXSTATUS        = 0x7FE00058;
    EMAC_TXDESCRIPTORNUM = 3 - 1;
    EMAC_TXPRODUCEINDEX  = 0;
}

void EMAC_init(void)
{
    char string_buff[32];
    unsigned int phy_id;

    PCONP |= (1 << 30); // Supply power to Ethernet Engine
    delay_us(10000);

    EMAC_POWERDOWN = 0;
    delay_us(10000);

    EMAC_MAC1    = 0x0000CF00;
    EMAC_COMMAND = 0x00000038;
    delay_us(10000);

    EMAC_MAC1    = 0x00000000;
    EMAC_COMMAND = (1 << 9);   // Map operation to RMII interface
    EMAC_SUPP    = 0;          // Set bandwidth transmission to 10Mbps
    EMAC_TEST    = 0;

    /* Set Local MAC Address Config: AB:38:21:1C:2B:01 */
    EMAC_SA0 = (0x38 << 8) | 0xAB;
    EMAC_SA1 = (0x1C << 8) | 0x21;
    EMAC_SA2 = (0x01 << 8) | 0x2B;

    EMAC_MAXF = 0x0600;        // Set max payload frame limit to 1536
    EMAC_MCFG = 0x801C;
    EMAC_MCMD = 0;
    EMAC_MCFG = 0x001C;

    EMAC_COMMAND |= (1 << 6) | (1 << 3);
    EMAC_MAC1    |= (1 << 1);

    EMAC_MAC2    = 0x00000030;
    EMAC_CLRT    = 0x0000370F;
    EMAC_IPGR    = 0x00000C12;
    EMAC_IPGT    = 0x00000012;

    write_PHY(0x00, 0x8000); // PHY reset
    delay_us(20000);

    row_wr(0, "WAIT TO RESET PHY   ");
    delay_us(50000);

    EMAC_MCFG |= (1 << 2);
    write_PHY(0x00, 0x1000);   // Initiate Link Auto-Negotiation

    row_wr(1, "AFTER COMPLETION OF ");
    row_wr(2, "RESET OF BOTH RX/TX ");
    row_wr(3, "PRESS SW2 KEY       ");

    while(key != 0x30);        // Block processing loop until SW2 is toggled

    /* Monitor PHY link status */
    while((read_PHY(0x01) & 0x0004) != 0x0004);
    row_wr(7, "LINK OOOOOOOK       ");

    phy_id = read_PHY(0x00);
    sprintf(string_buff, "0x%04X              ", phy_id);
    row_wr(0, string_buff);

    phy_id = read_PHY(0x01);
    sprintf(string_buff, "0x%04X              ", phy_id);
    row_wr(1, string_buff);

    phy_id = read_PHY(0x02);
    sprintf(string_buff, "0x%04X              ", phy_id);
    row_wr(2, string_buff);

    phy_id = read_PHY(0x03);
    sprintf(string_buff, "0x%04X              ", phy_id);
    row_wr(3, string_buff);

    rx_descr_init();
    tx_descr_init();

    EMAC_RXFILTERCTRL = (1 << 5) | (1 << 3) | (1 << 1);

    EMAC_COMMAND |= (1 << 0) | (1 << 1);
    EMAC_MAC1    |= (1 << 0);
}
