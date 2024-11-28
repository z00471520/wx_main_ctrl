#include "wx_can_slave_rmt_ctrl_req_msg.h"
#include "wx_can_slave.h"
UINT32 WX_CAN_SLAVE_ProcRmtCtrlReqMsgReset(WxCanSlave *this, WxRmtCtrlReqMsg *reqMsg)
{
    /* DO reset */

	return WX_ERR;
}

WxRmtCtrlReqMsgHandle g_wxRmtCtrlReqMsgHandle[WX_RMT_CTRL_REQ_MSG_TYPE_BUTT] = {
    [WX_RMT_CTRL_REQ_MSG_TYPE_RESET] = WX_CAN_SLAVE_ProcRmtCtrlReqMsgReset,
    /* if more please add here */
};

UINT32 WX_CAN_SLAVE_ProcRmtCtrlReqMsg(WxCanSlave *this, WxRmtCtrlReqMsg *reqMsg)
{
    if (reqMsg->type >= WX_RMT_CTRL_CODE_BUTT) {
        wx_critical(WX_EXCP_CAN_SLAVE_INVALID_REQ_MSG_TYPE, "Error Exit: unknown reqMsg->type(%u)", reqMsg->type);
        return WX_CAN_SLAVE_INVALID_REQ_MSG_TYPE;
    }
    WxRmtCtrlReqMsgHandle handle = g_wxRmtCtrlReqMsgHandle[reqMsg->type];
    if (handle == NULL) {
        return WX_CAN_SLAVE_UNDEFINE_REQ_MSG_HANDLE;
    }
    return handle(this, reqMsg);
}
