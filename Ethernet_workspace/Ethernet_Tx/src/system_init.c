#include "system_init.h"
#include "LPC24xx.h"
#include "stdint.h"
#include <stdint.h>

#define CCLK_MHZ  72u

void delay_us(uint32_t us)
{
    volatile uint32_t n = us * CCLK_MHZ;
    while(n--) __asm__("nop");
}

void delay_cycles(uint32_t cycles){ while(cycles--) __asm__("nop"); }

void PLL_Init(void)
{
    MEMMAP = 0x01;   		// Interrupts in User Flash.
    SCS |= 0x21;

    delay_cycles(100);

    CLKSRCSEL = 0x01;
    PLLCFG |= 0x001000C0;

    PLLFEED = 0xAA;
    PLLFEED = 0x55;

    PLLCON |= 0x01;
    PLLFEED = 0xAA;
    PLLFEED = 0x55;
    CCLKCFG = 0x03;
    delay_cycles(100);

    PLLCON |= 0x02;
    PLLFEED = 0xAA;
    PLLFEED = 0x55;
    PCLKSEL0 = 0;              // Peripheral clocks = cclk/4.
    PCLKSEL1 = 0;
}

void system_Init(void) {
    SCS |= (1 << 0);
//*********************** SET PINSEL FOR UART0***********************
    PINSEL0 &= (1UL << 4);//M_USB_TXD  P0[2] SET TO 01
    PINSEL0 &= (1UL << 6);//M_USB_RXD  P0[3] SET TO 01

//*********************** SET PINSEL TO CAN DEVICE***********************
    PINSEL0 &= (1UL << 0);//M_RD1(Can controller ) P0[0]
    PINSEL0 &= (1UL << 3);//M_TD1(Can controller ) P0[1]

//***********************SET DIRRECTION TO GLCD***********************
    PINSEL0 &= ~(3UL << 8);//M_TFT_R0/LCD_RS P0[4] SET TO 01
    PINSEL0 &= ~(3UL << 10);//M_TFT_R1/LCD_RW P0[5] SET TO 01
    PINSEL0 &= ~(3UL << 12);//M_TFT_G0/LCD_E P0[6] SET TO 01
    PINSEL0 &= ~(3UL << 14);//M_TFT_G1/LCD_DB0 P0[7] SET TO 01
    PINSEL0 &= ~(3UL << 16);//M_TFT_B0/LCD_DB1 P0[8] SET TO 01
    PINSEL0 &= ~(3UL << 18);//M_TFT_B1/LCD_DB2 P0[9] SET TO 01
    PINSEL3 &= ~(3UL << 8);//M_TFT_G2/LCD_DB3 P1[20] SET TO 01
    PINSEL3 &= ~(3UL << 10);//M_TFT_G3/LCD_DB4 P1[21] SET TO 01
    PINSEL3 &= ~(3UL << 14);//M_TFT_G5/LCD_DB5 P1[23] SET TO 01
    PINSEL3 &= ~(3UL << 16);//M_TFT_G6/LCD_DB6 P1[24] SET TO 01
    PINSEL3 &= ~(3UL << 18);//M_TFT_G7/LCD_DB7[25] SET TO 01
    PINSEL3 &= ~(3UL << 20);//M_TFT_B4/LCD_CS1 P1[26] SET TO 01
    PINSEL0 &= ~(3UL << 24);//M_TFT_B6/LCD_CS2 P1[28] SET TO 01
    PINSEL0 &= ~(3UL << 26);//M_TFT_B6/LCD_RST P1[28] SET TO 01

//***********************SET PINSEL RELAY_ BUZZER***********************
    PINSEL0 &= ~(3UL <<20);  // RELAY_BUZZER OUTPUT P0[10]

//***********************PINSEL FOR KEYBOARD***********************
    PINSEL4 &= ~(3UL << 0);  // KEYBOARD0 P2[0]
    PINSEL4 &= ~(3UL << 2);  // KEYBOARD1 P2[1]
    PINSEL5 &= ~(3UL << 12); // KEYBOARD2 P2[22]
    PINSEL5 &= ~(3UL << 14); // KEYBOARD3 P2[23]
    PINSEL5 &= ~(3UL << 18); // KEYBOARD4 P2[25]
    PINSEL5 &= ~(3UL << 20);  // KEYBOARD5 P2[26]
    PINSEL5 &= ~(3UL << 22); // kEYBOARD6 P2[27]
    PINSEL0 &= ~(3UL << 22); // KEYBOARD7 P0[11]

//***********************SET PINSEL FOR SPI PINS ALSO SD CARD***********************
    PINSEL1 &= ~(3UL << 6);//SD_CLOCK P0[19] SET TO 11
    PINSEL1 &= ~(3UL << 8);//M_SPI_SSEL P0[20]  SET TO 11
    PINSEL1 &= ~(3UL << 12);//M_SPI_MISO P0[22] SET TO 11
    PINSEL4 &= ~(3UL << 22);//M_SPI_MOSI P2[11] SET TO 11
    PINSEL2 &= ~(3UL << 22);//M_SPI_MOSI P1[11] SET TO 11
    PINSEL2 &= ~(3UL << 24);//SD_CS P1[12] SET TO 11

//***********************SET PINSEL INTERNAL ADC PIN***********************
    PINSEL1 &=  (1UL << 14);//M_ADC_CH0 SET TO 01

//***********************SET INTERNAL DAC PIN***********************
    PINSEL1 &=  (2UL << 20); //M_DAC_OUT P0[26] SET TO 10

//***********************SET PINSEL FOR EXTERNAL ADC PINS***********************
    PINSEL7 &= ~(3UL << 0);//D16/M_ADC_DB0 P3[16] SET TO 01
    PINSEL7 &= ~(3UL << 2);//D16/M_ADC_DB1 P3[17] SET TO 01
    PINSEL7 &= ~(3UL << 4);//D16/M_ADC_DB2 P3[18] SET TO 01
    PINSEL7 &= ~(3UL << 6);//D16/M_ADC_DB3 P3[19] SET TO 01
    PINSEL7 &= ~(3UL << 8);//D16/M_ADC_DB4 P3[20] SET TO 01
    PINSEL7 &= ~(3UL << 10);//D16/M_ADC_DB5 P3[21] SET TO 01
    PINSEL7 &= ~(3UL << 12);//D16/M_ADC_DB6 P3[22] SET TO 01
    PINSEL7 &= ~(3UL << 14);//D16/M_ADC_DB7 P3[23] SET TO 01
    PINSEL7 &= ~(3UL << 16);//D16/M_ADC_DB8 P3[24] SET TO 01
    PINSEL7 &= ~(3UL << 18);//D16/M_ADC_DB9 P3[25] SET TO 01
    PINSEL7 &= ~(3UL << 20);//D16/M_ADC_DB10 P3[26] SET TO 01
    PINSEL7 &= ~(3UL << 22);//D16/M_ADC_DB11 P3[27] SET TO 01
    PINSEL7 &= ~(3UL << 24);//D28/M_ADC_CONVST# P3[28] SET TO 01
    PINSEL7 &= ~(3UL << 26);//D29/M_ADC_CS# P3[29] SET TO 01//DEFAULT OUT SET
    PINSEL7 &= ~(3UL << 28);//D30/M_ADC_RD# P3[30] SET TO 01//DEFAULT OUT SET

// ***********************SET PISEL FOR DAC PINS***********************
    PINSEL4 &= ~(3UL << 28);//M_DAC_CS# SET TO 01
    PINSEL1 &= ~(3UL << 18);//M_DAC_CLR# P0[25] SET TO 01
    PINSEL2 &= ~(3UL << 14);//M_DAC_LD# P1[7] SET TO 01

//***********************SET PINSEL FOR SPI PINS***********************
    PINSEL0 &=  (2UL << 30);//M_SPI_SCK P0[15] SET TO 10
    PINSEL1 &=  (2UL << 0);//M_SPI_SSEL P0[16]  SET TO 10
    PINSEL1 &=  (2UL << 2);//M_SPI_MISO P0[17] SET TO 10
    PINSEL1 &=  (2UL << 4);//M_SPI_MOSI P0[18] SET TO 10

//***********************SET PINSEL FOR I2C***********************
    PINSEL1 &= (1UL << 22);//M_I2C_SDA P0[27] set to(01)
    PINSEL1 &= (1UL << 24);//M_I2C_SCL P0[28] set to(01)

//***********************SET PINSEL FOR MOTOR***********************
    PINSEL1 &= ~(3UL << 10);// M_MOTOR_EN_A  P0[21]
    PINSEL1 &= ~(3UL << 16);// M_MOTOR_EN_B  P0[24]
    PINSEL2 &= ~(3UL << 4);//M_A_PHASE/M_PWM0[1] P1[2] SET TO 11
    PINSEL2 &= ~(3UL << 6);//M_B_PHASE/M_PWM0[2] P1[3]SET TO 11
    PINSEL2 &= ~(3UL << 10);//M_C_PHASE/M_PWM0[3] P1[15]  SET TO 11
    PINSEL2 &= ~(3UL << 12);//M_D_PHASE/M_PWM0[4] P1[16] SET TO 11

//*********************** Set PINSEL LED 0 to 3***********************
    PINSEL4 &= ~(3UL << 30);  // P2.15
    PINSEL5 &= ~(3UL << 6);   // P2.19
    PINSEL5 &= ~(3UL << 10);  // P2.21
    PINSEL2 &= ~(3UL << 26);  // P1.13

//***********************PINSEL for Switch 0 to 3***********************
    PINSEL9 &= ~(3UL << 14);  // Switch 0 P4[23]
    PINSEL9 &= ~(3UL << 20);  // Switch 1 P4[26]
    PINSEL9 &= ~(3UL << 22);  // Switch 2 P4[27]
    PINSEL9 &= ~(3UL << 30);  // Switch 3 P4[31]

//***********************SET PINSEL EXTERNAL INTERRUPT***********************
    PINSEL4 &= (1UL << 20);//M_ISP/M_EINT0# P2[10] SET TO 01

//***********************ETHERNET SECTION***************************************
    PINSEL2 &= (1UL << 0); //ENET_TXD0 P1[0] set to 01
    PINSEL2 &= (1UL << 2); //ENET_TXD1 P1[1] set to 01
    PINSEL2 &= (1UL << 8); //ENET_TX_EN P1[4] set to 01
    PINSEL2 &= (1UL << 30); //ENET_RX_CLK P1[15] set to 01
    PINSEL2 &= (1UL << 16); //ENET_CRS P1[8] set to 01
    PINSEL2 &= (1UL << 18); //ENET_RXD0 P1[9] set to 01
    PINSEL2 &= (1UL << 20); //ENET_RXD1 P1[10] set to 01
    PINSEL2 &= (1UL << 28); //ENET_RX_ER P1[14] set to 01
    PINSEL3 &= (1UL << 0); //ENET_MDC P1[16] set to 01
    PINSEL3 &= (1UL << 2); //ENET_MDIO P1[17] set to 01

//**************************PINMODE (ALL PINS ARE PULL UP REGISTER ENABLED)*******
    PINMODE0 = 0x00000000;
    PINMODE1 = 0X00000000;
    /* --- CRITICAL FIX: Disable pull-ups on RMII lines --- */
    PINMODE2 = 0XAAAAAAAA;
    /* ---------------------------------------------------- */
    PINMODE3 = 0X00000000;
    PINMODE4 = 0X00000000;
    PINMODE5 = 0XAA02022A;
    PINMODE6 = 0XAAAAAAAA;
    PINMODE7 = 0XAAAAAAAA;
    PINMODE8 = 0XAAAAAAAA;
    PINMODE9 = 0X200A2AAA;

 //--------SET DIRRECTIONS OF PINSEL --------//
    FIO0DIR = 0X03784FF0;
    FIO1DIR = 0X3FF038EC;
    FIO2DIR = 0X0EE8F3FC;
    FIO3DIR = 0X70000000;
    FIO4DIR = 0X30000000;

    FIO0SET = 0X02004000;
    FIO1SET = 0X00003080;
    FIO2SET = 0X0028C000;
    FIO3SET = 0X70000000;
    FIO4SET = 0X00000000;

    FIO0CLR = 0X01780FF0;
    FIO1CLR = 0X3FF0086C;
    FIO2CLR = 0X0EC03BFC;
    FIO3CLR = 0X00000000;
    FIO4CLR = 0X30000000;

    FIO0MASK = 0;
    FIO1MASK = 0;
    FIO2MASK = 0;
    FIO3MASK = 0;
    FIO4MASK = 0;

    FIO1DIR = (1 << 22);
    FIO0DIR |= (1 << 4);
    FIO0DIR |= (1 << 5);
    FIO0DIR |= (1 << 6);
    FIO0DIR |= (1 << 7);
    FIO0DIR |= (1 << 8);
    FIO0DIR |= (1 << 9);
    FIO1DIR |= (1 << 20);
    FIO1DIR |= (1 << 21);
    FIO1DIR |= (1 << 23);
    FIO1DIR |= (1 << 24);
    FIO1DIR |= (1 << 25);
    FIO1DIR |= (1 << 26);
    FIO0DIR |= (1 << 28);
}
