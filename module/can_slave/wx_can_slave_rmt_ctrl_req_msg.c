#include "wx_can_slave.h"
/* this WxCanSlave */
UINT32 WX_CAN_SLAVE_ProcRmtCtrlReqMsgReset(VOID *this, WxRmtCtrlReqMsg *reqMsg)
{
    /* DO reset */

	return WX_NOT_SUPPORT;
}

WxRmtCtrlReqMsgHandle g_wxRmtCtrlReqMsgHandle[WX_RMT_CTRL_REQ_MSG_TYPE_BUTT] = {
    [WX_RMT_CTRL_REQ_MSG_TYPE_RESET] = WX_CAN_SLAVE_ProcRmtCtrlReqMsgReset,
    /* if more please add here */
};

UINT32 WX_CAN_SLAVE_ProcRmtCtrlReqMsg(WxCanSlave *this, WxRmtCtrlReqMsg *reqMsg)
{
    if (reqMsg->type >= WX_RMT_CTRL_REQ_MSG_TYPE_BUTT) {
        wx_critical("Error Exit: unknown reqMsg->type(%u)", reqMsg->type);
        return WX_CAN_SLAVE_INVALID_REQ_MSG_TYPE;
    }
    WxRmtCtrlReqMsgHandle handle = g_wxRmtCtrlReqMsgHandle[reqMsg->type];
    if (handle == NULL) {
        return WX_CAN_SLAVE_UNDEFINE_REQ_MSG_HANDLE;
    }
    return handle(this, reqMsg);
}
