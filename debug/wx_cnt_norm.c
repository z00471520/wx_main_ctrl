
#include "wx_debug.h"
#define WX_CNT_DEF(cntId) [cntId]={#cntId}
CHAR * g_wxCntNormDesp[] = {
	WX_CNT_DEF(WX_NORM_CNT_XXX),
};
UINT63 g_wxNormCnt[WX_NORM_CNT_BUTT] = {0};


VOID WX_CNT_DbgShow(char *argv[], UINT32 argc)
{
	/*
	Normal CNT INFOS
	-------------
	CntId											CntValue
	WX_NORM_CNT_XXX									10

	*/
	wx_show("Normal CNT INFOS");
	wx_show("-------------");
	wx_show("%64s %s", "CntId", "CntValue");
	for (i = 0; i < ; i++) {
		if (g_wxNormCnt[i] == 0) {
			continue;
		}
		if (g_wxCntNormDesc[i] == NULL) {
			wx_show("Undefined%lu %lu", i, g_wxNormCnt[i]);
		} else {
			wx_show("%64s %s", g_wxCntNormDesc[i], g_wxNormCnt[i]);
		}
		
	}
}
