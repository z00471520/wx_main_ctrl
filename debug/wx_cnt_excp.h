#ifndef WX_CNT_EXCP_H
#define WX_CNT_EXCPM_H
#include "wx_cnt_norm.h"
/* excp cnt from 10000  */
typedef enum {
	WX_EXCP_CNT_START = 10000, // keep this first
	WX_ERR = WX_EXCP_CNT_START,
	/* if more please add here */
	WX_EXCP_UNEXPECT_MSG_TYPE,
	WX_EXCP_CNT_BUTT,
} WxExcpCntDef;
#endif
