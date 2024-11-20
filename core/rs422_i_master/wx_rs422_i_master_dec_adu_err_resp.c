#include "wx_include.h"
#include "wx_rs422_i_master_task.h"
/* 不同设备的异常码含义定义表 */
WxRs422IExcpDesc g_wxRs422IExcpDesc[] = {
    /* |设备地址|异常码|描述 */
    {WX_RS422I_MASTER_SLAVE_ADDR_NA, 0, "undefine error."},
};

/* 参数合法性由调用者保证 */
inline WxMsgType WX_RS422I_MASTER_GetRspMsgTypeByMbOpr(WxRs422IModbusOprType oprType)
{
    return g_wxRs422IRspMsgType[oprType];
}

UINT32 g_wxMbOpr2FailCode[WX_RS422I_MASTER_MB_OPR_BUTT] = {
    [WX_RS422I_MASTER_MB_OPR_RD_DATA] = WX_RS422I_MASTER_RECV_RSP_RD_DATA_ERR,
    [WX_RS422I_MASTER_MB_OPR_WR_DATA] = WX_RS422I_MASTER_RECV_RSP_WR_DATA_ERR,
    [WX_RS422I_MASTER_MB_OPR_RD_FILE] = WX_RS422I_MASTER_RECV_RSP_RD_FILE_ERR,
    [WX_RS422I_MASTER_MB_OPR_WR_FILE] = WX_RS422I_MASTER_RECV_RSP_WR_FILE_ERR,
};

/* Function-code到内部MB操作的转换 */
inline WxRs422IModbusOprType WX_RS422I_MASTER_ErrFuncCode2OprType(UINT8 funcCode)
{
    switch (funcCode)
    {
        case WX_MODBUS_FUNC_CODE_READ_DATA_ERR: {
            return WX_RS422I_MASTER_MB_OPR_RD_DATA;
        }
        case WX_MODBUS_FUNC_CODE_WRITE_DATA_ERR: {
            return WX_RS422I_MASTER_MB_OPR_WR_DATA;
        }
        case WX_MODBUS_FUNC_CODE_READ_FILE_ERR: {
            return WX_RS422I_MASTER_MB_OPR_RD_FILE;
        }
        case WX_MODBUS_FUNC_CODE_WRITE_FILE_ERR: {
            return WX_RS422I_MASTER_MB_OPR_WR_FILE;
        }
        default:
            /* 异常计数 */
            return WX_RS422I_MASTER_MB_OPR_BUTT;
    }
}

/* 异常响应处理 */
UINT32 WX_RS422I_MASTER_DecodeAduExcpResponce(WxRs422IMasterTask *this, WxModbusAdu *txAdu, WxModbusAdu *rxAdu, WxRs422IMasterMsg *rxMsg)
{
    if (txAdu->valueLen != WX_MODBUS_ADU_ERR_RSP_LEN) {
        return WX_RS422I_MASTER_RECV_EXCP_RSP_LEN_ERR;
    }

    if (WX_Modbus_AduCrcCheck(rxAdu) != WX_SUCCESS) {
        return WX_RS422I_MASTER_RECV_EXCP_RSP_CRC_ERR;
    }
    
    UINT8 funcCode = rxAdu->value[WX_MODBUS_ADU_ERR_RSP_FUNC_CODE_IDX];
    UINT8 slaveAddr = rxAdu->value[WX_MODBUS_ADU_ERR_RSP_SLAVE_ADDR_IDX];
    UINT8 excpCode = rxAdu->value[WX_MODBUS_ADU_ERR_RSP_FUNC_CODE_IDX];
    WxRs422IModbusOprType mbOpr = WX_RS422I_MASTER_ErrFuncCode2OprType(funcCode);
    if (mbOpr >= WX_RS422I_MASTER_MB_OPR_BUTT) {
        return WX_RS422I_MASTER_RECV_UNEXPEC_MB_OPR;
    }
    /* 异常统计 */
    this->slaveExcpCnt[mbOpr][slaveAddr][excpCode]++;
    return g_wxMbOpr2FailCode[mbOpr]; /* 返回异常码 */
}
