#include "wx_frame.h"
#include "wx_msg_res_pool.h"
#include "wx_tasks.h"
#include "wx_modules.h"
#include "wx_frame.h"
#include "wx_msg_schedule.h"


UINT8 g_curCoreId = WX_CORE_ID_0; /* 褰撳墠杩愯鐨勬牳ID */
extern XScuGic xInterruptController;
XScuGic *WX_GetScuGicInstance(VOID)
{
    if (xInterruptController.IsReady == XIL_COMPONENT_IS_READY) {
        return &xInterruptController;
    } else {
        return NULL;
    }
}

UINT32 WX_InitGlobalParams(VOID)
{
    boot_debug("WX_InitGlobalParams");
    /* create the event msg pool  */
    UINT32 ret = WX_CreateMsgResPool();
    if (ret != WX_SUCCESS) {
        boot_debug("Error Exit: WX_CreateMsgResPool fail(%u)", ret);
        return ret;
    }
    /* create the module router list */
    ret = WX_CreateModuleRouterList();
    if (ret != WX_SUCCESS) {
        boot_debug("Error Exit: WX_CreateModuleRouterList fail(%u)", ret);
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
