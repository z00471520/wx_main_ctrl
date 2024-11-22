#ifndef __WX_UTIL_H__
#define __WX_UTIL_H__
#include "wx_include.h"
/* 适用于结构体对象的初始化 */
#define WX_CLEAR_OBJ(op) memset(op, sizeof(*op), 0)
/*
 * memname          内存的名字，字符串
 * item_num         item个数
 * item_byte_size   每个item的尺寸大小
 * 该模块支持内存统计，方便定位内存残留问题，暂时不支持
 **/
#define WX_Mem_Alloc(memname, item_num, item_byte_size) malloc((item_num) * (item_byte_size))
/*
 * 改函数释放后会把指针设置为NULL
 **/
#define WX_Mem_Free(p) /
do {/
    if ((p) != NULL) {/
        free((p)); /
        (p) = NULL;/
    }/
} while(FALSE)

#define WX_8BYTE_ALIGN_SIZE(size) (((size) + 7 / 8) * 8)
#endif