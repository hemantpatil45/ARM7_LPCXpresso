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
void PLL_Init()
{
    MEMMAP = 0x01;
    SCS |= 0x21;
    delay_cycles(100);

    CLKSRCSEL = 0x01;

    // FIX 1: M=12, N=1 generates a perfect 288 MHz PLL
    PLLCFG = 0x0000000B;

    PLLFEED = 0xAA;
    PLLFEED = 0x55;

    PLLCON |= 0x01;
    PLLFEED = 0xAA;
    PLLFEED = 0x55;

    // CPU Clock is 288 / 6 = 48 MHz
    CCLKCFG = 0x05;
    delay_cycles(100);

    PLLCON |= 0x02;
    PLLFEED = 0xAA;
    PLLFEED = 0x55;

    PCLKSEL0 = 0;
    PCLKSEL1 = 0;
}
// Ensure USB clock divider is set to 5 (288MHz / (5+1) = 48MHz USB Clock)

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
    PINSEL0 &= ~(3UL << 18);//M_TFT_B1/LCD_DB2 P0[9] SET TO 01     PINSEL3 &= ~(3UL << 8);//M_TFT_G2/LCD_DB3 P1[20] SET TO 01
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
    //    PINSEL7 &=  (1UL << 30);//D31/M_ADC_BUSY P3[31] SET TO 01

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


    //**********************SET TO USB2 DEVICE***********************
        PINSEL0 &= (1UL << 24);//M_USB_PPWR2 P0[12] SET TO 01
        PINSEL0 &= (1UL << 26);//M_USB_UP_LED2 P0[13] SET TO 01
        PINSEL0 &= ~(3UL << 28);//VBUS P0[14] SET TO 01
        PINSEL1 &= (1UL << 30);//M_USB_D+2 P0[31] SET TO 01
        PINSEL3 &= (1UL << 28);//M_USB_PWRD2 P1[30] SET TO 01
        PINSEL3 &= (1UL << 30);//M_USB_OVRCR2# P1[31] SET TO 01

 //*********************** SET TO USB1 DEVICE***********************
    PINSEL1 &= (1UL << 26);//M_USB_D+1 P0[29] SET TO 01
    PINSEL1 &= (1UL << 28);//M_USB_D-1 P0[30] SET TO 01
    PINSEL2 &= (1UL << 16);//USB_UP -LED P1[18] SET TO 01
    PINSEL2 &= (1UL << 18);//USB_1 CONNECT P1[19] SET TO 01

//***********************EXTERNAL FLASH CONTROL ***********************
    PINSEL9 &= (1UL << 16);//FLASH_OE P4[24]
    PINSEL9 &= (1UL << 28);//FLASH_CE P4[30]

//***************************EXTERNAL SDRAM CONTROL SIGNALS************************
    PINSEL5 &= (1UL << 8);  //SDRAM CS0 P2[20] set to 01
    PINSEL5 &= (1UL << 0);  //SDRAM CAS P2[16] set to 01
    PINSEL5 &= (1UL << 2);  //SDRAM RAS P2[17] set to 01
    PINSEL5 &= (1UL << 4);  //SDRAM CLOCK P2[18] set to 01
    PINSEL5 &= (1UL << 16);  //SDRAM CLOCK ENABLE P2[24] set to 01
    PINSEL5 &= (1UL << 24);  //SDRAM1 DQML P2[28] set to 01
    PINSEL5 &= (1UL << 26);  //SDRAM1 DQMH P2[29] set to 01
    PINSEL5 &= (1UL << 28);  //SDRAM2 DQML P2[30] set to 01
    PINSEL5 &= (1UL << 30);  //SDRAM2 DQMH P2[31] set to 01


 //****************************EXTERNAL SDRAM & FLASH COMMON CONTROL SIGNALS******
    PINSEL9 &= (1UL << 18);  //SDRAM & FLASH WRENB P4[25] set to 01

 //**********************EXTERNAL MEMORY ADDRESS LINES****************************
    PINSEL8 &= (1UL << 0);  //EXTERNAL MEMORY ADDRESS0 P4[0] set to 01
   	PINSEL8 &= (1UL << 2);  //EXTERNAL MEMORY ADDRESS1 P4[1] set to 01
   	PINSEL8 &= (1UL << 4);  //EXTERNAL MEMORY ADDRESS2 P4[2] set to 01
  	PINSEL8 &= (1UL << 6);  //EXTERNAL MEMORY ADDRESS3 P4[3] set to 01    	PINSEL8 &= (1UL << 8);  //EXTERNAL MEMORY ADDRESS4 P4[4] set to 01
    PINSEL8 &= (1UL << 10);  //EXTERNAL MEMORY ADDRESS5 P4[5] set to 01
   	PINSEL8 &= (1UL << 12);  //EXTERNAL MEMORY ADDRESS6 P4[6] set to 01
   	PINSEL8 &= (1UL << 14);  //EXTERNAL MEMORY ADDRESS7 P4[7] set to 01
   	PINSEL8 &= (1UL << 16);  //EXTERNAL MEMORY ADDRESS8 P4[8] set to 01
   	PINSEL8 &= (1UL << 18);  //EXTERNAL MEMORY ADDRESS9 P4[9] set to 01    	PINSEL8 &= (1UL << 20);  //EXTERNAL MEMORY ADDRESS10 P4[10] set to 01
    PINSEL8 &= (1UL << 22);  //EXTERNAL MEMORY ADDRESS11 P4[11] set to 01
   	PINSEL8 &= (1UL << 24);  //EXTERNAL MEMORY ADDRESS12 P4[12] set to 01
   	PINSEL8 &= (1UL << 26);  //EXTERNAL MEMORY ADDRESS13 P4[13] set to 01
   	PINSEL8 &= (1UL << 28);  //EXTERNAL MEMORY ADDRESS14 P4[14] set to 01
   	PINSEL8 &= (1UL << 30);  //EXTERNAL MEMORY ADDRESS15 P4[15] set to 01    	PINSEL9 &= (1UL << 0);  //EXTERNAL MEMORY ADDRESS16 P4[16] set to 01
    PINSEL9 &= (1UL << 2);  //EXTERNAL MEMORY ADDRESS17 P4[17] set to 01
   	PINSEL9 &= (1UL << 4);  //EXTERNAL MEMORY ADDRESS18 P4[18] set to 01
   	PINSEL9 &= (1UL << 6);  //EXTERNAL MEMORY ADDRESS19 P4[19] set to 01
   	PINSEL9 &= (1UL << 8);  //EXTERNAL MEMORY ADDRESS20 P4[20] set to 01
   	PINSEL9 &= (1UL << 10);  //EXTERNAL MEMORY ADDRESS21 P4[21] set to 01    	PINSEL9 &= (1UL << 12);  //EXTERNAL MEMORY ADDRESS22 P4[22] set to 01

//************************EXTERNAL MEMORY DATA LINES******************************    	PINSEL6 &= (1UL << 0);  //EXTERNAL MEMORY DATA0 P3[0] set to 01
    PINSEL6 &= (1UL << 2);  //EXTERNAL MEMORY DATA1 P3[1] set to 01
   	PINSEL6 &= (1UL << 4);  //EXTERNAL MEMORY DATA2 P3[2] set to 01
   	PINSEL6 &= (1UL << 6);  //EXTERNAL MEMORY DATA3 P3[3] set to 01
   	PINSEL6 &= (1UL << 8);  //EXTERNAL MEMORY DATA4 P3[4] set to 01
   	PINSEL6 &= (1UL << 10);  //EXTERNAL MEMORY DATA5 P3[5] set to 01
    PINSEL6 &= (1UL << 12);  //EXTERNAL MEMORY DATA6 P3[6] set to 01
   	PINSEL6 &= (1UL << 14);  //EXTERNAL MEMORY DATA7 P3[7] set to 01
   	PINSEL6 &= (1UL << 16);  //EXTERNAL MEMORY DATA8 P3[8] set to 01
   	PINSEL6 &= (1UL << 18);  //EXTERNAL MEMORY DATA9 P3[9] set to 01
   	PINSEL6 &= (1UL << 20);  //EXTERNAL MEMORY DATA10 P3[10] set to 01    	PINSEL6 &= (1UL << 22);  //EXTERNAL MEMORY DATA11 P3[11] set to 01
    PINSEL6 &= (1UL << 24);  //EXTERNAL MEMORY DATA12 P3[12] set to 01
   	PINSEL6 &= (1UL << 26);  //EXTERNAL MEMORY DATA13 P3[13] set to 01
   	PINSEL6 &= (1UL << 28);  //EXTERNAL MEMORY DATA14 P3[14] set to 01
   	PINSEL6 &= (1UL << 30);  //EXTERNAL MEMORY DATA15 P3[15] set to 01

//************************REMAINING LINES USE FOR TFT DISPLAY********************
 //Current these pins are not used (TFT not used),so as GPIO.
   	PINSEL3 &= (1UL << 12); //AS a GPIO for P1[22] set to 01
   	PINSEL3 &= (1UL << 22); //AS a GPIO for P1[27] set to 01
   	PINSEL4 &= (3UL << 4); //AS a GPIO for P2[2] set to 11
  	PINSEL4 &= (3UL << 6); //AS a GPIO for P2[3] set to 11
   	PINSEL4 &= (3UL << 8); //AS a GPIO for P2[4] set to 11
   	PINSEL4 &= (3UL << 10); //AS a GPIO for P2[5] set to 11
  	PINSEL4 &= (3UL << 12); //AS a GPIO for P2[6] set to 11
   	PINSEL4 &= (3UL << 14); //AS a GPIO for P2[7] set to 11
   	PINSEL4 &= (3UL << 16); //AS a GPIO for P2[8] set to 11
   	PINSEL4 &= (3UL << 18); //AS a GPIO for P2[9] set to 11
   	PINSEL4 &= (1UL << 24); //AS a GPIO for P2[12] set to 01
   	PINSEL4 &= (1UL << 26); //AS a GPIO for P2[13] set to 01
   	PINSEL9 &= (3UL << 24); //AS a GPIO for P4[28] set to 11
   	PINSEL9 &= (3UL << 26); //AS a GPIO for P4[29] set to 11

//**************************PINMODE (ALL PINS ARE PULL UP REGISTER ENABLED)*******
    PINMODE0 = 0x00000000;
   	PINMODE1 = 0X00000000;
   	PINMODE2 = 0X00000000;
   	PINMODE3 = 0X00000000;
   	PINMODE4 = 0X00000000;
   	PINMODE5 = 0XAA02022A;
   	PINMODE6 = 0XAAAAAAAA;//0XAAAAAAAA;//0X00000000;//0XFFFFFFFF;
   	PINMODE7 = 0XAAAAAAAA;//0XFFFFFFFF;//0X00000000;
   	PINMODE8 = 0XAAAAAAAA;//0XFFFFFFFF;//0X00000000;
   	PINMODE9 = 0X200A2AAA;//0XFFFFFFFF;//0X00000000;

 //--------SET DIRRECTIONS OF PINSEL --------//
 //***********************IO PIN DIRECTION REGISTOR*******************************
   	FIO0DIR = 0X03784FF0; //WRITE VALUE
   	FIO1DIR = 0X3FF038EC; //WRITE VALUE
   	FIO2DIR = 0X0EE8F3FC; //WRITE VALUE
   	FIO3DIR = 0X70000000; //WRITE VALUE
   	FIO4DIR = 0X30000000; //0X30000000; //WRITE VALUE

 //***********************IO PIN SET REGISTOR*************************************
   	FIO0SET = 0X02004000; //WRITE VALUE
   	FIO1SET = 0X00003080; //WRITE VALUE
   	FIO2SET = 0X0028C000; //WRITE VALUE
   	FIO3SET = 0X70000000; //WRITE VALUE
   	FIO4SET = 0X00000000; //WRITE VALUE

 //***********************IO PIN CLEAR REGISTOR**********************************
   	FIO0CLR = 0X01780FF0; //WRITE VALUE
   	FIO1CLR = 0X3FF0086C; //WRITE VALUE
   	FIO2CLR = 0X0EC03BFC; //WRITE VALUE
   	FIO3CLR = 0X00000000; //WRITE VALUE
   	FIO4CLR = 0X30000000; //WRITE VALUE

//**********************IO PIN MASK REGISTOR***********************************
 //NO ANY MASKING IS USED
   	FIO0MASK = 0;
   	FIO1MASK = 0;
   	FIO2MASK = 0;
   	FIO3MASK = 0;
   	FIO4MASK = 0;
//
    FIO1DIR = (1 << 22); //AS a GPIO for P1[22] ALL Direction = NA(OUT)
	FIO0DIR |= (1 << 4);//M_TFT_R0/LCD_RS P0[4]WORK AS OUTPUT
	FIO0DIR |= (1 << 5);//M_TFT_R1/LCD_RW P0[5] WORK AS OUTPUT
	FIO0DIR |= (1 << 6);//M_TFT_G0/LCD_E P0[6] WORK AS OUTPUT
	FIO0DIR |= (1 << 7);//M_TFT_G1/LCD_DB0 P0[7]WORK AS OUTPUT
	FIO0DIR |= (1 << 8);//M_TFT_B0/LCD_DB1 P0[8] WORK AS OUTPUT
	FIO0DIR |= (1 << 9);//M_TFT_B1/LCD_DB2 P0[9] WORK AS OUTPUT
	FIO1DIR |= (1 << 20);//M_TFT_G2/LCD_DB3 P1[20]WORK AS OUTPUT
	FIO1DIR |= (1 << 21);//M_TFT_G3/LCD_DB4 P1[21] WORK AS OUTPUT
	FIO1DIR |= (1 << 23);//M_TFT_G5/LCD_DB5 P1[23] WORK AS OUTPUT
	FIO1DIR |= (1 << 24);//M_TFT_G6/LCD_DB6 P1[24] WORK AS OUTPUT
	FIO1DIR |= (1 << 25);//M_TFT_G7/LCD_DB7[25] WORK AS OUTPUT
	FIO1DIR |= (1 << 26);//M_TFT_B4/LCD_CS1 P1[26] WORK AS OUTPUT
	FIO0DIR |= (1 << 28);//M_TFT_B6/LCD_CS2 P1[28] WORK AS OUTPUT

}
