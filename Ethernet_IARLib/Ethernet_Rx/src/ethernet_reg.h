#ifndef ETHERNET_REG_H
#define ETHERNET_REG_H

#include <stdint.h>

/* --- Absolute EMAC Register Definitions --- */
#define EMAC_MAC1             (*((volatile uint32_t *) 0xFFE00000))
#define EMAC_MAC2             (*((volatile uint32_t *) 0xFFE00004))
#define EMAC_IPGT             (*((volatile uint32_t *) 0xFFE00008))
#define EMAC_IPGR             (*((volatile uint32_t *) 0xFFE0000C))
#define EMAC_CLRT             (*((volatile uint32_t *) 0xFFE00010))
#define EMAC_MAXF             (*((volatile uint32_t *) 0xFFE00014))
#define EMAC_SUPP             (*((volatile uint32_t *) 0xFFE00018))
#define EMAC_TEST             (*((volatile uint32_t *) 0xFFE0001C))
#define EMAC_MCFG             (*((volatile uint32_t *) 0xFFE00020))
#define EMAC_MCMD             (*((volatile uint32_t *) 0xFFE00024))
#define EMAC_MADR             (*((volatile uint32_t *) 0xFFE00028))
#define EMAC_MWTD             (*((volatile uint32_t *) 0xFFE0002C))
#define EMAC_MRDD             (*((volatile uint32_t *) 0xFFE00030))
#define EMAC_MIND             (*((volatile uint32_t *) 0xFFE00034))
#define EMAC_SA0              (*((volatile uint32_t *) 0xFFE00040))
#define EMAC_SA1              (*((volatile uint32_t *) 0xFFE00044))
#define EMAC_SA2              (*((volatile uint32_t *) 0xFFE00048))
#define EMAC_COMMAND          (*((volatile uint32_t *) 0xFFE00100))
#define EMAC_RXDESCRIPTOR     (*((volatile uint32_t *) 0xFFE00108))
#define EMAC_RXSTATUS         (*((volatile uint32_t *) 0xFFE0010C))
#define EMAC_RXDESCRIPTORNUM  (*((volatile uint32_t *) 0xFFE00110))
#define EMAC_RXPRODUCEINDEX   (*((volatile uint32_t *) 0xFFE00114))
#define EMAC_RXCONSUMEINDEX   (*((volatile uint32_t *) 0xFFE00118))
#define EMAC_TXDESCRIPTOR     (*((volatile uint32_t *) 0xFFE0011C))
#define EMAC_TXSTATUS         (*((volatile uint32_t *) 0xFFE00120))
#define EMAC_TXDESCRIPTORNUM  (*((volatile uint32_t *) 0xFFE00124))
#define EMAC_TXPRODUCEINDEX   (*((volatile uint32_t *) 0xFFE00128))
#define EMAC_TXCONSUMEINDEX   (*((volatile uint32_t *) 0xFFE0012C))
#define EMAC_RXFILTERCTRL     (*((volatile uint32_t *) 0xFFE00200))
#define EMAC_POWERDOWN        (*((volatile uint32_t *) 0xFFE00FF4))

/* --- Absolute VIC Register Definitions --- */
#define VICADDRESS            (*((volatile uint32_t *) 0xFFFFFF00))
#define VICPROTECTION         (*((volatile uint32_t *) 0xFFFFF020))
#define VICINTENABLE          (*((volatile uint32_t *) 0xFFFFF010))
#define VICINTENCLEAR         (*((volatile uint32_t *) 0xFFFFF014))
#define VICINTSELECT          (*((volatile uint32_t *) 0xFFFFF00C))
#define VICVECTADDR4          (*((volatile uint32_t *) 0xFFFFF110))
#define VICVECTPRIORITY4      (*((volatile uint32_t *) 0xFFFFF210))
#define VIC_TIMER0_bit        (1 << 4)

/* --- Absolute Timer 0 Register Definitions --- */
#define T0TCR                 (*((volatile uint32_t *) 0xE0004004))

#endif /* ETHERNET_REG_H */
