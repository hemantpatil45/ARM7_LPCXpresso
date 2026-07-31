#ifndef __TYPE_H__
#define __TYPE_H__

#include <stdint.h>
#include <stdbool.h>

/* NXP USB Stack Specific Types */
typedef uint8_t  USB_INT08U;
typedef int8_t   USB_INT08S;
typedef uint16_t USB_INT16U;
typedef int16_t  USB_INT16S;
typedef uint32_t USB_INT32U;
typedef int32_t  USB_INT32S;

/* Standard NXP Types
 * Wrapped in #ifndef to prevent redefinition conflicts
 * with the FatFs 'ff.h' header.
 */
#ifndef _BYTE_DEF
#define _BYTE_DEF
typedef uint8_t  BYTE;
#endif

#ifndef _WORD_DEF
#define _WORD_DEF
typedef uint16_t WORD;
#endif

#ifndef _DWORD_DEF
#define _DWORD_DEF
typedef uint32_t DWORD;
#endif

/* Boolean Types */
#ifndef BOOL
typedef uint8_t BOOL;
#endif

#ifndef TRUE
#define TRUE  1
#endif

#ifndef FALSE
#define FALSE 0
#endif

/* Standard NXP Return Codes */
#ifndef OK
#define OK    0
#endif

#ifndef ERROR
#define ERROR -1
#endif

#endif /* __TYPE_H__ */
