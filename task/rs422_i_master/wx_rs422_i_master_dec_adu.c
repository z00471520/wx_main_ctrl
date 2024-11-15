
#include "wx_include.h"
#include "wx_rs422_i_master_task.h"

/* 写数据响应解码处理 */
WxFailCode WX_RS422I_Master_DecodeAduWriteDataResponce(WxRs422IMasterTask *this, WxModbusAdu *txAdu, WxModbusAdu *rxAdu, WxRs422IMasterMsg *rxMsgBuf)
{
    /* 写数据的响应的写操作的数据预期是一模一样的 */
    if (rxAdu->valueLen != txAdu->valueLen) {
        return WX_RS422I_Master_RECV_WR_DATA_LEN_MISMATCH;
    }

    if (memcmp(rxAdu->adu, txAdu->adu, rxAdu->valueLen) != 0) {
        return WX_RS422I_Master_RECV_WR_DATA_RSP_CTX_MISMATCH;
    }
    return WX_SUCCESS;
}

/* 读文件响应处理 - 暂不支持 */
WxFailCode WX_RS422I_Master_DecodeAduReadFileResponce(WxRs422IMasterTask *this, WxModbusAdu *txAdu, WxModbusAdu *rxAdu, WxRs422IMasterMsg *rxMsgBuf)
{
    return WX_RS422I_Master_RECV_RD_FILE_RSP_UNSPT;
}

/* 写文件响应处理 - 暂不支持 */
WxFailCode WX_RS422I_Master_DecodeAduWriteFileResponce(WxRs422IMasterTask *this, WxRs422IMasterTask *this, WxModbusAdu *txAdu, WxModbusAdu *rxAdu, WxRs422IMasterMsg *rxMsgBuf)
{
    return WX_RS422I_Master_RECV_WR_FILE_RSP_UNSPT;
}

/* 解码接收到ADU转换成消息发送 */
WxFailCode WX_RS422I_Master_DecodeAdu(WxRs422IMasterTask *this, WxModbusAdu *rxAdu, WxRs422IMasterMsg *rxMsg)
{
    WxFailCode rc = WX_SUCCESS;
    WxModbusAdu *txAdu = &this->txAdu;
    /* 长度检查 */
    if (rxAdu->valueLen < WX_MODBUS_ADU_ERR_RSP_LEN) {
        if (rxAdu->isTimeout) {
            /* TODO: ADD CNT */
            return WX_RS422I_Master_RECV_TIMEOUT;
        }
        return WX_RS422I_Master_RECV_UNEXPECT_LEN;
    }
    /* check the save addr shoule be same with the tx slave addr */
    UINT8 txSlaceAddr = txAdu->value[0];
    UINT8 rxSlaveAddr = rxAdu->value[0];
    if (txSlaceAddr != rxSlaveAddr) {
        return WX_RS422I_Master_RECV_SLAVE_ADDR_MISMATCH;
    }

    /* chech the function-code, the oct 1 is the function-code should samve with the tx or responding err code */
    UINT8 txFuncCode = txAdu->value[1];
    UINT8 rxFuncCode = txAdu->value[1];

    if (txFuncCode != rxFuncCode && (txFuncCode + 0x80 != rxFuncCode)) {
        return WX_RS422I_Master_RECV_UNEXPECT_FUNC_CODE;
    }

    /* this is a excpetion responce */
    if (txFuncCode + 0x80 == rxFuncCode) {
        return WX_RS422I_Master_DecodeAduExcpResponce(this, txAdu, rxAdu, rxMsg);
    }

    switch (rxFuncCode) {
        case WX_MODBUS_FUNC_CODE_READ_DATA: {
            return WX_RS422I_Master_DecodeAduReadDataResponce(this, txAdu, rxAdu, rxMsg);
        }
        case WX_MODBUS_FUNC_CODE_WRITE_DATA: {
            return WX_RS422I_Master_DecodeAduWriteDataResponce(this, txAdu, rxAdu, rxMsg);
        }
        case WX_MODBUS_FUNC_CODE_READ_FILE: {
            return WX_RS422I_Master_DecodeAduReadFileResponce(this, txAdu, rxAdu, rxMsg);
        }
        case WX_MODBUS_FUNC_CODE_WRITE_FILE: {
            return WX_RS422I_Master_DecodeAduWriteFileResponce(this, txAdu, rxAdu, rxMsg);
        }
        /* if more please add here */
        default: {
            return WX_RS422I_Master_RECV_UNSPT_FUNC_CODE;
        }
    }
}
