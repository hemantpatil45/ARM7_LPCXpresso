/*
 * GENERATED FILE - DO NOT EDIT
 * (c) Code Red Technologies Ltd, 2008-2013
 * (c) NXP Semiconductors 2013-2026
 * Generated linker script file for LPC2478
 * Created from memory.ldt by FMCreateLinkMemory
 * Using Freemarker v2.3.23
 * LPCXpresso v8.2.2 [Build 650] [2016-09-09]  on Jul 20, 2026 3:18:02 PM
 */

MEMORY
{
  /* Define each memory region */
  MFlash512 (rx) : ORIGIN = 0x0, LENGTH = 0x80000 /* 512K bytes (alias Flash) */  
  Ram64 (rwx) : ORIGIN = 0x40000000, LENGTH = 0x10000 /* 64K bytes (alias RAM) */  
  RAM_ETH_16 (rwx) : ORIGIN = 0x7fe00000, LENGTH = 0x4000 /* 16K bytes (alias RAM2) */  
  RAM_USB_16 (rwx) : ORIGIN = 0x7fd00000, LENGTH = 0x4000 /* 16K bytes (alias RAM3) */  
  RAM_RTC_2 (rwx) : ORIGIN = 0xe0084000, LENGTH = 0x800 /* 2K bytes (alias RAM4) */  
}

  /* Define a symbol for the top of each memory region */
  __base_MFlash512 = 0x0  ; /* MFlash512 */  
  __base_Flash = 0x0 ; /* Flash */  
  __top_MFlash512 = 0x0 + 0x80000 ; /* 512K bytes */  
  __top_Flash = 0x0 + 0x80000 ; /* 512K bytes */  
  __base_Ram64 = 0x40000000  ; /* Ram64 */  
  __base_RAM = 0x40000000 ; /* RAM */  
  __top_Ram64 = 0x40000000 + 0x10000 ; /* 64K bytes */  
  __top_RAM = 0x40000000 + 0x10000 ; /* 64K bytes */  
  __base_RAM_ETH_16 = 0x7fe00000  ; /* RAM_ETH_16 */  
  __base_RAM2 = 0x7fe00000 ; /* RAM2 */  
  __top_RAM_ETH_16 = 0x7fe00000 + 0x4000 ; /* 16K bytes */  
  __top_RAM2 = 0x7fe00000 + 0x4000 ; /* 16K bytes */  
  __base_RAM_USB_16 = 0x7fd00000  ; /* RAM_USB_16 */  
  __base_RAM3 = 0x7fd00000 ; /* RAM3 */  
  __top_RAM_USB_16 = 0x7fd00000 + 0x4000 ; /* 16K bytes */  
  __top_RAM3 = 0x7fd00000 + 0x4000 ; /* 16K bytes */  
  __base_RAM_RTC_2 = 0xe0084000  ; /* RAM_RTC_2 */  
  __base_RAM4 = 0xe0084000 ; /* RAM4 */  
  __top_RAM_RTC_2 = 0xe0084000 + 0x800 ; /* 2K bytes */  
  __top_RAM4 = 0xe0084000 + 0x800 ; /* 2K bytes */  
