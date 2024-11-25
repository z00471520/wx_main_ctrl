#include "wx_include.h"
#include "wx_rs422_master.h"
WxRs422Master g_wxRs422IMasterTask = {0};

/*
 * This is the configuration of the RS422I-master
 **/
WxRs422MasterCfgInfo g_wxRs422IMasterCfgInfo = {
    .rs422DevId = 0,
    .taskPri = 5,
    .rs422Format.BaudRate = 9600,
    .rs422Format.DataBits = XUN_FORMAT_8_BITS,
    .rs422Format.Parity = XUN_FORMAT_NO_PARITY,
    .rs422Format.StopBits = XUN_FORMAT_1_STOP_BIT,
};



WxMsgType WX_RS422_MASTER_GetRspMsgType(WxMsgType reqMsgType)
{
    switch (reqMsgType) {
        case WX_RS422_MASTER_MSG_READ_DATA: {
            return WX_RS422_MASTER_MSG_READ_DATA_RSP;
        }
        case WX_RS422_MASTER_MSG_WRITE_DATA: {
            return WX_RS422_MASTER_MSG_WRITE_DATA_RSP;
        }
        case WX_RS422_MASTER_MSG_READ_FILE: {
            return WX_RS422_MASTER_MSG_READ_FILE_RSP;
        }
        case WX_RS422_MASTER_MSG_WRITE_FILE: {
            return WX_RS422_MASTER_MSG_WRITE_FILE_RSP;
        }
        default:
            wx_excp_cnt(WX_EXCP_UNEXPECT_MSG_TYPE); 
            return reqMsgType;
    }
}

UINT32 WX_RS422_MASTER_ProcMsg(WxRs422Master *this, WxRs422IMasterMsg *msg)
{
    UINT32 ret = WX_RS422_MASTER_EncodeAdu(&this->rs422Msg, &this->txAdu);
    if (ret != WX_SUCCESS) {
        return ret;
    }
    /*
     * send the adu by RS422 buf
     */
    ret = WX_RS422_MASTER_TxAdu2Rs422MasterDriver(this, &this->txAdu);
    if (ret != WX_SUCCESS) {
        return ret;
    }
    /* 请求和相应是一条消息，只需要把消息类型反转一下即可，没有必须新建一个消息 */
    this->rs422Msg.msgType = WX_RS422_MASTER_GetRspMsgType(his->rs422Msg.msgType);
    
    /* wait to recieve the resbonse */
    ret = WX_RS422_MASTER_RxAdu(this);
    if (ret != WX_SUCCESS) {
        return ret;
    }
    
    /* decode the rx ADU into msg */
    ret = WX_RS422_MASTER_DecodeAdu(this, this->rxAdu, &this->rs422Msg);
    if (ret != WX_SUCCESS) {
        return ret;
    }

    return ret;
}

UINT32 WX_RS422_MASTER_Construct(VOID *module)
{
    UINT32 ret;
    WxRs422Master *this = WX_Mem_Alloc(WX_GetModuleName(module), 1, sizeof(WxRs422Master));
    if (this == NULL) {
        return WX_ERR;
    }
    WX_CLEAR_OBJ(this);

    /* 设置上Module */
    WX_SetModuleInfo(module, this);
    return WX_SUCCESS;
}

UINT32 WX_RS422_MASTER_Destruct(VOID *module)
{
    WxRs422Master *this = WX_GetModuleInfo(module);
    if (this == NULL) {
        return WX_SUCCESS;
    }
    WX_Mem_Free(this);
    WX_SetModuleInfo(module, NULL);
    return WX_SUCCESS;
}

UINT32 WX_RS422_MASTER_TxAdu2Rs422MasterDriver(WxRs422Master *this, WxAdu *adu)
{
    /* 申请消息 */
    WxMsg *msg = WX_ApplyEvtMsg(WX_MSG_TYPE_CAN_FRAME);
    if (msg == NULL) {
        return WX_APPLY_EVT_MSG_ERR;
    }
    /* 初始化消息头 */
    WX_CLEAR_OBJ(msg);
    /* 填写消息信息 */
    msg->sender = this->moduleId;
    msg->receiver = WX_MODULE_DRIVER_RS422_MASTER;
    msg->msgType = WX_MSG_TYPE_RS422_MASTER_DRIVER;
    msg->msgDataLen = sizeof(WxModbusAdu);
    /* 填写数据内容 */
    WxModbusAdu *modbusAdu = (WxModbusAdu *)msg->msgData;
    for (int i = 0; i < adu->dataLen; i++) {
        modbusAdu->data[i] = adu->data[i];
    }
    modbusAdu->dataLen = adu->dataLen;
    modbusAdu->expectRspLen = adu->expectRspLen;

    /* 发送消息 */
    UINT32 ret = WX_MsgShedule(his->moduleId, msg->msgHead.receiver, msg);
    if (ret != WX_SUCCESS) {
        WX_FreeEvtMsg(&msg);
    }

    return ret;
}

/* 处理RS422驱动收到的响应ADU */
UINT32 WX_RS422_MASTER_ProcAduRspMsg(WxRs422Master *this, WxMsg *msg)
{

}

/* 处理写数据请求 */
UINT32 WX_RS422_MASTER_ProcWrDataReqMsg(WxRs422Master *this, WxMsg *msg)
{
    UINT32 ret;
    ret = WX_RS422_MASTER_EncodeWrDataMsg2Adu(msg, &this->txAdu);
    if (ret != WX_SUCCESS) {
        return ret;
    }
    /*
     * send the adu by RS422 buf
     */
    ret = WX_RS422_MASTER_TxAdu2Rs422MasterDriver(this, &this->txAdu);
    if (ret != WX_SUCCESS) {
        return ret;
    }
}

/* 处理读数据请求 */
UINT32 WX_RS422_MASTER_ProcRdDataReqMsg(WxRs422Master *this, WxMsg *msg)
{

}

UINT32 WX_RS422_MASTER_Entry(VOID *module, WxMsg *msg)
{
    WxRs422Master *this = WX_GetModuleInfo(module);
    switch (msg->msgType) {
        case WX_MSG_TYPE_RS422_MASTER_ADU_RSP: {
            return WX_RS422_MASTER_ProcAduRspMsg(this, msg);
        }
        case WX_MSG_TYPE_RS422_MASTER_RD_DATA_REQ: {
            return WX_RS422_MASTER_ProcRdDataReqMsg(this, msg);
        }
        case WX_MSG_TYPE_RS422_MASTER_WR_DATA_REQ: {
            return WX_RS422_MASTER_ProcWrDataReqMsg(this, msg);
        }
    }
}