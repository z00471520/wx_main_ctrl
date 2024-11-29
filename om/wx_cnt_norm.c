
#include "wx_debug.h"
#define WX_CNT_DEF(cntId) #cntId

/* CntId */
CHAR *g_wxCntNormDesc[WX_NORM_CNT_BUTT] = {
	WX_CNT_DEF(WX_NORM_CNT_START),
	WX_CNT_DEF(WX_NORM_CNT_1),
	WX_CNT_DEF(WX_NORM_CNT_2),
	WX_CNT_DEF(WX_NORM_CNT_BUTT)
};

/* CntValue */
UINT64 g_wxNormCnt[WX_NORM_CNT_BUTT] = {1,2,3,4,5};

VOID WX_NormCnt_DbgShow(VOID)
{
	/*
	Normal CNT INFOS
	-------------
	CntId											CntValue
	WX_NORM_CNT_XXX									10

	*/
	wx_show("Normal CNT INFOS\n");
	wx_show("----------------\n");
	wx_show("%-32s %s\n", "(CntId)", "(CntValue)");
	for (UINT32 i = 0; i < WX_NORM_CNT_BUTT; i++) {
		if (g_wxNormCnt[i] == 0) {
			continue;
		}
		if (g_wxCntNormDesc[i] == NULL) {
			/*
			* Undefinedi	ivalue
			*/
			wx_show("Undefined %-22lu %lu\n", i, g_wxNormCnt[i]);
		} else {
			wx_show("%-32s %lu\n", g_wxCntNormDesc[i], g_wxNormCnt[i]);
		}
		
	}
}
