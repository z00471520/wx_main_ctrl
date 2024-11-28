#ifndef __WX_DEPLOY_H__
#define __WX_DEPLOY_H__

UINT8 g_curCoreId;
#define WX_GetCurCoreId() g_curCoreId
UINT32 WX_Deploy(UINT8 coreId);
#endif