#include "wx_include.h"
#include "wx_msg_common.h"
#include "wx_rs422_slave.h"
#include "wx_rs422_slave_rx_adu_req_intf.h"
#include "wx_rs422_slave_driver_tx_adu_req_intf.h"
#include "wx_deploy_modules.h"

/*
 * This is the configuration of the RS422I-master
 **/
WxRs422SlaveCfg g_wxRs422SlaveCfg = {
    .moduleId = WX_MODULE_RS422_SLAVE,
    .slaveAddr = WX_RS422_SLAVE_ADDR,
    .getRdHandles = WX_RS422Slave_GetRdDataHandles, /* 获取读数据句柄 */
    .getWrHandles = WX_RS422Slave_GetWrDataHandles, /* 获取写数据句柄 */
};

/* 发送ADU */
UINT32 WX_RS422Slave_SendAdu2Driver(WxRs422Slave *this, UINT8 receiver, WxModbusAdu *txAdu)
{
    /* 申请消息 */
    WxRs422SlaverDriverTxAduReq *msg = WX_ApplyEvtMsg(WX_MSG_TYPE_RS422_SLAVE_TX_ADU_REQ);
    if (msg == NULL) {
        return;
    }
    /* 初始化消息头 */
    WX_CLEAR_OBJ((WxMsg *)msg);
    /* 填写消息信息 */
    msg->sender = this->moduleId;
    msg->receiver = receiver;
    
    for (UINT8 i = 0; i < rxAdu->valueLen; i++) {
        msg->rxAdu.value[i] = rxAdu->value[i];
    }
    msg->rxAdu.valueLen = rxAdu->valueLen;
 
    /* 发送消息 */
    UINT32 ret = WX_MsgShedule(this->moduleId, msg->receiver, msg);
    if (ret != WX_SUCCESS) {
        WX_FreeEvtMsg(&msg);
    }
    return ret;
}

/* 发送异常响应 */
UINT32 WX_RS422Slave_SendExcpRsp(WxRs422Slave *this, WxModbusAdu *txAdu, UINT8 excpCode)
{
    if (excpCode >= WX_MODBUS_EXCP_MAX_EXCP_CODE_NUM) {
        return WX_RS322_SLAVE_INVALID_EXCP_CODE;
    }
    this->excpCnt[excpCode]++;
    /* 构造异常响应ADU */
    WX_Modbus_AduGenerateExceptionRsp(txAdu->salveAddr, txAdu->funcCode, excpCode, &this->txAdu);
     /* 发送ADU到驱动 */
    UINT32 ret = WX_RS422Slave_SendAdu2Driver(this, txAdu->receiver, &this->txAdu);
    if (ret != WX_SUCCESS) {
        wx_excp_cnt(WX_EXCP_RS422_SLAVE_SEND_ERR_ADU_FAIL);
        return ret;
    }

    return WX_SUCCESS;
}

/* 处理读数据请求 */
UINT32 WX_RS422Slave_ProcReadDataReq(WxRs422Slave *this, WxModbusAdu *rxAdu, WxModbusAdu *txAdu)
{
    UINT32 ret;
    /* 获取数据地址对应的Handle */
    if (txAdu->dataAddr >= this->rdHandleNum) {
        wx_excp_cnt(WX_EXCP_RS422_SLAVE_DATA_READ_ADDR_OUT_RANGE);
        return WX_RS422Slave_SendExcpRsp(this, txAdu, WX_MODBUS_EXCP_ILLEGAL_DATA_ADDRESS);
    }
    WxRs422SlaveRdDataHandle *handle = &this->rdDataHandles[txAdu->dataAddr];
    if (handle == NULL) {
        wx_excp_cnt(WX_EXCP_RS422_SLAVE_DATA_READ_HANDLE_NULL);
        return WX_RS422Slave_SendExcpRsp(this, txAdu, WX_MODBUS_EXCP_ILLEGAL_DATA_ADDRESS);
    }

    /* 检查handle中的钩子函数是否提供 */
    if (handle->readData == NULL || handle->encAdu == NULL) {
        wx_excp_cnt(WX_EXCP_RS422_SLAVE_DATA_READ_HANDLE_NULL);
        return WX_RS422Slave_SendExcpRsp(this, txAdu, WX_MODBUS_EXCP_RD_ADDR_NOT_DEFINED);
    }
    /* 获取数据 */
    ret = handle->readData(&txAdu->data);
    if (ret != WX_SUCCESS) {
        wx_excp_cnt(WX_EXCP_RS422_SLAVE_DATA_READ_FAIL);
        return WX_RS422Slave_SendExcpRsp(this, txAdu, WX_MODBUS_EXCP_RD_DATA_FAIL);
    }
    /* 数据编码 */
    ret = handle->encAdu(&txAdu->data, txAdu, &this->txAdu);
    if (ret != WX_SUCCESS) {
        wx_excp_cnt(WX_EXCP_RS422_SLAVE_DATA_ENC_FAIL);
        return WX_RS422Slave_SendExcpRsp(this, txAdu, WX_MODBUS_EXCP_ENC_RD_DATA_FAIL);
    }

    /* 这里不返回失败 */
    ret = WX_RS422Slave_SendAdu2Driver(this, txAdu->receiver, &this->txAdu);
    if (ret != WX_SUCCESS) {
        wx_excp_cnt(WX_EXCP_RS422_SLAVE_RD_SEND_ADU_FAIL);
    }

    return ret;
}

/* 从机处理写数据请求 */
UINT32 WX_RS422Slave_ProcWriteDataReq(WxRs422Slave *this, WxModbusAdu *rxAdu, WxModbusAdu *txAdu)
{
     (txAdu->dataAddr >= this->wrHandleNum) {
        wx_excp_cnt(WX_EXCP_RS422_SLAVE_DATA_WRITE_ADDR_OUT_RANGE);
        return WX_RS422Slave_SendExcpRsp(this, txAdu, WX_MODBUS_EXCP_ILLEGAL_DATA_ADDRESS);
    }
    /* 获取写Handle */
    WxRs422SlaveWrDataHandle *handle = this->wrDataHandles + txAdu->dataAddr;
    if (handle == NULL) {
        wx_excp_cnt(WX_EXCP_RS422_SLAVE_DATA_WRITE_HANDLE_NULL);
        return WX_RS422Slave_SendExcpRsp(this, txAdu, WX_MODBUS_EXCP_ILLEGAL_DATA_ADDRESS);
    }
    /* 检查Handles是否为空 */
    if (handle->decAdu == NULL || handle->writeData == NULL) {
        wx_excp_cnt(WX_EXCP_RS422_SLAVE_DATA_WRITE_HANDLE_ITEM_MISS)
        return WX_RS422Slave_SendExcpRsp(this, txAdu, WX_MODBUS_EXCP_WR_HANDLE_UNFINE);
    }
    /* 先对写数据的ADU进行解码 */
    UINT32 ret = handle->decAdu(&this->rxAdu, &txAdu->data);
    if (ret != WX_SUCCESS) {
        wx_excp_cnt(WX_EXCP_RS422_SLAVE_DATA_DEC_FAIL);
        return WX_RS422Slave_SendExcpRsp(this, txAdu, WX_MODBUS_EXCP_WR_DATA_DEC_FAIL);
    }
    /* 数据写到本地 */
    ret = handle->writeData(&txAdu->data);
    if (ret != WX_SUCCESS) {
        wx_excp_cnt(WX_EXCP_RS422_SLAVE_DATA_WRITE_FAIL);
        return WX_RS422Slave_SendExcpRsp(this, txAdu, WX_MODBUS_EXCP_WR_DATA_FAIL);
    }
    /* 把数据原封不动的发送处理 */
    ret = WX_RS422Slave_SendAdu2Driver(this, txAdu->receiver, &this->txAdu);
    if (ret != WX_SUCCESS) {
        wx_excp_cnt(WX_EXCP_RS422_SLAVE_WR_SEND_ADU_FAIL);
    }

    return ret;
}

/* 处理从机接收到ADU的请求 */
UINT32 WX_RS422Slave_ProcRxAduReq(WxRs422Slave *this, WxRs422SlaveRxAduReq *rxAduReq)
{
    WxModbusAdu *rxAdu = &rxAduReq->rxAdu;
    UINT32 ret = WX_SUCCESS;
    /* 检查长度 */
    if (rxAdu->valueLen < WX_MODBUS_ADU_MIN_SIZE) {
        wx_excp_cnt(WX_EXCP_RS422_SLAVE_RECV_ADU_SHORT_LEN);
        return WX_RS422_SLAVE_RECV_ADU_UNEXPECT_LEN;
    }

    /* txAdu 用于存储结果解码 */
    WxModbusAdu *txAdu = &this->txAdu;
    /* 初始化为未知错误 */
    txAdu->excpCode = WX_MODBUS_EXCP_NONE;
    txAdu->receiver = rxAduReq->sender;
    /* 不是本设备的从机地址则不处理 */
    txAdu->slaveAddr = rxAdu->value[WX_MODBUS_ADU_SLAVE_ADDR_IDX];
    txAdu->funcCode = rxAdu->value[WX_MODBUS_ADU_FUNC_CODE_IDX];
    /* 根据请求发送响应 */
    switch (txAdu->funcCode) {
        case WX_MODBUS_FUNC_CODE_READ_DATA:
            return WX_RS422Slave_ProcReadDataReq(this, rxAdu, txAdu);
        case WX_MODBUS_FUNC_CODE_WRITE_DATA:
            return WX_RS422Slave_ProcWriteDataReq(this, rxAdu, txAdu);
        default: {
            return WX_RS422Slave_SendExcpRsp(this, txAdu, WX_MODBUS_EXCP_ILLEGAL_FUNCTION);
        }
    }
}

UINT32 WX_RS422Slave_Construct(VOID *module)
{
    UINT32 ret;
    WxRs422Slave *this = WX_Mem_Alloc(WX_GetModuleName(module), 1, sizeof(WxRs422Slave));
    if (this == NULL) {
        return WX_ERR;
    }
    WX_CLEAR_OBJ(this);
    this->moduleId = WX_GetModuleId(module);
    this->cfg = &g_wxRs422SlaveCfg;
    this->rdDataHandles = cfg->getRdHandles(this, &this->rdHandleNum);
    this->wrDataHandles = cfg->getWrHandles(this, &this->wrHandleNum);
    this->slaveAddr = cfg->slaveAddr;
    /* 设置上Module */
    WX_SetModuleInfo(module, this);
    return WX_SUCCESS;
}


UINT32 WX_RS422Slave_Destruct(VOID *module)
{
    WxRs422Slave *this = WX_GetModuleInfo(module);
    if (this == NULL) {
        return WX_SUCCESS;
    }
    WX_Mem_Free(this);
    WX_SetModuleInfo(module, NULL);
    return WX_SUCCESS;
}

UINT32 WX_RS422Slave_Entry(VOID *module, WxMsg *evtMsg)
{
    WxRs422Slave *this = WX_GetModuleInfo(module);
    switch (evtMsg->msgType) {
        case WX_MSG_TYPE_RS422_SLAVE_RX_ADU_REQ:
            return WX_RS422Slave_ProcRxAduReq(this, (WxRs422SlaveRxAduReq *)evtMsg);
        default:
            return WX_RS422_SLAVE_UNSPT_MSG_TYPE;
    }
}