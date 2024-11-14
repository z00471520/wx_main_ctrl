#include "wx_include.h"
#include "wx_rs422_i_master_task.h"
/* 不同设备的异常码含义定义表 */
WxRs422IExcpDesc g_wxRs422IExcpDesc[] = {
    /* |设备地址|异常码|描述 */
    {WX_RS422I_Master_SLAVE_ADDR_NA, 0, "undefine error."},
};

/* 参数合法性由调用者保证 */
inline WxMsgType WX_RS422I_Master_GetRspMsgTypeByMbOpr(WxRs422IModbusOprType oprType)
{
    return g_wxRs422IRspMsgType[oprType];
}

WxFailCode g_wxMbOpr2FailCode[WX_RS422I_Master_MB_OPR_BUTT] = {
    [WX_RS422I_Master_MB_OPR_RD_DATA] = WX_RS422I_Master_RECV_RSP_RD_DATA_ERR,
    [WX_RS422I_Master_MB_OPR_WR_DATA] = WX_RS422I_Master_RECV_RSP_WR_DATA_ERR,
    [WX_RS422I_Master_MB_OPR_RD_FILE] = WX_RS422I_Master_RECV_RSP_RD_FILE_ERR,
    [WX_RS422I_Master_MB_OPR_WR_FILE] = WX_RS422I_Master_RECV_RSP_WR_FILE_ERR,
};

/* Function-code到内部MB操作的转换 */
inline WxRs422IModbusOprType WX_RS422I_Master_ErrFuncCode2OprType(UINT8 funcCode)
{
    switch (funcCode)
    {
        case WX_MODBUS_FUNC_CODE_READ_DATA_ERR: {
            return WX_RS422I_Master_MB_OPR_RD_DATA;
        }
        case WX_MODBUS_FUNC_CODE_WRITE_DATA_ERR: {
            return WX_RS422I_Master_MB_OPR_WR_DATA;
        }
        case WX_MODBUS_FUNC_CODE_READ_FILE_ERR: {
            return WX_RS422I_Master_MB_OPR_RD_FILE;
        }
        case WX_MODBUS_FUNC_CODE_WRITE_FILE_ERR: {
            return WX_RS422I_Master_MB_OPR_WR_FILE;
        }
        default:
            /* 异常计数 */
            return WX_RS422I_Master_MB_OPR_BUTT;
    }
}

/* 异常响应处理 */
WxFailCode WX_RS422I_Master_DecodeAduExcpResponce(WxRs422IMasterTask *this, WxModbusAdu *txAdu, WxModbusAdu *rxAdu, WxRs422IMasterMsg *rxMsg)
{
    if (txAdu->aduLen != WX_MODBUS_ERR_ADU_LEN) {
        return WX_RS422I_Master_RECV_EXCP_RSP_LEN_ERR;
    }

    UINT16 expectCrcValue = WX_Modbus_Crc16(rxAdu->adu, rxAdu->aduLen - 2);
    UINT16 crcValue = WX_MODBUS_CRC_VALUE(rxAdu->adu[WX_MODBUS_ERR_ADU_CRC_HI_IDX] + rxAdu->adu[WX_MODBUS_ERR_ADU_CRC_LO_IDX]);
    if (expectCrcValue != crcValue) {
        return WX_RS422I_Master_RECV_EXCP_RSP_CRC_ERR;
    }
    

    UINT8 funcCode = rxAdu->adu[WX_MODBUS_ERR_ADU_FUNC_CODE_IDX];
    UINT8 slaveAddr = rxAdu->adu[WX_MODBUS_ERR_ADU_SLAVE_ADDR_IDX];
    UINT8 excpCode = rxAdu->adu[WX_MODBUS_ERR_ADU_FUNC_CODE_IDX];
    WxRs422IModbusOprType mbOpr = WX_RS422I_Master_ErrFuncCode2OprType(funcCode);
    if (mbOpr >= WX_RS422I_Master_MB_OPR_BUTT) {
        return WX_RS422I_Master_RECV_UNEXPEC_MB_OPR;
    }
    /* 异常统计 */
    this->slaveExcpCnt[mbOpr][slaveAddr][excpCode]++;
    return g_wxMbOpr2FailCode[mbOpr]; /* 返回异常码 */
}
