#include "wx_frame.h"
#include "wx_msg_res_pool.h"
#include "wx_tasks.h"
#include "wx_modules.h"
#include "wx_frame.h"
UINT8 g_curCoreId = WX_CORE_ID_0; /* 褰撳墠杩愯鐨勬牳ID */
XScuGic g_wxScuGicInst = {0}; /* Instance of the Interrupt Controller */

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

UINT32 WX_SetupFrame(UINT8 coreId)
{
    /* 创建消息资源池 */
    UINT32 ret = WX_CreateMsgResPool();
    if (ret != WX_SUCCESS) {
        return ret;
    } 
    ret = WX_DeployTasks(coreId);
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
