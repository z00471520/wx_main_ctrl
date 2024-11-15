#include "wx_include.h"
#include "wx_rs422_i_master_task.h"
/* 读数据响应解码 */
UINT32 WX_RS422I_Master_DecodeAduReadDataResponce(WxRs422IMasterTask *this, WxModbusAdu *txAdu, WxModbusAdu *rxAdu, WxRs422IMasterMsg *rspMsp)
{
    if (rxAdu->valueLen < WX_MODBUS_ADU_RD_RSP_MIN_LEN) {
        return WX_RS422I_Master_RECV_RSP_RD_LEN_ERR
    }
    /* 检查长度是否符合预期 */
    if (rxAdu->valueLen != txAdu->expectRspLen) {
        return WX_RS422I_Master_RECV_RSP_RD_LEN_MISMATCH;
    }

    if (WX_Modbus_AduCrcCheck(rxAdu) != WX_SUCCESS) {
        return WX_RS422I_Master_RECV_RSP_RD_CRC_ERR;
    }

    /* 获取解码的钩子函数 */
    WxRs422IReadDateDecodeFunc decFunc = WX_RS422I_Master_GetDecodeFunc(rspMsp->msgSubType);
    if (decFunc == NULL) {
        return WX_RS422I_Master_RECV_RSP_RD_GET_DEC_FUNC_FAIL;
    }
    /* 调用解码函数解码，把数据流解码到结构体  */
    UINT32 rc = decFunc(&rxAdu->value[WX_MODBUS_ADU_RD_RSP_DATA_START_IDX], rxAdu->valueLen - WX_MODBUS_ADU_RD_RSP_DATA_START_IDX, (WxRs422IReadDataRsp *)rspMsp->msgBody);
    return rc;
}
