
#include "wx_task.h"
#include "wx_include.h"
#include "wx_task_deploy.h"
#include "wx_msg_common.h"
#include "wx_evt_msg_res_pool.h"
#include "wx_can_slave_a.h"
#include "wx_can_slave_b.h"
UINT8 g_curCoreId = WX_CORE_ID_0; /* 当前运行的核ID */
#define INTC_DEVICE_ID		XPAR_SCUGIC_SINGLE_DEVICE_ID
XScuGic g_wxScuGicInst = {0}; /* Instance of the Interrupt Controller */
/* 获取中断控制器实例 */
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

/* 部署指定核的任务 */
UINT32 WX_Deploy(UINT8 coreId)
{
    /* 部署任务 */
    UINT32 ret = WX_DeployTasks(coreId) {
        return ret;
    }

    /* 部署模块 */
    ret = WX_DeployModules(coreId);
    if (ret != WX_SUCCESS) {
        return ret;
    }

    return WX_SUCCESS;
}


