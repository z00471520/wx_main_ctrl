
#include "wx_rs422_i_decode_adu_err_response.h"

/* 不同设备的异常码含义定义表 */
WxRs422IExcpDesc g_wxRs422IExcpDesc[] = {
    /* |设备地址|异常码|描述 */
    {WX_RS422I_SLAVE_ADDR_NA, 0, "undefine error."},
};

/* 获取FunctionCode对应的本地操作类型 */
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
    return ;
}
