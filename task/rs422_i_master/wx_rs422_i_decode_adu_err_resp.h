#ifndef WX_RS422_I_DECODE_ADU_ERR_RESPONSE_H
#define WX_RS422_I_DECODE_ADU_ERR_RESPONSE_H
#include "wx_modbus.h"
#include "wx_rs422_i_task.h"
/* 设备异常描述 */
typedef struct
{
    WxRs422ISlaveAddrDef deviceAddr;   /* 设备地址 */
    UINT8 funcCode;     /* 设备的异常码 */
    CHAR *desc;         /* 设备的异常码的描述 */
} WxRs422IExcpDesc;

/* Function-code到内部MB操作的转换 */
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
WxFailCode WX_RS422I_DecodeAduExcpResponce(WxRs422ITask *this, WxRs422ITxAdu *txAdu, WxRs422IRxAdu *rxAdu, WxRs422IMsg *rxMsg);
#endif
