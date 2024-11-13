
#include "wx_typedef.h"
#include "wx_debug.h"
#include "wx_rs422_i_intf.h"
#include "wx_modbus.h"
#include "wx_rs422_i_task.h"
#include "wx_rs422_i_decode_adu_err_response.h"

/* 读数据响应解码 */
WxFailCode WX_RS422I_DecodeAduReadDataResponce(WxRs422ITask *this, WxRs422ITxAdu *txAdu, WxRs422IRxAdu *rxAdu, WxRs422IMsg *rxMsgBuf)
{
    UINT32 expecLen = ; /* 预期的响应长度 */
    if ()
}

/* 写数据响应解码处理 */
WxFailCode WX_RS422I_DecodeAduWriteDataResponce(WxRs422ITask *this, WxRs422ITxAdu *txAdu, WxRs422IRxAdu *rxAdu, WxRs422IMsg *rxMsgBuf)
{
    /* 写数据的响应的写操作的数据预期是一模一样的 */
    if (rxAdu->aduLen != txAdu->aduLen) {
        return WX_RS422I_RECV_WR_DATA_LEN_MISMATCH;
    }

    if (memcmp(rxAdu->adu, txAdu->adu, rxAdu->aduLen) != 0) {
        return WX_RS422I_RECV_WR_DATA_RSP_CTX_MISMATCH;
    }
    return WX_SUCCESS;
}

/* 读文件响应处理 - 暂不支持 */
WxFailCode WX_RS422I_DecodeAduReadFileResponce(WxRs422ITask *this, WxRs422ITxAdu *txAdu, WxRs422IRxAdu *rxAdu, WxRs422IMsg *rxMsgBuf)
{
    return WX_RS422I_RECV_RD_FILE_RSP_UNSPT;
}

/* 写文件响应处理 - 暂不支持 */
WxFailCode WX_RS422I_DecodeAduWriteFileResponce(WxRs422ITask *this, WxRs422ITask *this, WxRs422ITxAdu *txAdu, WxRs422IRxAdu *rxAdu, WxRs422IMsg *rxMsgBuf)
{
    return WX_RS422I_RECV_WR_FILE_RSP_UNSPT;
}

/* 解码接收到ADU转换成消息发送 */
WxFailCode WX_RS422I_DecodeAdu(WxRs422ITask *this, WxRs422IRxAdu *rxAdu, WxRs422IMsg *rxMsg)
{
    WxFailCode rc = WX_SUCCESS;
    WxRs422ITxAdu *txAdu = &this->txAdu;
    /* 长度检查 */
    if (rxAdu->aduLen < WX_MODBUS_ERR_ADU_LEN) {
        if (rxAdu->isTimeout) {
            /* TODO: ADD CNT */
            return WX_RS422I_RECV_TIMEOUT;
        }
        return WX_RS422I_RECV_UNEXPECT_LEN;
    }
    /* check the save addr shoule be same with the tx slave addr */
    UINT8 txSlaceAddr = txAdu->adu[0];
    UINT8 rxSlaveAddr = rxAdu->adu[0];
    if (txSlaceAddr != rxSlaveAddr) {
        return WX_RS422I_RECV_SLAVE_ADDR_MISMATCH;
    }

    /* chech the function-code, the oct 1 is the function-code should samve with the tx or responding err code */
    UINT8 txFuncCode = txAdu->adu[1];
    UINT8 rxFuncCode = txAdu->adu[1];

    if (txFuncCode != rxFuncCode && (txFuncCode + 0x80 != rxFuncCode)) {
        return WX_RS422I_RECV_UNEXPECT_FUNC_CODE;
    }

    /* this is a excpetion responce */
    if (txFuncCode + 0x80 == rxFuncCode) {
        return WX_RS422I_DecodeAduExcpResponce(this, txAdu, rxAdu, rxMsg);
    }

    switch (rxFuncCode) {
        case WX_MODBUS_FUNC_CODE_READ_DATA: {
            return WX_RS422I_DecodeAduReadDataResponce(this, txAdu, rxAdu, rxMsg);
        }
        case WX_MODBUS_FUNC_CODE_WRITE_DATA: {
            return WX_RS422I_DecodeAduWriteDataResponce(this, txAdu, rxAdu, rxMsg);
        }
        case WX_MODBUS_FUNC_CODE_READ_FILE: {
            return WX_RS422I_DecodeAduReadFileResponce(this, txAdu, rxAdu, rxMsg);
        }
        case WX_MODBUS_FUNC_CODE_WRITE_FILE: {
            return WX_RS422I_DecodeAduWriteFileResponce(this, txAdu, rxAdu, rxMsg);
        }
        /* if more please add here */
        default: {
            return WX_RS422I_RECV_UNSPT_FUNC_CODE;
        }
    }
}
