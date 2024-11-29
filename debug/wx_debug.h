#ifndef WX_DEBUG_H
#define WX_DEBUG_H
#include "wx_include.h"
#include "wx_failcode.h"
#include "wx_log.h"
typedef struct {
	CHAR *showItem;
    CHAR *showDesc;
	WxDegShowFunc showFunc;
}WxDbgShowHandle;


VOID WX_DebugShow(char *argv[], UINT32 argc);
#endif
