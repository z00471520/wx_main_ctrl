
#include "wx_task.h"
#include "wx_include.h"
#include "wx_task_deploy.h"
#include "wx_msg_intf.h"
#include "wx_evt_msg_res_pool.h"
#include "wx_can_slave_a.h"
#include "wx_can_slave_b.h"
UINT8 g_curCoreId = WX_CORE_ID_0; /* 当前运行的核ID */


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


