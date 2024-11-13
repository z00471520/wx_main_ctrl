
#include "wx_typedef.h"
#include "wx_debug.h"
#include "wx_rs422_i_intf.h"
#include "wx_modbus.h"
#include "wx_rs422_i_task.h"

/* 异常描述 */
typedef struct
{
    /* data */
}WxRs422IExcpDesc;



inline WxRs422IModbusOprType WX_RS422I_ErrFuncCode2OprType(UINT8 funcCode)
{
    switch (funcCode)
    {
        case WX_MODBUS_FUNC_CODE_READ_DATA_ERR: {
            return WX_RS422I_MB_OPR_RD_DATA;
        }
        case WX_MODBUS_FUNC_CODE_WRITE_DATA_ERR: {
            return WX_RS422I_MB_OPR_WR_DATA;
        }
        case WX_MODBUS_FUNC_CODE_READ_FILE_ERR: {
            return WX_RS422I_MB_OPR_RD_FILE;
        }
        case WX_MODBUS_FUNC_CODE_WRITE_FILE_ERR: {
            return WX_RS422I_MB_OPR_WR_FILE;
        }
        default:
            /* 异常计数 */
            return WX_RS422I_MB_OPR_BUTT;
    }
}


/* 异常响应处理 */
WxFailCode WX_RS422I_DecodeAduExcpResponce(WxRs422ITask *this, WxRs422ITxAdu *txAdu, WxRs422IRxAdu *rxAdu, WxRs422IMsg *rxMsgBuf)
{
    if (txAdu->aduLen != WX_MODBUS_ERR_ADU_LEN) {
        return WX_RS422I_RECV_EXCP_RSP_LEN_ERR;
    }

    UINT16 expectCrcValue = WX_Modbus_Crc16(rxAdu->adu, rxAdu->aduLen);
    UINT16 crcValue = ((UINT16)rxAdu->adu[WX_MODBUS_ERR_ADU_CRC_HI_IDX] << 8) + (UINT16)rxAdu->adu[WX_MODBUS_ERR_ADU_CRC_LO_IDX];
    if (expectCrcValue != crcValue) {
        return WX_RS422I_RECV_EXCP_RSP_CRC_ERR;
    }

    UINT8 funcCode = rxAdu->adu[WX_MODBUS_ERR_ADU_FUNC_CODE_IDX];
    UINT8 slaveAddr = rxAdu->adu[WX_MODBUS_ERR_ADU_SLAVE_ADDR_IDX];
    UINT8 excpCode = rxAdu->adu[WX_MODBUS_ERR_ADU_FUNC_CODE_IDX];
    

    WxRs422IModbusOprType mbOpr = WX_RS422I_ErrFuncCode2OprType(funcCode); 
    if (mbOpr >= WX_RS422I_MB_OPR_BUTT) {
        return WX_RS422I_RECV_UNEXPEC_MB_OPR;
    }

    this->slaveExcpCnt[mbOpr][slaveAddr][excpCode]++;
}

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

/* 读文件响应处理 */
WxFailCode WX_RS422I_DecodeAduReadFileResponce(WxRs422ITask *this, WxRs422ITxAdu *txAdu, WxRs422IRxAdu *rxAdu, WxRs422IMsg *rxMsgBuf)
{
    return WX_RS422I_RECV_RD_FILE_RSP_UNSPT;
}

/* 写文件响应处理 */
WxFailCode WX_RS422I_DecodeAduWriteFileResponce(WxRs422ITask *this, WxRs422ITask *this, WxRs422ITxAdu *txAdu, WxRs422IRxAdu *rxAdu, WxRs422IMsg *rxMsgBuf)
{
    return WX_RS422I_RECV_WR_FILE_RSP_UNSPT;
}

/* 解码接收到ADU转换成消息发送 */
WxFailCode WX_RS422I_DecodeAdu(WxRs422ITask *this, WxRs422IRxAdu *rxAdu, WxRs422IMsg *rxMsgBuf)
{
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
        rc = WX_RS422I_DecodeAduExcpResponce(this, txAdu, rxAdu, rxMsgBuf);
        if (rc != WX_SUCCESS) {
            return rc;
        }

        return;
    }

    switch (rxFuncCode) {
        case WX_MODBUS_FUNC_CODE_READ_DATA: {
            return WX_RS422I_DecodeAduReadDataResponce(this, txAdu, rxAdu, rxMsgBuf);
        }
        case WX_MODBUS_FUNC_CODE_WRITE_DATA: {
            return WX_RS422I_DecodeAduWriteDataResponce(this, txAdu, rxAdu, rxMsgBuf);
        }
        case WX_MODBUS_FUNC_CODE_READ_FILE: {
            return WX_RS422I_DecodeAduReadFileResponce(this, txAdu, rxAdu, rxMsgBuf);
        }
        case WX_MODBUS_FUNC_CODE_WRITE_FILE: {
            return WX_RS422I_DecodeAduWriteFileResponce(this, txAdu, rxAdu, rxMsgBuf);
        }
        /* if more please add here */
        default: {
            return WX_RS422I_RECV_UNSPT_FUNC_CODE;
        }
    }
}
