#include "wx_include.h"
#include "wx_rs422_master.h"
#include "wx_rs422_master_rd_data_rsp_intf.h"
#include "wx_rs422_master_wr_data_rsp_intf.h"
 
#include "wx_rs422_master_adu_rsp_intf.h"
#include "wx_msg_res_pool.h"
 #include "wx_frame.h"
UINT32 WX_RS422_MASTER_SendAdu2Driver(WxRs422Master *this, WxModbusAdu *adu)
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
    /* 填写数据内容 */
    WxModbusAdu *modbusAdu = (WxModbusAdu *)msg->msgData;
    for (int i = 0; i < adu->valueLen; i++) {
        modbusAdu->value[i] = adu->value[i];
    }
    modbusAdu->valueLen = adu->valueLen;
    modbusAdu->expectRspLen = adu->expectRspLen;

    /* 发送消息 */
    UINT32 ret = WX_MsgShedule(this->moduleId, msg->receiver, msg);
    if (ret != WX_SUCCESS) {
        WX_FreeEvtMsg(&msg);
    }

    return ret;
}

/* RS422主读其它设备最终返回的响应ADU */
UINT32 WX_RS422_MASTER_ProcRdDataRspAduMsg(WxRs422Master *this, WxRs422MasterRspAduMsg *rspAduMsg)
{
    UINT32 ret = WX_SUCCESS;
    WxModbusAdu *rxAdu = &rspAduMsg->rspAdu;
    if (rxAdu->subMsgType >= WX_RS422_MASTER_MSG_READ_DATA_BUTT) {
        wx_excp_cnt(WX_EXCP_UNEXPECT_MSG_TYPE);
        return WX_RS422_MASTER_PROC_RSP_ADU_SUB_MSG_TYPE_ERR;
    }

    /* 申请响应消息 */
    WxRs422MasterRdDataRspMsg *rdDataRspMsg = WX_ApplyEvtMsg(WX_MSG_TYPE_RS422_MASTER_RD_DATA_RSP);
    /* 初始化消息 */
    WX_CLEAR_OBJ((WxMsg *)rdDataRspMsg);
    rdDataRspMsg->subMsgType = rxAdu->subMsgType;
    /* 谁读的就发给谁 */
    rdDataRspMsg->receiver = this->rdDataModule[rspAduMsg->rspAdu.subMsgType];
    /* 如果失败告诉对方失败码 */
    if (rspAduMsg->rspAdu.failCode != WX_SUCCESS) {
        rdDataRspMsg->rsp.failCode = rspAduMsg->rspAdu.failCode;
    } else {
    	/* 需要进行解码 */
        rdDataRspMsg->rsp.failCode = WX_RS422_MASTER_DecRdDataAdu(rxAdu, &rdDataRspMsg->rsp.data);
    }
    ret = WX_MsgShedule(this->moduleId, rdDataRspMsg->receiver, rdDataRspMsg);
    if (ret != WX_SUCCESS) {
        WX_FreeEvtMsg((WxMsg **)&rdDataRspMsg);
    }

    return ret;
}

UINT32 WX_RS422_MASTER_ProcWrDataRspAduMsg(WxRs422Master *this, WxRs422MasterRspAduMsg *rspAduMsg)
{
    /* 申请响应消息 */
    WxRs422MasterWrDatRspMsg *wrDataRspMsg = WX_ApplyEvtMsg(WX_MSG_TYPE_RS422_MASTER_WR_DATA_RSP);
    /* 初始化消息 */
    WX_CLEAR_OBJ((WxMsg *)wrDataRspMsg);
    wrDataRspMsg->rsp.subMsgType = rspAduMsg->rspAdu.subMsgType;
    wrDataRspMsg->rsp.failCode = rspAduMsg->rspAdu.failCode;

    UINT8 reciever = this->wrDataModule[rspAduMsg->rspAdu.subMsgType];
    UINT32 ret = WX_MsgShedule(this->moduleId, reciever, wrDataRspMsg);
    if (ret != WX_SUCCESS) {
        WX_FreeEvtMsg((WxMsg **)&wrDataRspMsg);
    }
    return ret;
}

/* 处理从RS422驱动收到的响应ADU消息 */
UINT32 WX_RS422_MASTER_ProRspcAduMsg(WxRs422Master *this, WxMsg *msg)
{
    WxRs422MasterRspAduMsg *rspAduMsg = (WxRs422MasterRspAduMsg *)msg;

    /* 响应消息对应的请求消息 */
    switch (rspAduMsg->rspAdu.msgType) {
        case WX_MSG_TYPE_RS422_MASTER_WR_DATA_REQ: {
            return WX_RS422_MASTER_ProcWrDataRspAduMsg (this, rspAduMsg);
        }
        case WX_MSG_TYPE_RS422_MASTER_RD_DATA_REQ: {
            return WX_RS422_MASTER_ProcRdDataRspAduMsg(this, rspAduMsg);
        }
        default: {
            return WX_RS422_MASTER_RECV_UNSPT_MSG_TYPE;
        }
    }
}

/* |salve address: 1byte| func code: 1byte| data addr: 2byte | data len：1byte | data: N | */
UINT32 WX_RS422_MASTER_EncWrDataReqMsg2Adu(WxRs422MasterWrDataReqMsg *msg, WxModbusAdu *adu)
{
    WxRs422MasterWrDataEncHandle *handle = WX_RS422_MASTER_GetWrDataHandle(msg->subMsgType);
    if (handle->encStruct == NULL) {
        return WX_RS422_MASTER_WR_REQ_ENCODE_FUNC_UNDEF;
    }
    UINT16 dataAddr = (UINT16)handle->dataAddr;
    /* slave address */
    adu->value[WX_MODBUS_ADU_WR_REQ_SLAVE_ADDR_IDX] = handle->slaveDevice;      
    /* func code */        
    adu->value[WX_MODBUS_ADU_WR_REQ_FUNC_CODE_IDX] = WX_MODBUS_FUNC_CODE_WRITE_DATA;   
    /* data address hi */ 
    adu->value[WX_MODBUS_ADU_WR_REQ_DATA_ADDR_HI_IDX] = (UINT8)((dataAddr >> 8) & 0xff);   
    /* data address lo */
    adu->value[WX_MODBUS_ADU_WR_REQ_DATA_ADDR_LO_IDX] = (UINT8)(dataAddr & 0xff);         
    adu->valueLen = 4;

    /* the length size can be used to encode */
    UINT16 lelfLen = WX_MODBUS_ADU_MAX_SIZE - adu->valueLen - WX_MODBUS_CRC_LEN; /* the crc and adu len */
    /* 4 is the data length */
    adu->value[WX_MODBUS_ADU_WR_REQ_DATA_LEN_IDX] = 
        handle->encStruct(&adu->value[WX_MODBUG_ADU_WR_REQ_DATA_START_IDX], lelfLen, &msg->wrData);
    if (adu->value[WX_MODBUS_ADU_WR_REQ_DATA_LEN_IDX] == 0) {
        return WX_RS422_MASTER_WR_REQ_ENCODE_FAIL;
    }
     /* 编码adu[4]存放的是长度，其本身占用1字节  */
    adu->valueLen += adu->value[WX_MODBUS_ADU_WR_REQ_DATA_LEN_IDX] + 1;
    /* to calc the crc value */
    UINT16 crcValue = WX_Modbus_Crc16(adu->value, adu->valueLen);
    adu->value[adu->valueLen++] = (UINT8)((crcValue >> 8) & 0xff);   
    adu->value[adu->valueLen++] = (UINT8)(crcValue & 0xff); 
    adu->expectRspLen = adu->valueLen; /* 写多少字节，响应就应该是多少字节 */
    return WX_SUCCESS;
}

/*
 * 函数功能: 对读数据请求消息进行编码，编码后的数据存放在txAdu中
 * ADU的格式: |slave address：1byte| func code: 1byte | data address: 2byte | data len：1byte |
 * 注意事项：参数合法性由调用者保证
 **/
UINT32 WX_RS422_MASTER_EncRdDataReqMsg2Adu(WxRs422MasterRdDataReqMsg *msg, WxModbusAdu *txAdu)
{
    /* 判断消息类型是否合理 */
    if (msg->subMsgType >= WX_RS422_MASTER_MSG_READ_DATA_BUTT) {
        return WX_RS422_MASTER_INVALID_SUB_OPR_TYPE;
    }
    WxRs422MasterRdDataHandle *handle = &g_wxRs422MasterReadDataHandles[msg->subMsgType];
    /* the length 0 means that you not define the encode info, wtf! */
    if (handle->dataLen == 0) {
        return WX_RS422_MASTER_READ_REQ_ENCODE_INFO_UNDEF;
    }
    txAdu->msgType = msg->msgType,
    txAdu->subMsgType = msg->subMsgType;
    txAdu->valueLen = 0;
    /* slave address */
    txAdu->value[txAdu->valueLen++] = handle->slaveDevice;
    /* func code */
    txAdu->value[txAdu->valueLen++] = WX_MODBUS_FUNC_CODE_READ_DATA;
    /* data address hi */
    txAdu->value[txAdu->valueLen++] = (UINT8)((handle->dataAddr >> 8) & 0xff); 
    /* data address lo */
    txAdu->value[txAdu->valueLen++] = (UINT8)(handle->dataAddr & 0xff); 
    /* data len */
    txAdu->value[txAdu->valueLen++] = (UINT8)handle->dataLen; 
    /* crc value */
    UINT16 crcValue = WX_Modbus_Crc16(txAdu->value, txAdu->valueLen);
    txAdu->value[txAdu->valueLen++] = (UINT8)((crcValue >> 8) & 0xff);   
    txAdu->value[txAdu->valueLen++] = (UINT8)(crcValue & 0xff); 
    /* 期望响应的长度
     * 响应的数据格式如下如下:
     * |从站：1byte | 功能码：1byte | 数据长度：1byte | 数据： N byte | CRC: 2BYTE|
     **/
    txAdu->expectRspLen = 1 + 1 + 1 + handle->dataLen + WX_MODBUS_CRC_LEN;

    return WX_SUCCESS;
}

/* 处理其他模块通过RS422写数据给外设的请求 */
UINT32 WX_RS422_MASTER_ProcWrDataReqMsg(WxRs422Master *this, WxMsg *msg)
{
    UINT32 ret;
    /* 把消息编码为ADU */
    ret = WX_RS422_MASTER_EncWrDataReqMsg2Adu((WxRs422MasterWrDataReqMsg*)msg, &this->txAdu);
    if (ret != WX_SUCCESS) {
        return ret;
    }
    /* 发送ADU到RS422 Master的驱动 */
    ret = WX_RS422_MASTER_SendAdu2Driver(this, &this->txAdu);
    if (ret != WX_SUCCESS) {
        return ret;
    }
    /* 记录发送者用于 */
    this->wrDataModule[msg->subMsgType] = msg->sender;
    return WX_SUCCESS;
}

/* 处理读数据请求 */
UINT32 WX_RS422_MASTER_ProcRdDataReqMsg(WxRs422Master *this, WxMsg *msg)
{
    UINT32 ret;
    /* 编码读数据请求到MODBUS ADU */
    ret = WX_RS422_MASTER_EncRdDataReqMsg2Adu((WxRs422MasterRdDataReqMsg *)msg, &this->txAdu);
    if (ret != WX_SUCCESS) {
        return ret;
    }
    /* 发送ADU到RS422 Master的驱动 */
    ret = WX_RS422_MASTER_SendAdu2Driver(this, &this->txAdu);
    if (ret != WX_SUCCESS) {
        return ret;
    }
    this->rdDataModule[msg->subMsgType] = msg->sender;
    return WX_SUCCESS;
}


UINT32 WX_RS422_MASTER_Construct(VOID *module)
{
    WxRs422Master *this = WX_Mem_Alloc(WX_GetModuleName(module), 1, sizeof(WxRs422Master));
    if (this == NULL) {
        return WX_MEM_ALLOC_FAIL;
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

UINT32 WX_RS422_MASTER_Entry(VOID *module, WxMsg *msg)
{
    WxRs422Master *this = WX_GetModuleInfo(module);
    switch (msg->msgType) {
        case WX_MSG_TYPE_RS422_MASTER_ADU_RSP: {
            return WX_RS422_MASTER_ProRspcAduMsg(this, msg);
        }
        case WX_MSG_TYPE_RS422_MASTER_RD_DATA_REQ: {
            return WX_RS422_MASTER_ProcRdDataReqMsg(this, msg);
        }
        case WX_MSG_TYPE_RS422_MASTER_WR_DATA_REQ: {
            return WX_RS422_MASTER_ProcWrDataReqMsg(this, msg);
        }
        default: {
        	break;
        }
    }

    return WX_RS422_MASTER_UNSPT_MSGTYPE;
}
