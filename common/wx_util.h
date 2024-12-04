#ifndef __WX_UTIL_H__
#define __WX_UTIL_H__
#include "wx_typedef.h"
#include "stdlib.h"
#include "xil_printf.h"
#define WX_Mem_Alloc(memname, item_num, item_byte_size) malloc((item_num) * (item_byte_size))

#define WX_CLEAR_OBJ(op) 			\
do {								\
	memset((op), 0, sizeof(*(op))); \
} while(0)


#define WX_Mem_Free(p) 	\
do {					\
    if ((p) != NULL) {	\
        free((p)); 		\
        (p) = NULL;		\
    }					\
} while (FALSE)

#define WX_8BYTE_ALIGN_SIZE(size) (((size) + 7 / 8) * 8)

#endif
