#ifndef __WX_UTIL_H__
#define __WX_UTIL_H__
#include "wx_include.h"
/* 閫傜敤浜庣粨鏋勪綋瀵硅薄鐨勫垵濮嬪寲 */
#define WX_CLEAR_OBJ(op) memset(op, sizeof(*(op)), (UINT8)0)
/*
 * memname          鍐呭瓨鐨勫悕瀛楋紝瀛楃涓�
 * item_num         item涓暟
 * item_byte_size   姣忎釜item鐨勫昂瀵稿ぇ灏�
 * 璇ユā鍧楁敮鎸佸唴瀛樼粺璁★紝鏂逛究瀹氫綅鍐呭瓨娈嬬暀闂锛屾殏鏃朵笉鏀寔
 **/
#define WX_Mem_Alloc(memname, item_num, item_byte_size) malloc((item_num) * (item_byte_size))
/*
 * 鏀瑰嚱鏁伴噴鏀惧悗浼氭妸鎸囬拡璁剧疆涓篘ULL
 **/
#define WX_Mem_Free(p) 	\
do {					\
    if ((p) != NULL) {	\
        free((p)); 		\
        (p) = NULL;		\
    }					\
} while (FALSE)

#define WX_8BYTE_ALIGN_SIZE(size) (((size) + 7 / 8) * 8)
#endif
