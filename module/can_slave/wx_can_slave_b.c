#include "wx_include.h"
#include "wx_can_slave_common.h"
#include "wx_can_slave_b.h"
WxCanSlaveCfgInfo g_wxCanSlaveBCfg = {
    /* 自定义配置 */
    .selfDefCfg.canFrameDataLen = 8,
    /* 设备配置 */
    .deviceCfgInfo.isEnable = FALSE, /* 是否使能 */
    .deviceCfgInfo.baudPrescalar = 0,
    /* 中断配置 */
    .intrCfgInfo.intrId = 0,
    .intrCfgInfo.callBackRef = 0,
};

/* 模块构建函数 */
UINT32 WX_CAN_SLAVE_B_Construct(VOID *module)
{
    UINT32 ret;
    WxCanSlaveModule *this = WX_Mem_Alloc(WX_GetModuleName(module), 1, sizeof(WxCanSlaveModule));
    if (this == NULL) {
        return WX_ERR;
    }
    ret = WX_CAN_SLAVE_Constuct(this, &g_wxCanSlaveBCfg);
    if (ret != WX_SUCCESS) {
        WX_Mem_Free(this);
        return ret;
    }
    /* 设置上Module */
    WX_SetModuleInfo(module, this);
    return WX_SUCCESS;
}

/* 模块析构函数 */
UINT32 WX_CAN_SLAVE_B_Destruct(VOID *module)
{
    WxCanSlaveModule *this = WX_GetModuleInfo(module);
    if (this == NULL) {
        return WX_SUCCESS;
    }
    WX_Mem_Free(this);
    WX_SetModuleInfo(module, NULL);
    return WX_SUCCESS;
}

/* 模块消息处理入口 */
UINT32 WX_CAN_SLAVE_B_Entry(VOID *module, WxEvtMsg *evtMsg)
{
    WxCanSlaveModule *this = WX_GetModuleInfo(module);
    switch (evtMsg->msgHead.msgType)
    {
        case WX_MSG_TYPE_CAN_FRAME: {
            return WX_CAN_SLAVE_ProcCanFrameMsg(this, evtMsg);
        }
        /* if more please add here */
        default: {
            return WX_ERR;
        }
    }
}