#include "wx_rs422_i_task.h"
/* 不同读操作的编码信息 */
WxRs422IReadDataEncodeInfo g_wxRs422IReadDataEncodeInfos[WX_RS422_I_MSG_READ_DATA_BUTT] = {
    [WX_RS422_I_MSG_READ_DATA_XXX] = {WX_RS422I_SLAVE_XXX, 0, sizeof(WxRs422IDataXXXInfo)},
    [WX_RS422_I_MSG_READ_DATA_YYY] = {WX_RS422I_SLAVE_YYY, 0, sizeof(WxRs422IDataYYYInfo)},
};

/* |slave address：1byte| func code: 1byte | data address: 2byte | data len：1byte | */
WxFailCode WX_RS422I_EncodeAduReadDataReq(WxRs422IMsg *txMsg, WxRs422IAdu *txAdu)
{
    if (txMsg->msgSubType >= WX_RS422_I_MSG_READ_DATA_BUTT) {
        return WX_RS422I_INVALID_SUB_OPR_TYPE;
    }
    WxRs422IReadDataEncodeInfo *encodeInfo = &g_wxRs422IReadDataEncodeInfos[txMsg->msgSubType];
    /* the length 0 means that you not define the encode info, wtf! */
    if (encodeInfo->dataLen == 0) {
        return WX_RS422I_READ_REQ_ENCODE_INFO_UNDEF;
    }
    txAdu->aduLen = 0;
    txAdu->adu[txAdu->aduLen++] = encodeInfo->slaveDevice; /* slave address */
    txAdu->adu[txAdu->aduLen++] = WX_MODBUS_FUNC_CODE_READ_DATA; /* func code */
    txAdu->adu[txAdu->aduLen++] = (UINT8)((encodeInfo->dataAddr >> 8) & 0xff); /* data address hi */
    txAdu->adu[txAdu->aduLen++] = (UINT8)(encodeInfo->dataAddr & 0xff); /* data address lo */
    txAdu->adu[txAdu->aduLen++] = encodeInfo->dataLen; /* data len */
    /* to calc the crc value */
    UINT16 crcValue = WX_Modbus_Crc16(txAdu->adu, txAdu->aduLen);
    txAdu->adu[txAdu->aduLen++] = (UINT8)((crcValue >> 8) & 0xff);   
    txAdu->adu[txAdu->aduLen++] = (UINT8)(crcValue & 0xff); 
    /* 从站：1byte + 功能码：1byte + 数据长度：1byte + 数据： N byte + CRC: 2BYTE */
    txAdu->expectRspLen = 1 + 1 + 1 + encodeInfo->dataLen + WX_MODBUS_CRC_LEN;

    return WX_SUCCESS;
}