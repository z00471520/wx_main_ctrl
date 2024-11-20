#include "wx_can_slave_rmt_ctrl_req_msg.h"
UINT32 WX_CAN_SLAVE_ProcRmtCtrlReqMsgReset(WxCanSlave *this, WxRmtCtrlReqMsg *reqMsg)
{
    /* DO reset */

    /* 发送响应消息 */
}

/* 遥控请求消息处理的Handle表 */
WxRmtCtrlReqMsgHandle g_wxRmtCtrlReqMsgHandle[WX_RMT_CTRL_REQ_MSG_TYPE_BUTT] = {
    /* [请求消息类型]|请求消息处理函数 */
    [WX_RMT_CTRL_REQ_MSG_TYPE_RESET] = WX_CAN_SLAVE_ProcRmtCtrlReqMsgReset,
    /* if more please add here */
};

/* 处理遥控消息 */
UINT32 WX_CAN_SLAVE_ProcRmtCtrlReqMsg(WxCanSlave *this, WxRmtCtrlReqMsg *reqMsg)
{
    /* 检查类型是否合理 */
    if (reqMsg->type >= WX_RMT_CTRL_CODE_BUTT) {
        wx_critical(WX_EXCP_CAN_SLAVE_INVALID_REQ_MSG_TYPE, "Error Exit: unknown reqMsg->type(%u)", reqMsg->type);
        return WX_CAN_SLAVE_INVALID_REQ_MSG_TYPE;
    }
    /* 获取PDU解码函数 */
    WxRmtCtrlReqMsgHandle handle = g_wxRmtCtrlReqMsgHandle[msg->type].msgQueHandle;
    if (handle == NULL) {
        return WX_CAN_SLAVE_UNDEFINE_REQ_MSG_HANDLE;
    }
    return handle(this, reqMsg);
}
