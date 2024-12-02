#include "wx_include.h"
#include "wx_can_slave.h"
#include "wx_can_slave.h"
#include "wx_id_def.h"
#include "wx_can_slave_rmt_ctrl_pdu.h"
#include "wx_msg_can_frame_intf.h"
#include "wx_can_slave_rmt_ctrl_pdu.h"
#include "wx_can_slave_rmt_ctrl_req_msg.h"
#include "wx_can_slave_rmt_ctrl_rsp_msg.h"

 #include "wx_frame.h"
WxCanSlaveCfg g_wxCanSlaveCfg[] = {
    {
        .canFrameDataLen = 8,
        .moduleId = WX_MODULE_CAN_SLAVE_A,
    }, 
    {
        .canFrameDataLen = 8,
        .moduleId = WX_MODULE_CAN_SLAVE_B,
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
    [WX_RMT_CTRL_CODE_RESET] = {
    	WX_RMT_CTRL_REQ_MSG_TYPE_RESET,
		WX_CAN_SLAVE_DecRmtCtrlPduReset,
		WX_CAN_SLAVE_ProcRmtCtrlReqMsgReset
    },
};


UINT32 WX_CAN_SLAVE_CheckCanFrameMsg(WxCanFrame *canFrame)
{
    if (canFrame->dataLengCode == 0) {
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

UINT32 WX_CAN_SLAVE_ProcCanFrameMsg(WxCanSlave *this, WxMsg *evtMsg)
{
    WxCanFrameMsg *canFrameMsg = (WxCanFrameMsg *)evtMsg;
    UINT32 ret = WX_CAN_SLAVE_DecapCanFrame(this, &canFrameMsg->canFrame, &this->reqPdu);
    if (ret != WX_SUCCESS) {
        if (ret == WX_TO_BE_CONTINUE) {
            return WX_SUCCESS;
        }
        return ret;
    }
    
    WxRmtCtrlReqMsg *reqMsg = &this->reqMsg;
    ret = WX_CAN_SLAVE_DecRmtCtrlPdu(this, &this->reqPdu, reqMsg);
    if (ret != WX_SUCCESS) {
        return ret;
    }

    ret = WX_CAN_SLAVE_ProcRmtCtrlReqMsg(this, reqMsg);
    if (ret != WX_SUCCESS) {
        return ret;
    }

    return WX_SUCCESS;
}

UINT32 WX_CAN_SLAVE_Construct(VOID *module)
{
    WxCanSlave *this = WX_Mem_Alloc(WX_GetModuleName(module), 1, sizeof(WxCanSlave));
    if (this == NULL) {
        return WX_MEM_ALLOC_FAIL;
    }
   
    this->moduleId = WX_GetModuleId(module);
    this->cfgInfo =  WX_CanSlave_GetCfg(this->moduleId);
    if (this->cfgInfo == NULL) {
        wx_critical("Error Exit: WX_CanSlave_GetCfg(%u) fail", this->moduleId);
        return WX_CAN_SLAVE_MODULE_CFG_UNDEF;
    }
    WX_SetModuleInfo(module, this);
    return WX_SUCCESS;
}

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
            return WX_CAN_SLAVE_UNSPT_MSG_TYPE;
        }
    }
}
