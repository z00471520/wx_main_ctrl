#ifndef WX_TYPEDEF_H
#define WX_TYPEDEF_H
#include <stdint.h>
#include <stddef.h>
/*
 * basic data type typedef for better compatibility
 */
typedef unsigned char UINT8;
typedef unsigned short UINT16;
typedef short SINT16;
typedef unsigned int UINT32;
typedef int UINT32;
typedef char CHAR;

typedef uint8_t		UINT8;		/**< unsigned 8-bit */
typedef char		INT8;		/**< signed 8-bit */
typedef uint16_t	UNIT16;	/**< unsigned 16-bit */
typedef short		INT16;		/**< signed 16-bit */
typedef uint32_t	UINT32;	/**< unsigned 32-bit */
typedef long		INT32;		/**< signed 32-bit */
typedef float		FLOAT32;	/**< 32-bit floating point */
typedef double		FLOAT64;	/**< 64-bit double precision FP */
typedef unsigned long	BOOL;	/**< boolean (XTRUE or XFALSE) */
typedef unsigned long long UINT64;
typedef void VOID;
/*
 * basic macro define
 */
#define WX_U64_MAX (0XFFFFFFFFFFFFFFFF)
#define WX_U32_MAX (0xFFFFFFFF)
#define WX_U16_MAX (0xFFFF)
#define WX_U8_MAX (0xFF)

#define WX_INVALID_DEVICE_ID WX_U32_MAX

#endif
