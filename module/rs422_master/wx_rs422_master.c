#include "wx_include.h"
#include "wx_rs422_master.h"
#include "wx_rs422_master_rd_data_rsp_intf.h"
#include "wx_rs422_master_wr_data_rsp_intf.h"
#include  "wx_rs422_master_driver_intf.h"
#include "wx_rs422_master_adu_rsp_intf.h"
#include "wx_msg_res_pool.h"
#include "wx_frame.h"
UINT32 WX_RS422_MASTER_SendAdu2Driver(WxRs422Master *this, WxRs422MasterDriverTxDataReq *txDataReq)
{
    /* create a event message */
    WxRs422MasterDriverMsg *msg = (WxRs422MasterDriverMsg *)WX_ApplyEvtMsg(WX_MSG_TYPE_RS422_MASTER_DRIVER_REQ);
    if (msg == NULL) {
        return WX_APPLY_EVT_MSG_ERR;
    }

    /* init the message */
    msg->sender = this->moduleId;
    msg->receiver = WX_MODULE_RS422_MASTER_DRIVER;
    msg->msgType = WX_MSG_TYPE_RS422_MASTER_DRIVER_REQ;
    msg->subMsgType = WX_SUB_MSG_REQ_RS422_MASTER_DRIVER_TX_ADU;
    msg->reqTxData = *txDataReq;

    /* send msg to driver */
    UINT32 ret = WX_MsgShedule(this->moduleId, msg->receiver, msg);
    if (ret != WX_SUCCESS) {
        WX_FreeEvtMsg((WxMsg **)&msg);
    }

    return ret;
}

/* Process the read data response message of the driver */
UINT32 WX_RS422_MASTER_ProcRdDataRspFromDriver(WxRs422Master *this, WxRs422MasterDriverRspMsg *slaveRspMsg)
{
    UINT32 ret = WX_SUCCESS;
    if (slaveRspMsg->data.reqSubMsgType >= WX_RS422_MASTER_MSG_READ_DATA_BUTT) {
        wx_excp_cnt(WX_EXCP_UNEXPECT_MSG_TYPE);
        return WX_RS422_MASTER_PROC_RSP_ADU_SUB_MSG_TYPE_ERR;
    }

    /* to upper layer */
    WxRs422MasterRdDataRspMsg *masterRspMsg = WX_ApplyEvtMsg(WX_MSG_TYPE_RS422_MASTER_RD_DATA_RSP);
    if (masterRspMsg == NULL) {
        return WX_APPLY_EVT_MSG_ERR;
    }
    /* set the message data */
    masterRspMsg->subMsgType = slaveRspMsg->data.reqMsgType;
    masterRspMsg->sender = this->moduleId;
    /* rs422 driver will set the sender to origin sender */
    masterRspMsg->receiver = slaveRspMsg->data.reqSender;
    if (slaveRspMsg->data.rspCode != WX_SUCCESS) {
        masterRspMsg->rsp.failCode = slaveRspMsg->data.rspCode;
    } else {
    	/* decode the data */
        masterRspMsg->rsp.failCode = WX_RS422_MASTER_DecRdDataRsp(&slaveRspMsg->data, &masterRspMsg->rsp.data);
    }
    ret = WX_MsgShedule(masterRspMsg->sender, masterRspMsg->receiver, masterRspMsg);
    if (ret != WX_SUCCESS) {
        WX_FreeEvtMsg((WxMsg **)&masterRspMsg);
    }

    return ret;
}

UINT32 WX_RS422_MASTER_ProcDriverWrDataRsp(WxRs422Master *this, WxRs422MasterDriverRspMsg *rspMsg)
{
    /* apply the message */
    WxRs422MasterWrDatRspMsg *wrDataRspMsg = WX_ApplyEvtMsg(WX_MSG_TYPE_RS422_MASTER_WR_DATA_RSP);
    if (wrDataRspMsg == NULL) {
        return WX_APPLY_EVT_MSG_ERR;
    }
    wrDataRspMsg->rsp.subMsgType = rspMsg->data.reqSubMsgType;
    wrDataRspMsg->rsp.failCode = 0;
    wrDataRspMsg->sender = this->moduleId;
    wrDataRspMsg->receiver = rspMsg->data.reqSender;
    UINT32 ret = WX_MsgShedule(this->moduleId, wrDataRspMsg->receiver, wrDataRspMsg);
    if (ret != WX_SUCCESS) {
        WX_FreeEvtMsg((WxMsg **)&wrDataRspMsg);
    }
    return ret;
}

/* proc the response ADU message */
UINT32 WX_RS422_MASTER_ProRspcAduMsg(WxRs422Master *this, WxRs422MasterDriverRspMsg *rspMsg)
{

    /* based on the message type, process the response ADU */
    switch (rspMsg->data.reqMsgType) {
        case WX_MSG_TYPE_RS422_MASTER_WR_DATA_REQ: {
            return WX_RS422_MASTER_ProcDriverWrDataRsp (this, rspMsg);
        }
        case WX_MSG_TYPE_RS422_MASTER_RD_DATA_REQ: {
            return WX_RS422_MASTER_ProcRdDataRspFromDriver(this, rspMsg);
        }
        default: {
            return WX_RS422_MASTER_RECV_UNSPT_MSG_TYPE;
        }
    }
}

/* |salve address: 1byte| func code: 1byte| data addr: 2byte | data len : 1byte | data: N | crc: 2byte | */
UINT32 WX_RS422_MASTER_EncWrDataReq(WxRs422MasterWrDataReqMsg *msg, WxRs422MasterDriverTxDataReq *wrDataReq)
{
    WX_CLEAR_OBJ(wrDataReq);
    WxRs422MasterWrDataEncHandle *handle = WX_RS422_MASTER_GetWrDataHandle(msg->subMsgType);
    if (handle->encStruct == NULL) {
        return WX_RS422_MASTER_WR_REQ_ENCODE_FUNC_UNDEF;
    }
    wrDataReq->msgType = msg->msgType;
    wrDataReq->subMsgType = msg->subMsgType;
    WxModbusAdu *adu = &wrDataReq->adu;
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

    /* data length not decided yet, so ignore it */
    adu->valueLen = 5;
   
    /* set the data field by encode function */
    UINT8 dataLen = handle->encStruct(adu, &msg->wrData);
    if (dataLen == 0) {
        return WX_RS422_MASTER_WR_REQ_ENCODE_FAIL;
    }
    /* here we set the data length to the adu data len */
    adu->value[WX_MODBUS_ADU_WR_REQ_DATA_LEN_IDX] = dataLen;
    
    /* to calc the crc value */
    UINT16 crcValue = WX_Modbus_Crc16(adu->value, adu->valueLen);
    adu->value[adu->valueLen++] = (UINT8)((crcValue >> 8) & 0xff);   
    adu->value[adu->valueLen++] = (UINT8)(crcValue & 0xff); 
    adu->expectRspLen = adu->valueLen; /* rsp len is the same as write length  */
    return WX_SUCCESS;
}

/*
 * ADU: |slave address:1byte| func code: 1byte | data address: 2byte | data len：1byte |
 **/
UINT32 WX_RS422_MASTER_EncRdDataReq(WxRs422MasterRdDataReqMsg *msg, WxRs422MasterDriverTxDataReq *rdDataReq)
{
    WX_CLEAR_OBJ(rdDataReq);
    /* check the sub operation type */
    if (msg->subMsgType >= WX_RS422_MASTER_MSG_READ_DATA_BUTT) {
        return WX_RS422_MASTER_INVALID_SUB_OPR_TYPE;
    }
    WxModbusAdu *txAdu = &rdDataReq->adu;
    rdDataReq->msgType = msg->msgType;
    rdDataReq->subMsgType = msg->subMsgType;
    WxRs422MasterRdDataHandle *handle = &g_wxRs422MasterReadDataHandles[msg->subMsgType];
    /* the length 0 means that you not define the encode info, wtf! */
    if (handle->dataLen == 0) {
        return WX_RS422_MASTER_READ_REQ_ENCODE_INFO_UNDEF;
    }
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
    /* 鏈熸湜鍝嶅簲鐨勯暱搴�
     * 鍝嶅簲鐨勬暟鎹牸寮忓涓嬪涓�:
     * |浠庣珯锛�1byte | 鍔熻兘鐮侊細1byte | 鏁版嵁闀垮害锛�1byte | 鏁版嵁锛� N byte | CRC: 2BYTE|
     **/
    txAdu->expectRspLen = 1 + 1 + 1 + handle->dataLen + WX_MODBUS_CRC_LEN;

    return WX_SUCCESS;
}

/* proc write data request message */
UINT32 WX_RS422_MASTER_ProcWrDataReqMsg(WxRs422Master *this, WxRs422MasterWrDataReqMsg *msg)
{
    UINT32 ret;
    WxRs422MasterDriverTxDataReq *txDataReq = &this->txDataReq;
    /* encode msg 2 adu */
    ret = WX_RS422_MASTER_EncWrDataReq(msg, txDataReq);
    if (ret != WX_SUCCESS) {
        return ret;
    }
    /* send adu 2 driver */
    ret = WX_RS422_MASTER_SendAdu2Driver(this, txDataReq);
    if (ret != WX_SUCCESS) {
        return ret;
    }
    return WX_SUCCESS;
}

/* proc read data request message */
UINT32 WX_RS422_MASTER_ProcRdDataReqMsg(WxRs422Master *this, WxRs422MasterRdDataReqMsg *msg)
{
    UINT32 ret;
    WxRs422MasterDriverTxDataReq *txDataReq = &this->txDataReq;
    /* endcode msg 2 adu */
    ret = WX_RS422_MASTER_EncRdDataReq(msg, txDataReq);
    if (ret != WX_SUCCESS) {
        return ret;
    }
    /* send adu 2 driver */
    ret = WX_RS422_MASTER_SendAdu2Driver(this, txDataReq);
    if (ret != WX_SUCCESS) {
        return ret;
    }
    return WX_SUCCESS;
}


UINT32 WX_RS422_MASTER_Construct(VOID *module)
{
    WxRs422Master *this = WX_Mem_Alloc(WX_GetModuleName(module), 1, sizeof(WxRs422Master));
    if (this == NULL) {
        return WX_MEM_ALLOC_FAIL;
    }
    WX_CLEAR_OBJ(this);
    this->moduleId = WX_GetModuleId(module);

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
    wx_debug("module %s entry msg type %u", WX_GetModuleName(module), msg->msgType);
    UINT32 ret;
    switch (msg->msgType) {
        case WX_MSG_TYPE_RS422_MASTER_ADU_RSP: {
            ret = WX_RS422_MASTER_ProRspcAduMsg(this, (WxRs422MasterDriverRspMsg *)msg);
            break;
        }
        case WX_MSG_TYPE_RS422_MASTER_RD_DATA_REQ: {
            ret = WX_RS422_MASTER_ProcRdDataReqMsg(this, (WxRs422MasterRdDataReqMsg *)msg);
            break;
        }
        case WX_MSG_TYPE_RS422_MASTER_WR_DATA_REQ: {
            ret = WX_RS422_MASTER_ProcWrDataReqMsg(this, (WxRs422MasterWrDataReqMsg *)msg);
            break;
        }
        default: {
        	ret = WX_RS422_MASTER_UNSPT_MSGTYPE;
            wx_critical("Error Exit:unsppt msg type(%u)", msg->msgType);
        	break;
        }
    }
    wx_debug("module return(%u)", ret);
    return ret;
}
