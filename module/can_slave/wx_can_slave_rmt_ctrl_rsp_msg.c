#include "wx_can_slave_rmt_ctrl_rsp_msg.h"
#include "wx_can_slave.h"
UINT32 WX_CAN_SLAVE_EncodeRspPduReset(WxCanSlave *this, WxRmtCtrlRspMsg *rspMsg, WxRmtCtrlPdu *pdu)
{
    return;
}

/* 响应消息的编码为PDU */
WxRmtCtrlRspMsgEncPduHandle g_wxRmtCtrlRspMsgEncPduHandle[WX_RMT_CTRL_RSP_MSG_TYPE_BUTT] = {
    [WX_RMT_CTRL_RSP_MSG_TYPE_RESET] = WX_CAN_SLAVE_EncodeRspPduReset,
};

/* 把遥控消息编码成PDU */
UINT32 WX_CAN_SLAVE_EncodeRspPdu(WxCanSlave *this, WxRmtCtrlRspMsg *rspMsg, WxRmtCtrlPdu *pdu)
{
    if (rspMsg->type >= WX_RMT_CTRL_RSP_MSG_TYPE_BUTT) {
        return WX_CAN_SLAVE_ENC_RSP_PDU_TYPE_ERR;
    }

    WxRmtCtrlRspMsgEncPduHandle encHandle = g_wxRmtCtrlRspMsgEncPduHandle[rspMsg->type];
    if (encHandle == NULL) {
        return WX_CAN_SLAVE_UNDEFINE_RSP_PDU_ENC_HANDLE;
    }

    return encHandle(this, rspMsg, pdu);
}

/* 发送遥控消息到CAN接口 */
UINT32 WX_CAN_SLAVE_SendRspMsg2CanIf(WxCanSlave *this, WxRmtCtrlRspMsg *rspMsg)
{
    /* 消息编码为PDU */
    UINT32 ret = WX_CAN_SLAVE_EncodeRspPdu(this, rspMsg, &this->rspPdu);
    if (ret != WX_SUCCESS) {
        return ret;
    }
    
    /* 发送PDU到CANIF */
    ret = WX_CAN_SLAVE_SendPdu2CanIf(this, &this->rspPdu);
    if (ret != WX_SUCCESS) {
        return ret;
    }

    return WX_SUCCESS;
}
