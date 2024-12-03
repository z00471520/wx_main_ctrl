#include "wx_frame.h"
#include "wx_msg_res_pool.h"
#include "wx_tasks.h"
#include "wx_modules.h"
#include "wx_frame.h"
#include "wx_msg_schedule.h"

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
    Xil_ExceptionInit();
    /*
	 * Connect the interrupt controller interrupt handler to the hardware
	 * interrupt handling logic in the processor.
	 */
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_IRQ_INT,
				(Xil_ExceptionHandler)XScuGic_InterruptHandler,
				&g_wxScuGicInst);
    return &g_wxScuGicInst;
}

UINT32 WX_InitGlobalParams(VOID)
{
    boot_debug("WX_InitGlobalParams");
    /* create the event msg pool  */
    UINT32 ret = WX_CreateMsgResPool();
    if (ret != WX_SUCCESS) {
        return ret;
    }
    /* create the module router list */
    ret = WX_CreateModuleRouterList();
    if (ret != WX_SUCCESS) {
        return ret;
    }
    boot_debug("WX_InitGlobalParams end");
    return WX_SUCCESS;

}

UINT32 WX_SetupFrame(UINT8 coreId)
{
	UINT32 ret;
	/* init global params */
	ret = WX_InitGlobalParams();
	if (ret != WX_SUCCESS) {
		boot_debug("Error Exit: WX_InitGlobalParams fail(%u)", ret);
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
