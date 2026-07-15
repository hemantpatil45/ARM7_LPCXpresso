#include "ethernet_reg.h"

/* Instantiate Variables Globally */
union DataBus data;
union RetBus ret_bus;
unsigned char *Flash_Pointer1;
unsigned int *SDRAM_pointer;
unsigned char *p;
unsigned int *ADC_DATA_RECIEVED;
unsigned char buffer[50];
unsigned int *Descriptor_pt;
unsigned int RX_BUF[4] = {0x7FE00064, 0x7FE00664, 0x7FE00C64, 0x7FE01264};
unsigned int TX_BUF[4] = {0x7FE01864, 0x7FE01E64, 0x7FE02464};
unsigned int RX_BUF_1[4] = {0x7FE00064, 0x7FE00664, 0x7FE00C64, 0x7FE01264};
unsigned char *RXDESBUFFER_1;
unsigned int smpl_tmr;
float *adc_val;
unsigned char key = 0xFF;
float INTER_ADC;
unsigned int AD7492_DATA;

#define CFG_PINSEL(REG, PIN, VAL) (REG) = ((REG) & ~(3UL << ((PIN) * 2))) | ((VAL) << ((PIN) * 2))

void delay(int y) {
    while(y) { y--; }
}

void delay_sec(unsigned int delay_val) {
    smpl_tmr = 0;
    while(smpl_tmr != delay_val) {}
}

void VERIABLE_INIT_FUN(unsigned int ver1, unsigned int ver2, unsigned int ver3) {
    p = (unsigned char*)ver1;
    adc_val = (float*)ver2;
    ADC_DATA_RECIEVED = (unsigned int*)ver3;
}

static void Eth_InitPins(void) {
    PCONP |= (1 << 30);
    PINSEL2 = (PINSEL2 & ~0xF03F030F) | 0x50150105;
    PINSEL3 = (PINSEL3 & ~0x0000000F) | 0x00000005;
}

void Eth_WritePHY(uint32_t PhyReg, uint16_t Value) {
    LPC_EMAC->MCMD = 0;
    LPC_EMAC->MADR = DP83848_DEF_ADR | PhyReg;
    LPC_EMAC->MWTD = Value;
    while ((LPC_EMAC->MIND & 1) != 0);
}

uint16_t Eth_ReadPHY(uint32_t PhyReg) {
    LPC_EMAC->MADR = DP83848_DEF_ADR | PhyReg;
    LPC_EMAC->MCMD = 1;
    while ((LPC_EMAC->MIND & 1) != 0);
    LPC_EMAC->MCMD = 0;
    return (uint16_t)(LPC_EMAC->MRDD & 0xFFFF);
}

void rx_descr_init(void) {
    uint32_t i;
    Descriptor_pt = (unsigned int*)(0x7FE00000);
    for (i = 0; i < NUM_RX_FRAG; i++) {
        *Descriptor_pt = RX_BUF[i]; Descriptor_pt++;
        *Descriptor_pt = 0x00000000 | (ETH_FRAG_SIZE - 1); Descriptor_pt++;
    }
    Descriptor_pt = (unsigned int*)(0x7FE00020);
    for (i = 0; i < NUM_RX_FRAG; i++) {
        *Descriptor_pt = 0; Descriptor_pt++;
        *Descriptor_pt = 0; Descriptor_pt++;
    }
    LPC_EMAC->RXDESCRIPTOR = 0x7FE00000;
    LPC_EMAC->RXSTATUS = 0x7FE00020;
    LPC_EMAC->RXDESCRIPTORNUM = NUM_RX_FRAG - 1;
    LPC_EMAC->RXCONSUMEINDEX = 0;
}

void tx_descr_init(void) {
    uint32_t i;
    Descriptor_pt = (unsigned int*)(0x7FE00040);
    for (i = 0; i < NUM_TX_FRAG; i++) {
        *Descriptor_pt = TX_BUF[i]; Descriptor_pt++;
        *Descriptor_pt = 0XF000004D; Descriptor_pt++;
    }
    Descriptor_pt = (unsigned int*)(0x7FE00058);
    for (i = 0; i < NUM_TX_FRAG; i++) {
        *Descriptor_pt = 0; Descriptor_pt++;
    }
    LPC_EMAC->TXDESCRIPTOR = 0x7FE00040;
    LPC_EMAC->TXSTATUS = 0x7FE00058;
    LPC_EMAC->TXDESCRIPTORNUM = NUM_TX_FRAG - 1;
    LPC_EMAC->TXPRODUCEINDEX = 0;
}

bool Eth_Init(void) {
    uint32_t timeout;
    uint16_t phyStatus;

    Eth_InitPins();
    LPC_EMAC->MAC1 = MAC1_RES_TX | MAC1_RES_MCS_TX | MAC1_RES_RX | MAC1_RES_MCS_RX | MAC1_SIM_RES | MAC1_SOFT_RES;
    LPC_EMAC->COMMAND = CR_REG_RES | CR_TX_RES | CR_RX_RES;
    for (timeout = 100; timeout; timeout--);

    LPC_EMAC->MAC1 = MAC1_PASS_ALL | MAC1_REC_EN;
    LPC_EMAC->COMMAND = CR_RMII;
    LPC_EMAC->MCFG = 0x0018;
    LPC_EMAC->MAC2 = MAC2_CRC_EN | MAC2_PAD_EN;
    LPC_EMAC->MAXF = 1536;
    LPC_EMAC->CLRT = 0x370F;
    LPC_EMAC->IPGR = 0x0C12;
    LPC_EMAC->IPGT = 0x0015;

    Eth_WritePHY(DP83848_BMCR, PHY_BMCR_RESET);
    for (timeout = 0; timeout < 0xFFFFF; timeout++) {
        if ((Eth_ReadPHY(DP83848_BMCR) & PHY_BMCR_RESET) == 0) break;
    }

    Eth_WritePHY(DP83848_BMCR, PHY_BMCR_AUTONEG_EN | PHY_BMCR_RESTART_AN);
    for (timeout = 0; timeout < 0xFFFFF; timeout++) {
        phyStatus = Eth_ReadPHY(DP83848_BMSR);
        if ((phyStatus & PHY_BMSR_LINK_STAT) && (phyStatus & PHY_BMSR_AUTONEG_C)) break;
    }
    if (timeout == 0xFFFFF) return false;

    phyStatus = Eth_ReadPHY(DP83848_PHYSTS);
    if (phyStatus & PHY_STS_DUPLEX) {
        LPC_EMAC->MAC2 |= MAC2_FULL_DUP;
        LPC_EMAC->COMMAND |= CR_FULL_DUP;
        LPC_EMAC->IPGT = 0x0015;
    } else {
        LPC_EMAC->IPGT = 0x0012;
    }
    LPC_EMAC->SUPP = (phyStatus & PHY_STS_SPEED) ? 0 : (1 << 8);

    LPC_EMAC->SA0 = (MAC_1 << 8) | MAC_2;
    LPC_EMAC->SA1 = (MAC_3 << 8) | MAC_4;
    LPC_EMAC->SA2 = (MAC_5 << 8) | MAC_6;

    rx_descr_init();
    tx_descr_init();

    LPC_EMAC->RXFILTERCTRL = RXFILT_CTRL_BROADCAST | RXFILT_CTRL_PERFECT;
    LPC_EMAC->INTENABLE = 0;
    LPC_EMAC->INTCLEAR = 0xFFFF;
    LPC_EMAC->COMMAND |= (CR_RX_EN | CR_TX_EN);
    LPC_EMAC->MAC1 |= MAC1_REC_EN;

    return true;
}

void Uart_0_init_115200_WITH_PLL(void) {
    U0FCR = 0x01;
    U0LCR = 0x83;
    U0DLL = BAUDRATEDIVISOR_115200_PLL & 0xFF;
    U0DLM = (BAUDRATEDIVISOR_115200_PLL >> 8) & 0xFF;
    U0LCR = 0x03;
    U0IER = 0x00;
}

void Init_timer0(void) {
    T0TCR = 0;
    T0PR = 0x0;
    T0MR0 = 0x24000;
    T0MCR = 0x03;
    T0CCR = 0;
    T0EMR = 0;
}

void SDRAM_init_32BIT(void) {
    PCONP |= (1 << 11);
    EMC_CTRL = 1;
    EMC_DYN_RD_CFG = 1;
    EMC_DYN_RASCAS0 = 0x00000303;
    EMC_DYN_RP = P2C(SDRAM_TRP);
    EMC_DYN_RAS = P2C(SDRAM_TRAS);
    EMC_DYN_SREX = P2C(SDRAM_TXSR);
    EMC_DYN_APR = SDRAM_TAPR;
    EMC_DYN_DAL = SDRAM_TDAL + P2C(SDRAM_TRP);
    EMC_DYN_WR = SDRAM_TWR;
    EMC_DYN_RC = P2C(SDRAM_TRC);
    EMC_DYN_RFC = P2C(SDRAM_TRFC);
    EMC_DYN_XSR = P2C(SDRAM_TXSR);
    EMC_DYN_RRD = P2C(SDRAM_TRRD);
    EMC_DYN_MRD = SDRAM_TMRD;
    EMC_DYN_CFG0 = 0x0004680;

    EMC_DYN_CTRL = 0x0183;
    for(volatile int i = 200 * 30; i; i--);

    EMC_DYN_CTRL = 0x0103 | (2 << 7);
    EMC_DYN_RFSH = 1;
    for(volatile int i = 128; i; --i);
    EMC_DYN_RFSH = P2C(SDRAM_REFRESH) >> 4;

    EMC_DYN_CTRL = 0x0103 | (1 << 7);
    volatile unsigned long Dummy = *(SDRAM_pointer + 0x32);
    (void)Dummy;

    EMC_DYN_CTRL = 0x0000;
    EMC_DYN_CFG0 |= (1 << 19);
    for(volatile int i = 10000; i; i--);
}

void sys_init_without_TFT_32bit_SDRAM(void) {
    MEMMAP = 0x01;
    *((volatile uint32_t *)(SCB_BASE_ADDR + 0x188)) = 0x00000412;
    SCS = 0x21;
    while((SCS & (1 << 6)) == 0);

    CLKSRCSEL = 1;
    PLLCFG = 192 | (16 << 16);
    PLLFEED = 0xAA; PLLFEED = 0x55;
    PLLCON = 1;
    PLLFEED = 0xAA; PLLFEED = 0x55;
    CCLKCFG = 3;
    while((PLLSTAT & (1 << 26)) == 0);

    PLLCON = 3;
    PLLFEED = 0xAA; PLLFEED = 0x55;

    PCLKSEL0 = 0; PCLKSEL1 = 0;
    PCONP |= (1 << 3) | (1 << 1) | (1 << 11);

    CFG_PINSEL(PINSEL0, 2, 1);  CFG_PINSEL(PINSEL0, 3, 1);
    CFG_PINSEL(PINSEL0, 0, 1);  CFG_PINSEL(PINSEL0, 1, 1);
    CFG_PINSEL(PINSEL0, 4, 0);  CFG_PINSEL(PINSEL0, 5, 0);  CFG_PINSEL(PINSEL0, 6, 0);
    CFG_PINSEL(PINSEL3, (26-16), 0); CFG_PINSEL(PINSEL3, (28-16), 0); CFG_PINSEL(PINSEL3, (29-16), 0);
    CFG_PINSEL(PINSEL0, 7, 0);  CFG_PINSEL(PINSEL0, 8, 0);  CFG_PINSEL(PINSEL0, 9, 0);
    CFG_PINSEL(PINSEL3, (20-16), 0); CFG_PINSEL(PINSEL3, (21-16), 0); CFG_PINSEL(PINSEL3, (23-16), 0);
    CFG_PINSEL(PINSEL3, (24-16), 0); CFG_PINSEL(PINSEL3, (25-16), 0);
    CFG_PINSEL(PINSEL0, 10, 0);
    CFG_PINSEL(PINSEL4, 0, 0);  CFG_PINSEL(PINSEL4, 1, 0);
    CFG_PINSEL(PINSEL5, (22-16), 0); CFG_PINSEL(PINSEL5, (23-16), 0); CFG_PINSEL(PINSEL5, (25-16), 0);
    CFG_PINSEL(PINSEL5, (26-16), 0); CFG_PINSEL(PINSEL5, (27-16), 0);
    CFG_PINSEL(PINSEL0, 11, 0);
    CFG_PINSEL(PINSEL1, (19-16), 2); CFG_PINSEL(PINSEL1, (20-16), 2); CFG_PINSEL(PINSEL1, (22-16), 2);
    CFG_PINSEL(PINSEL4, 11, 2);
    CFG_PINSEL(PINSEL2, 11, 2); CFG_PINSEL(PINSEL2, 12, 2);
    CFG_PINSEL(PINSEL1, (23-16), 1); CFG_PINSEL(PINSEL1, (26-16), 2);
    CFG_PINSEL(PINSEL2, 7, 0);  CFG_PINSEL(PINSEL4, 14, 0); CFG_PINSEL(PINSEL1, (25-16), 0);
    CFG_PINSEL(PINSEL0, 15, 3); CFG_PINSEL(PINSEL1, (16-16), 3); CFG_PINSEL(PINSEL1, (17-16), 3); CFG_PINSEL(PINSEL1, (18-16), 3);
    CFG_PINSEL(PINSEL1, (27-16), 1); CFG_PINSEL(PINSEL1, (28-16), 1);
    CFG_PINSEL(PINSEL1, (21-16), 0); CFG_PINSEL(PINSEL1, (24-16), 0);
    CFG_PINSEL(PINSEL2, 2, 0);  CFG_PINSEL(PINSEL2, 3, 0);  CFG_PINSEL(PINSEL2, 5, 0);  CFG_PINSEL(PINSEL2, 6, 0);
    CFG_PINSEL(PINSEL5, (19-16), 0); CFG_PINSEL(PINSEL5, (21-16), 0);
    CFG_PINSEL(PINSEL2, 13, 0); CFG_PINSEL(PINSEL4, 15, 0);
    CFG_PINSEL(PINSEL9, (23-16), 0); CFG_PINSEL(PINSEL9, (26-16), 0); CFG_PINSEL(PINSEL9, (27-16), 0); CFG_PINSEL(PINSEL9, (31-16), 0);
    CFG_PINSEL(PINSEL4, 10, 0);
    CFG_PINSEL(PINSEL2, 0, 1);  CFG_PINSEL(PINSEL2, 1, 1);  CFG_PINSEL(PINSEL2, 4, 1);
    CFG_PINSEL(PINSEL2, 9, 1);  CFG_PINSEL(PINSEL2, 10, 1); CFG_PINSEL(PINSEL2, 14, 1);
    CFG_PINSEL(PINSEL2, 15, 1); CFG_PINSEL(PINSEL2, 8, 1);
    CFG_PINSEL(PINSEL3, (16-16), 1); CFG_PINSEL(PINSEL3, (17-16), 1);
    CFG_PINSEL(PINSEL0, 12, 1); CFG_PINSEL(PINSEL0, 13, 1); CFG_PINSEL(PINSEL0, 14, 0);
    CFG_PINSEL(PINSEL1, (31-16), 1); CFG_PINSEL(PINSEL3, (30-16), 1); CFG_PINSEL(PINSEL3, (31-16), 1);
    CFG_PINSEL(PINSEL1, (29-16), 1); CFG_PINSEL(PINSEL1, (30-16), 1); CFG_PINSEL(PINSEL3, (18-16), 1); CFG_PINSEL(PINSEL3, (19-16), 1);
    CFG_PINSEL(PINSEL9, (24-16), 1); CFG_PINSEL(PINSEL9, (30-16), 1);
    CFG_PINSEL(PINSEL5, (20-16), 1); CFG_PINSEL(PINSEL5, (16-16), 1); CFG_PINSEL(PINSEL5, (17-16), 1);
    CFG_PINSEL(PINSEL5, (18-16), 1); CFG_PINSEL(PINSEL5, (24-16), 1); CFG_PINSEL(PINSEL5, (28-16), 1);
    CFG_PINSEL(PINSEL5, (29-16), 1); CFG_PINSEL(PINSEL5, (30-16), 1); CFG_PINSEL(PINSEL5, (31-16), 1);
    CFG_PINSEL(PINSEL9, (25-16), 1);

    PINSEL8 = 0x55555555;
    CFG_PINSEL(PINSEL9, 0, 1);  CFG_PINSEL(PINSEL9, 1, 1);  CFG_PINSEL(PINSEL9, 2, 1);
    CFG_PINSEL(PINSEL9, 3, 1);  CFG_PINSEL(PINSEL9, 4, 1);  CFG_PINSEL(PINSEL9, 5, 1);
    CFG_PINSEL(PINSEL9, 6, 1);

    PINSEL6 = 0x55555555;
    PINSEL7 = 0x55555555;

    PINSEL10 = 0X00000000; PINSEL11 = 0X0000000F;
    PINMODE0 = 0x00000000; PINMODE1 = 0x00000000; PINMODE2 = 0x00000000; PINMODE3 = 0x00000000;
    PINMODE4 = 0x00000000; PINMODE5 = 0x00000000; PINMODE6 = 0XAAAAAAAA; PINMODE7 = 0XAAAAAAAA;
    PINMODE8 = 0XAAAAAAAA; PINMODE9 = 0X200A2AAA;

    FIO0DIR = 0X03204FF0; FIO1DIR = 0X3FF020EC; FIO2DIR = 0X0E28F3FC; FIO3DIR = 0X70000000; FIO4DIR = 0X30000000;
    FIO0SET = 0X02004000; FIO1SET = 0X00002080; FIO2SET = 0X0028C000; FIO3SET = 0X70000000; FIO4SET = 0X00000000;
    FIO0CLR = 0X01200FF0; FIO1CLR = 0X3FF0006C; FIO2CLR = 0X0E0033FC; FIO3CLR = 0X00000000; FIO4CLR = 0X30000000;
    FIO0MASK = 0; FIO1MASK = 0; FIO2MASK = 0; FIO3MASK = 0; FIO4MASK = 0;
}

void ret_asign(void) {
    ret_bus.ret_bus_bits.i0 = (FIO2PIN & (1 << 0)) ? 1 : 0;
    ret_bus.ret_bus_bits.i1 = (FIO2PIN & (1 << 1)) ? 1 : 0;
    ret_bus.ret_bus_bits.i2 = (FIO2PIN & (1 << 22)) ? 1 : 0;
    ret_bus.ret_bus_bits.i3 = (FIO2PIN & (1 << 23)) ? 1 : 0;
}

void kbd_chk_scn0(void) {
    unsigned char curr_sts; ret_asign(); curr_sts = ret_bus.bus | 0xF0;
    if(curr_sts == 0xFE) *p = 0x30; if(curr_sts == 0xFD) *p = 0x31;
    if(curr_sts == 0xFB) *p = 0x32; if(curr_sts == 0xF7) *p = 0x33;
}
void kbd_chk_scn1(void) {
    unsigned char curr_sts; ret_asign(); curr_sts = ret_bus.bus | 0xF0;
    if(curr_sts == 0xFE) *p = 0x34; if(curr_sts == 0xFD) *p = 0x35;
    if(curr_sts == 0xFB) *p = 0x36; if(curr_sts == 0xF7) *p = 0x37;
}
void kbd_chk_scn2(void) {
    unsigned char curr_sts; ret_asign(); curr_sts = ret_bus.bus | 0xF0;
    if(curr_sts == 0xFE) *p = 0x38; if(curr_sts == 0xFD) *p = 0x39;
    if(curr_sts == 0xFB) *p = 0x65; if(curr_sts == 0xF7) *p = 0x66;
}
void kbd_chk_scn3(void) {
    unsigned char curr_sts; ret_asign(); curr_sts = ret_bus.bus | 0xF0;
    if(curr_sts == 0xFE) *p = 0x67; if(curr_sts == 0xFD) *p = 0x68;
    if(curr_sts == 0xFB) *p = 0x69; if(curr_sts == 0xF7) *p = 0x6A;
}

void kbd_scan(void) {
    T0TCR = 0x00; T0IR = 0x01;
    smpl_tmr++;
    if(smpl_tmr == 0xFFFFFFFF) smpl_tmr = 0x00;
    if(*p != 0xFF) *p = 0xFF;

    scn0_0; kbd_chk_scn0(); scn0_1;
    scn1_0; kbd_chk_scn1(); scn1_1;
    scn2_0; kbd_chk_scn2(); scn2_1;
    scn3_0; kbd_chk_scn3(); scn3_1;

    T0TCR = 0x01;
    key = *p;
}

void Init_VIC_Interrupt(void) {
    VICProtection = 0;
    VICIntEnClr = 0xFFFFFFFF;
    VICVectAddr = 0;
    VICIntSelect &= ~(1 << 4);
    VICVectAddr4 = (unsigned int)&kbd_scan;
    VICVectPriority4 = 0;
    VICIntEnable |= (1 << 4);
}

void transmit_byte(unsigned char bt) {
    while ((U0LSR & (1 << 6)) == 0);
    U0THR = bt;
}
