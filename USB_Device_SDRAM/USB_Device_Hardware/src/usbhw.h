#ifndef __USBHW_H
#define __USBHW_H

#include <stdint.h>
#include <stdbool.h>

#define CMD_SET_ADDR        0xD0
#define CMD_CFG_DEV         0xD8
#define CMD_SET_DEV_STAT    0xFE
#define CMD_GET_DEV_STAT    0xFE
#define CMD_CLR_BUF         0xF2
#define CMD_VALID_BUF       0xFA
#define CMD_SEL_EP          0x00
#define CMD_SET_EP_STAT     0x40

extern void USB_WriteCmd(uint32_t cmd);
extern void USB_WriteCmdData(uint32_t cmd, uint32_t data);

extern void USB_Init(void);
extern void USB_Connect(bool connect);

extern void USB_SetAddress(uint32_t adr);
extern void USB_Configure(bool configure);
extern void USB_ConfigEP(uint32_t EPNum, uint32_t MaxPacketSize);
extern void USB_EnableEP(uint32_t EPNum);
extern void USB_DisableEP(uint32_t EPNum);
extern void USB_SetStallEP(uint32_t EPNum);
extern void USB_ClrStallEP(uint32_t EPNum);

extern uint32_t USB_ReadEP(uint32_t EPNum, uint8_t *pData);
extern uint32_t USB_WriteEP(uint32_t EPNum, uint8_t *pData, uint32_t cnt);

extern void USB_ISR(void) __attribute__((interrupt("IRQ")));

#endif
