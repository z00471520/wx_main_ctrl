#include "wx_debug.h"
#include "wx_cnt_excp.h"
#define WX_CNT_DEF(cntId) [cntId] = #cntId

/* CntId */
CHAR *g_wxCntExcpDesc[WX_EXCP_CNT_BUTT] = {
	WX_CNT_DEF(WX_EXCP_CNT_START),
    WX_CNT_DEF(WX_EXCP_UNEXPECT_MSG_TYPE),
    WX_CNT_DEF(WX_EXCP_UNDEFINE),

};
/* CntValue */
UINT64 g_wxExcpCnt[WX_EXCP_CNT_BUTT] = {0};

VOID WX_ExcpCnt_DbgShow(char *argv[], UINT32 argc)
{
	/*
	Exceptional CNT INFOS
	-------------
	CntId											CntValue
	WX_EXCP_CNT_XXX									10

	*/
	wx_show("Exceptional CNT INFOS");
	wx_show("-------------");
	wx_show("%-64s %s", "CntId", "CntValue");
	for (UINT32 i = 0; i < WX_EXCP_CNT_BUTT; i++) {
		if (g_wxExcpCnt[i] == 0) {
			continue;
		}
		if (g_wxCntExcpDesc[i] == NULL) {
			/*
			* Undefinedi	ivalue
			*/
			wx_show("Undefined%lu %lu", i, g_wxExcpCnt[i]);
		} else {
			wx_show("%-64s %lu", g_wxCntExcpDesc[i], g_wxExcpCnt[i]);
		}
		
	}
	wx_show("--- End");
}
