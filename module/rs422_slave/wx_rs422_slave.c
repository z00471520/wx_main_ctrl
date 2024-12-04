#include "wx_include.h"
#include "wx_msg_common.h"
#include "wx_rs422_slave_rx_adu_req_intf.h"
#include "wx_rs422_slave_driver_tx_adu_req_intf.h"
#include "wx_rs422_slave.h"
#include "wx_rs422_slave_addr_intf.h"
 
 #include "wx_frame.h"
#include "../../frame/wx_msg_res_pool.h"
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
UINT32 WX_RS422Slave_SendRspAdu2Driver(WxRs422Slave *this, WxModbusAdu *rspAdu)
{
    /* 申请消息 */
    WxRs422SlaverDriverTxAduReq *msg = WX_ApplyEvtMsg(WX_MSG_TYPE_RS422_SLAVE_TX_ADU_REQ);
    if (msg == NULL) {
        return WX_APPLY_EVT_MSG_ERR;
    }
    /* 填写消息信息 */
    msg->sender = this->moduleId;
    msg->receiver = WX_MODULE_RS422_SLAVE_DRIVER;
    
    for (UINT8 i = 0; i < rspAdu->valueLen; i++) {
        msg->txAdu.value[i] = rspAdu->value[i];
    }
    msg->txAdu.valueLen = rspAdu->valueLen;
 
    /* 发送消息 */
    UINT32 ret = WX_MsgShedule(this->moduleId, msg->receiver, msg);
    if (ret != WX_SUCCESS) {
        WX_FreeEvtMsg((WxMsg **)&msg);
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
    WX_Modbus_AduGenerateExceptionRsp(txAdu->slaveAddr, txAdu->funcCode, excpCode, &this->txAdu);
     /* 发送ADU到驱动 */
    UINT32 ret = WX_RS422Slave_SendRspAdu2Driver(this, &this->txAdu);
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
    WxRs422SlaveData data;
    WX_CLEAR_OBJ(&data);
    ret = handle->readData(this, &data);
    if (ret != WX_SUCCESS) {
        wx_excp_cnt(WX_EXCP_RS422_SLAVE_DATA_READ_FAIL);
        return WX_RS422Slave_SendExcpRsp(this, txAdu, WX_MODBUS_EXCP_RD_DATA_FAIL);
    }
    /* 数据编码 */
    ret = handle->encAdu(this, &data, txAdu);
    if (ret != WX_SUCCESS) {
        wx_excp_cnt(WX_EXCP_RS422_SLAVE_DATA_ENC_FAIL);
        return WX_RS422Slave_SendExcpRsp(this, txAdu, WX_MODBUS_EXCP_ENC_RD_DATA_FAIL);
    }

    /* 这里不返回失败 */
    ret = WX_RS422Slave_SendRspAdu2Driver(this, &this->txAdu);
    if (ret != WX_SUCCESS) {
        wx_excp_cnt(WX_EXCP_RS422_SLAVE_RD_SEND_ADU_FAIL);
    }

    return ret;
}

/* 从机处理驱动接收的发送过来的写数据请求ADU */
UINT32 WX_RS422Slave_ProcWriteDataReq(WxRs422Slave *this, WxModbusAdu *rxAdu, WxModbusAdu *txAdu)
{
    if (txAdu->dataAddr >= this->wrHandleNum) {
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
    WxRs422SlaveData data;
    WX_CLEAR_OBJ(&data);
    /* 先对写数据的ADU进行解码 */
    UINT32 ret = handle->decAdu(rxAdu, &data);
    if (ret != WX_SUCCESS) {
        wx_excp_cnt(WX_EXCP_RS422_SLAVE_DATA_DEC_FAIL);
        return WX_RS422Slave_SendExcpRsp(this, txAdu, WX_MODBUS_EXCP_WR_DATA_DEC_FAIL);
    }
    /* 数据写到本地 */
    ret = handle->writeData(&data);
    if (ret != WX_SUCCESS) {
        wx_excp_cnt(WX_EXCP_RS422_SLAVE_DATA_WRITE_FAIL);
        return WX_RS422Slave_SendExcpRsp(this, txAdu, WX_MODBUS_EXCP_WR_DATA_FAIL);
    }
    /* 把数据原封不动的发送处理 */
    ret = WX_RS422Slave_SendRspAdu2Driver(this, &this->txAdu);
    if (ret != WX_SUCCESS) {
        wx_excp_cnt(WX_EXCP_RS422_SLAVE_WR_SEND_ADU_FAIL);
    }

    return ret;
}

/* 处理主机给本从机发送的ADU请求 */
UINT32 WX_RS422Slave_ProcReqAdu(WxRs422Slave *this, WxRs422SlaveRxReqAdu *reqAdu)
{
    WxModbusAdu *rxAdu = &reqAdu->rxAdu;
    /* 检查长度 */
    if (rxAdu->valueLen < WX_MODBUS_ADU_MIN_SIZE) {
        wx_excp_cnt(WX_EXCP_RS422_SLAVE_RECV_ADU_SHORT_LEN);
        return WX_RS422_SLAVE_RECV_ADU_UNEXPECT_LEN;
    }

    /* txAdu 用于存储结果解码 */
    WxModbusAdu *txAdu = &this->txAdu;
    /* 初始化为未知错误 */
    txAdu->excpCode = WX_MODBUS_EXCP_NONE;
    txAdu->sender = reqAdu->sender;
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
    WxRs422Slave *this = WX_Mem_Alloc(WX_GetModuleName(module), 1, sizeof(WxRs422Slave));
    if (this == NULL) {
        boot_debug("Error Exit: memory allocation failed");
        return WX_MEM_ALLOC_FAIL;
    }
    WX_CLEAR_OBJ(this);
    this->moduleId = WX_GetModuleId(module);
    WxRs422SlaveCfg *cfg = &g_wxRs422SlaveCfg;
    this->cfg = cfg;
    this->rdDataHandles = cfg->getRdHandles(this->moduleId, &this->rdHandleNum);
    this->wrDataHandles = cfg->getWrHandles(this->moduleId, &this->wrHandleNum);
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
            return WX_RS422Slave_ProcReqAdu(this, (WxRs422SlaveRxReqAdu *)evtMsg);
        default:
            return WX_RS422_SLAVE_UNSPT_MSG_TYPE;
    }
}
