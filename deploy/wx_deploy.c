
#include "wx_include.h"
#include "wx_deploy_tasks.h"
#include "wx_msg_common.h"
#include "wx_msg_res_pool.h"
#include "wx_can_slave.h"
#include "wx_deploy_modules.h"
UINT8 g_curCoreId = WX_CORE_ID_0; /* 褰撳墠杩愯鐨勬牳ID */
XScuGic g_wxScuGicInst = {0}; /* Instance of the Interrupt Controller */
/* 鑾峰彇涓柇鎺у埗鍣ㄥ疄渚� */
XScuGic *WX_GetOrCreateScuGicInstance(VOID)
{
    if (g_wxScuGicInst.IsReady == XIL_COMPONENT_IS_READY) {
        return &g_wxScuGicInst;
    }

    XScuGic_Config *intcConfig = XScuGic_LookupConfig(INTC_DEVICE_ID);
	if (NULL == intcConfig) {
		return NULL;
	}

	int status = XScuGic_CfgInitialize(&g_wxScuGicInst, intcConfig, intcConfig->CpuBaseAddress);
	if (status != XST_SUCCESS) {
		return NULL;
	}

    return &g_wxScuGicInst;
}

/* deploy current core */
UINT32 WX_Deploy(UINT8 coreId)
{
    UINT32 ret = WX_DeployTasks(coreId);
    if (ret != WX_SUCCESS) {
        return ret;
    }

    /* 部署模块 */
    ret = WX_DeployModules(coreId);
    if (ret != WX_SUCCESS) {
        return ret;
    }

    return WX_SUCCESS;
}


