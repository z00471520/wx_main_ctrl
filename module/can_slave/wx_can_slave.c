#include "wx_include.h"
#include "wx_can_slave_common.h"
#include "wx_can_slave.h"
#include "wx_id_def.h"
WxCanSlaveCfg g_wxCanSlaveCfg[] = {
    {
        /* 自定义配置 */
        .canFrameDataLen = 8,
        .moduleId = WX_MODULE_CAN_SLAVE_A,
        /* 设备配置 */
        .deviceCfgInfo.isEnable = FALSE, /* 是否使能 */
        .deviceCfgInfo.baudPrescalar = 0,
        /* 中断配置 */
        .intrCfgInfo.intrId = 0,
        .intrCfgInfo.callBackRef = 0,
    }, 
    {
         /* 自定义配置 */
        .canFrameDataLen = 8,
        .moduleId = WX_MODULE_CAN_SLAVE_B,
        /* 设备配置 */
        .deviceCfgInfo.isEnable = FALSE, /* 是否使能 */
        .deviceCfgInfo.baudPrescalar = 0,
        /* 中断配置 */
        .intrCfgInfo.intrId = 0,
        .intrCfgInfo.callBackRef = 0,
    }
};

WxCanSlaveCfg *WX_CanSlave_GetCfg(UINT32 moduleId)
{
    UINT32 i;
    for (i = 0; i < sizeof(g_wxCanSlaveCfg) / sizeof(WxCanSlaveCfg); i++) {
        if (g_wxCanSlaveCfg[i].moduleId == moduleId) {
            return &g_wxCanSlaveCfg[i];
        }
    }
    return NULL;
}


/* 不同指令码对应的Handle */
WxRmtCtrlReqHandle g_wxRmtCtrlReqHandles[WX_RMT_CTRL_CODE_BUTT] = {
    /* [指令码] = {内部消息类型，指令码PDU解码函数, 消息处理函数} */
    [WX_RMT_CTRL_CODE_RESET] = {WX_RMT_CTRL_REQ_MSG_TYPE_RESET, WX_CAN_SLAVE_DecRmtCtrlPduReset, WX_CAN_SLAVE_ProcRmtCtrlReqMsgReset},
};


UINT32 WX_CAN_SLAVE_CheckCanFrameMsg(WxCanFrameMsg *msg)
{
    if (msg->msgHead.msgDataLen != sizeof(WxCanFrame)) {
        wx_critical(WX_EXCP_INVALID_MSG_BODY_LEN, "Error Exit: Invalid msgLen(%u)", msg->msgHead.msgDataLen);
        return WX_INVALID_MSG_BODY_LEN;
    }

    if (msg->canFrame.dataLengCode == 0) {
        wx_excp_cnt(WX_EXCP_CAN_FRAME_DATA_LEN_ERR);
        return WX_CAN_SLAVE_CAN_FRAME_DATA_LEN_ERR;
    }

    return WX_SUCCESS;
}

/* 
 * 函数功能: 解封装FRAME, 把CAN frame解析成PDU
 * 返回值说明
 * WX_SUCCESS - 解封装成功
 * WX_TO_BE_CONTINUE - 解封装未完待续
 * 其他：解封装异常
 * 备注：该模块需要根据最终的卫星协议来确定
 **/
UINT32 WX_CAN_SLAVE_DecapCanFrame(WxCanSlave *this, WxCanFrame *canFrame, WxRmtCtrlPdu *canPdu)
{
    UINT32 ret = WX_CAN_SLAVE_CheckCanFrameMsg(canFrame);
    if (ret != WX_SUCCESS) {
        return ret;
    }

    canPdu->rmtCtrlCode = canFrame->data[0];
    return WX_SUCCESS;
}

/* 处理CAN驱动发送过来的CAN FRAME */
UINT32 WX_CAN_SLAVE_ProcCanFrameMsg(WxCanSlave *this, WxMsg *evtMsg)
{
    WxCanFrameMsg *canFrameMsg = (WxCanFrameMsg *)evtMsg;
    /* 解封装CAN Frame, 把CAN Frame组合出CAN PDU */
    UINT32 ret = WX_CAN_SLAVE_DecapCanFrame(this, &canFrameMsg->canFrame, &this->reqPdu);
    if (ret != WX_SUCCESS) {
        /* 当前CAN Frame不足以解析出PDU */
        if (ret == WX_TO_BE_CONTINUE) {
            return WX_SUCCESS;
        }
        return ret;
    }
    
    /* 对PDU进行解码获取最终的遥控请求消息 */
    WxRmtCtrlReqMsg *reqMsg = &this->reqMsg;
    ret = WX_CAN_SLAVE_DecRmtCtrlPdu(this, &this->reqPdu, reqMsg);
    if (ret != WX_SUCCESS) {
        return ret;
    }

    /* 处理解码后的消息 */
    ret = WX_CAN_SLAVE_ProcRmtCtrlReqMsg(this, reqMsg);
    if (ret != WX_SUCCESS) {
        return ret;
    }

    return WX_SUCCESS;
}

/* 模块构建函数 */
UINT32 WX_CAN_SLAVE_Construct(VOID *module)
{
    UINT32 ret;
    WxCanSlave *this = WX_Mem_Alloc(WX_GetModuleName(module), 1, sizeof(WxCanSlave));
    if (this == NULL) {
        return WX_ERR;
    }
   
    this->moduleId = WX_GetModuleId(module);
    this->cfgInfo =  WX_CanSlave_GetCfg(this->moduleId);
    if (this->cfgInfo == NULL) {
        wx_critical("Error Exit: WX_CanSlave_GetCfg(%u) fail", this->moduleId);
        return WX_CAN_SLAVE_MODULE_CFG_UNDEF;
    }
    /* 设置上Module */
    WX_SetModuleInfo(module, this);
    return WX_SUCCESS;
}

/* 模块析构函数 */
UINT32 WX_CAN_SLAVE_Destruct(VOID *module)
{
    WxCanSlave *this = WX_GetModuleInfo(module);
    if (this == NULL) {
        return WX_SUCCESS;
    }
    WX_Mem_Free(this);
    WX_SetModuleInfo(module, NULL);
    return WX_SUCCESS;
}

/* 模块消息处理入口 */
UINT32 WX_CAN_SLAVE_Entry(VOID *module, WxMsg *evtMsg)
{
    WxCanSlave *this = WX_GetModuleInfo(module);
    switch (evtMsg->msgType)
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