
#include "wx_include.h"
#include "wx_msg_intf.h"
#include "wx_can_driver_common.h"

/* CAN 设备配置 */
WxCanDriverCfg g_wxCanSlaveBCfg = {

};

/* CAN 设备中断配置 */
WxCanDriverIntrCfg g_wxCanSlaveBIntcCfg = {

};

/* 模块构建函数 */
UINT32 WX_CAN_DRIVER_B_Construct(VOID *module)
{
    UINT32 ret;
    WxCanDriver *this = WX_Mem_Alloc(WX_GetModuleName(module), 1, sizeof(WxCanDriver));
    if (this == NULL) {
        return WX_ERR;
    }
    WX_CLEAR_OBJ(this);
    this->moduleId = WX_GetModuleCoreId(module);
    ret = WX_CAN_DRIVER_Constuct(this, &g_wxCanSlaveBCfg, &g_wxCanSlaveBIntcCfg);
    if (ret != WX_SUCCESS) {
        WX_Mem_Free(this);
        return ret;
    }
    /* 设置上Module */
    WX_SetModuleInfo(module, this);
    return WX_SUCCESS;
}

UINT32 WX_CAN_DRIVER_B_Destruct(VOID *module)
{
    return WX_CAN_DRIVER_Destruct(module);
}

UINT32 WX_CAN_DRIVER_B_Entry(VOID *module, WxMsgType *evtMsg)
{
    WxCanDriver *this = WX_GetModuleInfo(module);
    if (this == NULL) {
        return WX_ERR;
    }
    UINT32 ret = WX_CAN_DRIVER_Entry(this, evtMsg);
    if (ret != WX_SUCCESS) {
        return ret;
    }
    return WX_SUCCESS;
}