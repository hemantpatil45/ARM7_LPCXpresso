#ifndef __TYPE_H__
#define __TYPE_H__

#include <stdint.h>

#ifndef NULL
#define NULL    ((void *)0)
#endif

#ifndef FALSE
#define FALSE   (0)
#endif

#ifndef TRUE
#define TRUE    (1)
#endif

/* Standard NXP Types */
typedef uint32_t  DWORD;
typedef uint16_t  WORD;
typedef uint8_t   BYTE;
typedef uint32_t  BOOL;

/* NXP USB Stack Specific Types */
typedef uint32_t  USB_INT32U;
typedef int32_t   USB_INT32S;
typedef uint16_t  USB_INT16U;
typedef int16_t   USB_INT16S;
typedef uint8_t   USB_INT08U;
typedef int8_t    USB_INT08S;

#endif  /* __TYPE_H__ */
