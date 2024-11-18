#include "wx_include.h"
#include "wx_rs422_i_salve_task.h"
#include "wx_can_slave_task.h"
WxCanSlaveTask g_canSlaveTask[WX_CAN_TYPE_BUTT] = {0};
WxCanSlaveCfgInfo g_wxCanSlaveCfg[WX_CAN_TYPE_BUTT] = {

    [WX_CAN_TYPE_A] = {
        .deviceCfgInfo.isEnable = FALSE, /* 是否使能 */
        .deviceCfgInfo.baudPrescalar = 0,
        .intrCfgInfo.intrId = 0,
        .intrCfgInfo.callBackRef = 0,
    },
    [WX_CAN_TYPE_B] = {
        .deviceCfgInfo.isEnable = FALSE,
        .deviceCfgInfo.baudPrescalar = 0,
        .intrCfgInfo.intrId = 0,
        .intrCfgInfo.callBackRef = 0,
    },
};


/* 创建RS422I主机任务 */
UINT32 WX_CAN_SALVE_CreateTask(VOID)
{
    UINT32 rc;
    WxCanSlaveCfgInfo *cfg = &g_wxCanSlaveCfg;
    WxCanSlaveTask *this = &g_canSlaveTask;
    /* create the msg que to buff the msg to tx */
    if (this->msgQueHandle == 0) {
        this->msgQueHandle = xQueueCreate(WX_RS422I_MASTER_MSG_ITERM_MAX_NUM, sizeof(WxRs422IMasterMsg));
        if (this->msgQueHandle == 0) {
            wx_log(WX_CRITICAL, "Error Exit: xQueueCreate fail");
            return WX_RS422I_MASTER_CREATE_MSG_QUE_FAIL;
        }
    }

    /* the inst or rs422 used for uart data tx/rx */
    rc = WX_InitUartNs550(&this->rs422Inst, cfg->rs422DevId, cfg->rs422Format);
    if (rc != WX_SUCCESS) {
        return rc;
    }

    /* 设置中断 */
    rc = WX_SetupUartNs550Interrupt(&this->rs422Inst, &cfg->rs422IntrCfg);
    if (rc != WX_SUCCESS) {
        return rc;
    }

    /* 创建任务 */
    
    wx_log(WX_DBG, "Create RS422 task success!");
    return WX_SUCCESS;
}