#include "wx_debug.h"
#include "wx_rs422_i_task.h"
#include "wx_modbus.h"
#include "wx_rs422_i_master_intf.h"
#include "wx_rs422_i_decode_adu_read_data_rsp.h"
#include "wx_rs422_i_encode_adu_read_data_req.h"
/* 读数据响应解码 */
WxFailCode WX_RS422I_DecodeAduReadDataResponce(WxRs422ITask *this, WxRs422ITxAdu *txAdu, WxRs422IRxAdu *rxAdu, WxRs422IMsg *rspMsp)
{
    if (rxAdu->aduLen < WX_MODBUS_RD_RSP_MIN_LEN) {
        return WX_RS422I_RECV_RSP_RD_LEN_ERR
    }
    /* 检查长度是否符合预期 */
    if (rxAdu->aduLen != txAdu->expectRspLen) {
        return WX_RS422I_RECV_RSP_RD_LEN_MISMATCH;
    }

    UINT16 crcValue = WX_MODBUS_CRC_VALUE(txAdu->adu[rxAdu->aduLen - 2], txAdu->adu[rxAdu->aduLen - 1]);
    UINT16 expectCrcValue = WX_Modbus_Crc16(rxAdu->adu, rxAdu->aduLen - 2);
    if (expectCrcValue != crcValue) {
        return WX_RS422I_RECV_RSP_RD_CRC_ERR;
    }

    /* 获取解码的钩子函数 */
    WxRs422IReadDateDecodeFunc decFunc = WX_RS422I_GetDecodeFunc(rspMsp->msgSubType);
    if (decFunc == NULL) {
        return WX_RS422I_RECV_RSP_RD_GET_DEC_FUNC_FAIL;
    }
    /* 调用解码函数解码，把数据流解码到结构体  */
    WxFailCode rc = decFunc(&rxAdu->adu[WX_MODBUS_RD_RSP_DATA], rxAdu->aduLen - WX_MODBUS_RD_RSP_DATA, (WxRs422IReadDataRsp *)rspMsp->msgBody);
    return rc;
}
