#ifndef __WX_TASK_DEPLOY_H__
#define __WX_TASK_DEPLOY_H__



VOID WX_Deply_TaskFuncCode(VOID *param);
UINT32 WX_DeployTasks(WxCoreId coreId);

UINT8 g_curCoreId;
#define WX_GetCurCoreId() g_curCoreId
#endif