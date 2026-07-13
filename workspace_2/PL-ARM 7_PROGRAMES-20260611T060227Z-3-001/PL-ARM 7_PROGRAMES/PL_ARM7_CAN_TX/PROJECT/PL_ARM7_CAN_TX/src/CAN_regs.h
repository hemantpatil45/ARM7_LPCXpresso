// can_regs.h
#ifndef CAN_REGS_H_
#define CAN_REGS_H_

#include <stdint.h>
#define CAN1_CMR (*(volatile uint32_t *)(CAN1_BASE + 0x08))
#define CAN1_STAT (*(volatile uint32_t *)(CAN1_BASE + 0x0C))
#define CAN1_RID (*(volatile uint32_t *)(CAN1_BASE + 0x30))
#define CAN1_RID_MASK 0x7FF

#define CAN1_BASE       0xE0044000UL
#define CAN1_MOD        (*(volatile uint32_t *)(CAN1_BASE + 0x00)) // Mode Register
#define CAN1_CMD        (*(volatile uint32_t *)(CAN1_BASE + 0x04)) // Command Register
//#define CAN1_STAT       (*(volatile uint32_t *)(CAN1_BASE + 0x08)) // Status Register
#define CAN1_INT        (*(volatile uint32_t *)(CAN1_BASE + 0x0C)) // Interrupt Register
#define CAN1_ILE        (*(volatile uint32_t *)(CAN1_BASE + 0x1C)) // Interrupt Line Enable Register

// Interface Registers Offsets
#define CAN1_IF1_CMD    (*(volatile uint32_t *)(CAN1_BASE + 0x80))
#define CAN1_IF1_ARB    (*(volatile uint32_t *)(CAN1_BASE + 0x84))
#define CAN1_IF1_DATA_A (*(volatile uint32_t *)(CAN1_BASE + 0x88))
#define CAN1_IF1_DATA_B (*(volatile uint32_t *)(CAN1_BASE + 0x8C))

// CAN1_MOD bit masks
#define CAN_MOD_RM      (1u << 0)
#define CAN_MOD_LOM     (1u << 1)
#define CAN_MOD_STM     (1u << 2)

// CAN1_CMD bit masks
#define CAN_CMD_TXRQ    (1u << 0)

#endif /* CAN_REGS_H_ */
