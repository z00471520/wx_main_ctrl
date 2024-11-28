#include "wx_include.h"
#include "wx_can_slave.h"
#include "wx_can_slave.h"
#include "wx_id_def.h"
#include  "wx_can_slave_rmt_ctrl_pdu.h"
WxCanSlaveCfg g_wxCanSlaveCfg[] = {
    {
        /* 鑷畾涔夐厤缃� */
        .canFrameDataLen = 8,
        .moduleId = WX_MODULE_CAN_SLAVE_A,
        /* 璁惧閰嶇疆 */
        .deviceCfgInfo.isEnable = FALSE, /* 鏄惁浣胯兘 */
        .deviceCfgInfo.baudPrescalar = 0,
        /* 涓柇閰嶇疆 */
        .intrCfgInfo.intrId = 0,
        .intrCfgInfo.callBackRef = 0,
    }, 
    {
         /* 鑷畾涔夐厤缃� */
        .canFrameDataLen = 8,
        .moduleId = WX_MODULE_CAN_SLAVE_B,
        /* 璁惧閰嶇疆 */
        .deviceCfgInfo.isEnable = FALSE, /* 鏄惁浣胯兘 */
        .deviceCfgInfo.baudPrescalar = 0,
        /* 涓柇閰嶇疆 */
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

WxRmtCtrlReqHandle g_wxRmtCtrlReqHandles[WX_RMT_CTRL_CODE_BUTT] = {
    /* 指令码						消息类型	消息解码函数		消息处理函数 */
    [WX_RMT_CTRL_CODE_RESET] = {WX_RMT_CTRL_REQ_MSG_TYPE_RESET, WX_CAN_SLAVE_DecRmtCtrlPduReset, WX_CAN_SLAVE_ProcRmtCtrlReqMsgReset},
};


UINT32 WX_CAN_SLAVE_CheckCanFrameMsg(WxCanFrameMsg *msg)
{
    if (msg->canFrame.dataLengCode == 0) {
        wx_excp_cnt(WX_EXCP_CAN_FRAME_DATA_LEN_ERR);
        return WX_CAN_SLAVE_CAN_FRAME_DATA_LEN_ERR;
    }

    return WX_SUCCESS;
}

UINT32 WX_CAN_SLAVE_DecapCanFrame(WxCanSlave *this, WxCanFrame *canFrame, WxRmtCtrlPdu *canPdu)
{
    UINT32 ret = WX_CAN_SLAVE_CheckCanFrameMsg(canFrame);
    if (ret != WX_SUCCESS) {
        return ret;
    }

    canPdu->rmtCtrlCode = canFrame->data[0];
    return WX_SUCCESS;
}

/* 澶勭悊CAN椹卞姩鍙戦�佽繃鏉ョ殑CAN FRAME */
UINT32 WX_CAN_SLAVE_ProcCanFrameMsg(WxCanSlave *this, WxMsg *evtMsg)
{
    WxCanFrameMsg *canFrameMsg = (WxCanFrameMsg *)evtMsg;
    /* 瑙ｅ皝瑁匔AN Frame, 鎶奀AN Frame缁勫悎鍑篊AN PDU */
    UINT32 ret = WX_CAN_SLAVE_DecapCanFrame(this, &canFrameMsg->canFrame, &this->reqPdu);
    if (ret != WX_SUCCESS) {
        /* 褰撳墠CAN Frame涓嶈冻浠ヨВ鏋愬嚭PDU */
        if (ret == WX_TO_BE_CONTINUE) {
            return WX_SUCCESS;
        }
        return ret;
    }
    
    /* 瀵筆DU杩涜瑙ｇ爜鑾峰彇鏈�缁堢殑閬ユ帶璇锋眰娑堟伅 */
    WxRmtCtrlReqMsg *reqMsg = &this->reqMsg;
    ret = WX_CAN_SLAVE_DecRmtCtrlPdu(this, &this->reqPdu, reqMsg);
    if (ret != WX_SUCCESS) {
        return ret;
    }

    /* 澶勭悊瑙ｇ爜鍚庣殑娑堟伅 */
    ret = WX_CAN_SLAVE_ProcRmtCtrlReqMsg(this, reqMsg);
    if (ret != WX_SUCCESS) {
        return ret;
    }

    return WX_SUCCESS;
}

/* 妯″潡鏋勫缓鍑芥暟 */
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
    /* 璁剧疆涓奙odule */
    WX_SetModuleInfo(module, this);
    return WX_SUCCESS;
}

/* 妯″潡鏋愭瀯鍑芥暟 */
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

/* 妯″潡娑堟伅澶勭悊鍏ュ彛 */
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
