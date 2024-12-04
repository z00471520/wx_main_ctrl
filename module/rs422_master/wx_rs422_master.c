#include "wx_include.h"
#include "wx_rs422_master.h"
#include "wx_rs422_master_rd_data_rsp_intf.h"
#include "wx_rs422_master_wr_data_rsp_intf.h"
 
#include "wx_rs422_master_adu_rsp_intf.h"
#include "wx_msg_res_pool.h"
 #include "wx_frame.h"
UINT32 WX_RS422_MASTER_SendAdu2Driver(WxRs422Master *this, WxModbusAdu *adu)
{
    /* create a event message */
    WxMsg *msg = WX_ApplyEvtMsg(WX_MSG_TYPE_RS422_MASTER_DRIVER_REQ);
    if (msg == NULL) {
        return WX_APPLY_EVT_MSG_ERR;
    }
    /* clear the message */
    WX_CLEAR_OBJ(msg);
    /* init the message */
    msg->sender = this->moduleId;
    msg->receiver = WX_MODULE_DRIVER_RS422_MASTER;
    msg->msgType = WX_MSG_TYPE_RS422_MASTER_DRIVER_REQ;
    /* set the message data */
    WxModbusAdu *modbusAdu = (WxModbusAdu *)msg->msgData;
    for (int i = 0; i < adu->valueLen; i++) {
        modbusAdu->value[i] = adu->value[i];
    }
    modbusAdu->valueLen = adu->valueLen;
    modbusAdu->expectRspLen = adu->expectRspLen;

    /* send msg to driver */
    UINT32 ret = WX_MsgShedule(this->moduleId, msg->receiver, msg);
    if (ret != WX_SUCCESS) {
        WX_FreeEvtMsg(&msg);
    }

    return ret;
}

/* RS422涓昏鍏跺畠璁惧鏈�缁堣繑鍥炵殑鍝嶅簲ADU */
UINT32 WX_RS422_MASTER_ProcRdDataRspAduMsg(WxRs422Master *this, WxRs422MasterRspAduMsg *rspAduMsg)
{
    UINT32 ret = WX_SUCCESS;
    WxModbusAdu *rxAdu = &rspAduMsg->rspAdu;
    if (rxAdu->subMsgType >= WX_RS422_MASTER_MSG_READ_DATA_BUTT) {
        wx_excp_cnt(WX_EXCP_UNEXPECT_MSG_TYPE);
        return WX_RS422_MASTER_PROC_RSP_ADU_SUB_MSG_TYPE_ERR;
    }

    /* 鐢宠鍝嶅簲娑堟伅 */
    WxRs422MasterRdDataRspMsg *rdDataRspMsg = WX_ApplyEvtMsg(WX_MSG_TYPE_RS422_MASTER_RD_DATA_RSP);
    /* 鍒濆鍖栨秷鎭� */
    WX_CLEAR_OBJ((WxMsg *)rdDataRspMsg);
    rdDataRspMsg->subMsgType = rxAdu->subMsgType;
    /* 璋佽鐨勫氨鍙戠粰璋� */
    rdDataRspMsg->receiver = this->rdDataModule[rspAduMsg->rspAdu.subMsgType];
    /* 濡傛灉澶辫触鍛婅瘔瀵规柟澶辫触鐮� */
    if (rspAduMsg->rspAdu.failCode != WX_SUCCESS) {
        rdDataRspMsg->rsp.failCode = rspAduMsg->rspAdu.failCode;
    } else {
    	/* 闇�瑕佽繘琛岃В鐮� */
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
    /* 鐢宠鍝嶅簲娑堟伅 */
    WxRs422MasterWrDatRspMsg *wrDataRspMsg = WX_ApplyEvtMsg(WX_MSG_TYPE_RS422_MASTER_WR_DATA_RSP);
    /* 鍒濆鍖栨秷鎭� */
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

/* 澶勭悊浠嶳S422椹卞姩鏀跺埌鐨勫搷搴擜DU娑堟伅 */
UINT32 WX_RS422_MASTER_ProRspcAduMsg(WxRs422Master *this, WxMsg *msg)
{
    WxRs422MasterRspAduMsg *rspAduMsg = (WxRs422MasterRspAduMsg *)msg;

    /* 鍝嶅簲娑堟伅瀵瑰簲鐨勮姹傛秷鎭� */
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

/* |salve address: 1byte| func code: 1byte| data addr: 2byte | data len : 1byte | data: N | crc: 2byte | */
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
 * E
 * ADU鐨勬牸寮�: |slave address锛�1byte| func code: 1byte | data address: 2byte | data len锛�1byte |
 * 娉ㄦ剰浜嬮」锛氬弬鏁板悎娉曟�х敱璋冪敤鑰呬繚璇�
 **/
UINT32 WX_RS422_MASTER_EncRdDataReqMsg2Adu(WxRs422MasterRdDataReqMsg *msg, WxModbusAdu *txAdu)
{
    /* 鍒ゆ柇娑堟伅绫诲瀷鏄惁鍚堢悊 */
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
    /* 鏈熸湜鍝嶅簲鐨勯暱搴�
     * 鍝嶅簲鐨勬暟鎹牸寮忓涓嬪涓�:
     * |浠庣珯锛�1byte | 鍔熻兘鐮侊細1byte | 鏁版嵁闀垮害锛�1byte | 鏁版嵁锛� N byte | CRC: 2BYTE|
     **/
    txAdu->expectRspLen = 1 + 1 + 1 + handle->dataLen + WX_MODBUS_CRC_LEN;

    return WX_SUCCESS;
}

/* proc write data request message */
UINT32 WX_RS422_MASTER_ProcWrDataReqMsg(WxRs422Master *this, WxMsg *msg)
{
    UINT32 ret;
    /* encode msg 2 adu */
    ret = WX_RS422_MASTER_EncWrDataReqMsg2Adu((WxRs422MasterWrDataReqMsg*)msg, &this->txAdu);
    if (ret != WX_SUCCESS) {
        return ret;
    }
    /* send adu 2 driver */
    ret = WX_RS422_MASTER_SendAdu2Driver(this, &this->txAdu);
    if (ret != WX_SUCCESS) {
        return ret;
    }
    /* record the msg type sender for future rsp msg */
    this->wrDataModule[msg->subMsgType] = msg->sender;
    return WX_SUCCESS;
}

/* 澶勭悊璇绘暟鎹姹� */
UINT32 WX_RS422_MASTER_ProcRdDataReqMsg(WxRs422Master *this, WxMsg *msg)
{
    UINT32 ret;
    /* 缂栫爜璇绘暟鎹姹傚埌MODBUS ADU */
    ret = WX_RS422_MASTER_EncRdDataReqMsg2Adu((WxRs422MasterRdDataReqMsg *)msg, &this->txAdu);
    if (ret != WX_SUCCESS) {
        return ret;
    }
    /* 鍙戦�丄DU鍒癛S422 Master鐨勯┍鍔� */
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

    /* 璁剧疆涓奙odule */
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
            ret = WX_RS422_MASTER_ProRspcAduMsg(this, msg);
            break;
        }
        case WX_MSG_TYPE_RS422_MASTER_RD_DATA_REQ: {
            ret = WX_RS422_MASTER_ProcRdDataReqMsg(this, msg);
            break;
        }
        case WX_MSG_TYPE_RS422_MASTER_WR_DATA_REQ: {
            ret = WX_RS422_MASTER_ProcWrDataReqMsg(this, msg);
            break;
        }
        default: {
        	ret = WX_RS422_MASTER_UNSPT_MSGTYPE;
        	break;
        }
    }
    wx_debug("module return(%u)", ret);
    return ret;
}
