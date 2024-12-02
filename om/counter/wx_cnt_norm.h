#ifndef WX_CNT_NORM_H
#define WX_CNT_NORM_H

typedef enum {
	WX_NORM_CNT_START,
	WX_NORM_CNT_RESV_1,
	WX_NORM_CNT_RESV_2,
	WX_NORM_CNT_RESV_3,
	WX_NORM_CNT_RESV_4,
	WX_NORM_CNT_RESV_5,
	WX_NORM_CNT_RESV_6,
	/* if more please add here */
	
	WX_NORM_CNT_BUTT,
} WxNormCntDef;


VOID WX_NormCnt_DbgShow(char *argv[], UINT32 argc);
#endif
