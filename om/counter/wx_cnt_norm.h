#ifndef __WX_CNT_NORM_H__
#define __WX_CNT_NORM_H__

typedef enum {
	WX_NORM_CNT_START,
	WX_NORM_CNT_RESV_1,
	WX_NORM_CNT_RESV_2,
	WX_NORM_CNT_RESV_3,
	WX_NORM_CNT_RESV_4,
	WX_NORM_CNT_RESV_5,
	WX_NORM_CNT_RESV_6,
	WX_NORM_RS422_MASTER_DRIVER_RECV_TX_ADU_REQ,
	WX_NORM_RS422_MASTER_DRIVER_TX_ADU_BUFFED,
	/* if more please add here */
	
	WX_NORM_CNT_BUTT,
} WxNormCntDef;
UINT64 g_wxNormCnt[WX_NORM_CNT_BUTT];
#define wx_norm_cnt(v) g_wxNormCnt[v]++

VOID WX_NormCnt_DbgShow(char *argv[], UINT32 argc);
#endif
