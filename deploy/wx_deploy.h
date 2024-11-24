#ifndef __WX_TASK_DEPLOY_H__
#define __WX_TASK_DEPLOY_H__

#define WX_GetModuleName(m)         (((WxModule *)m)->moduleName)
#define WX_GetModuleInfo(m)         (((WxModule *)m)->moduleInfo)
#define WX_SetModuleInfo(m, v)      (((WxModule *)m)->moduleInfo = (v))
#define WX_GetModuleCoreId(m)       (((WxModule *)m)->coreId)

VOID WX_Deply_TaskFuncCode(VOID *param);
UINT32 WX_DeployTasks(WxCoreId coreId);

UINT8 g_curCoreId;
#define WX_GetCurCoreId() g_curCoreId
#endif