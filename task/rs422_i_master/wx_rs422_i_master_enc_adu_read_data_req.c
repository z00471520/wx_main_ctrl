#include "wx_rs422_i_master_task.h"
#include "wx_rs422_i_master_enc_adu_read_data_req.h"
/* 不同读操作的编码信息, 方便添加我把所有的都放到这里了 */
WxRs422IReadDataHandle g_wxRs422IReadDataHandles[WX_RS422_I_MSG_READ_DATA_BUTT] = {
                                     /* 提供数据的从机地址, 读数据地址U16， 读数据的长度（U8），读到数据的解码函数 */
    [WX_RS422_I_MSG_READ_DATA_XXX] = {WX_RS422I_Master_SLAVE_XXX, 0, sizeof(WxRs422IDataXXXInfo), WX_RS422I_Master_DecodeDataXxx},
    [WX_RS422_I_MSG_READ_DATA_YYY] = {WX_RS422I_Master_SLAVE_YYY, 0, sizeof(WxRs422IDataYYYInfo), WX_RS422I_Master_DecodeDataYyy},
};

/* 获取指定数据类型的解码函数 */
WxRs422IReadDateDecodeFunc WX_RS422I_Master_GetDecodeFunc(WxRs422IReadDataType dataType)
{
    if (dataType >= WX_RS422_I_MSG_READ_DATA_BUTT) {
        return NULL;
    }
    return g_wxRs422IReadDataHandles[dataType].decFunc;
}

/* |slave address：1byte| func code: 1byte | data address: 2byte | data len：1byte | */
WxFailCode WX_RS422I_Master_EncodeAduReadDataReq(WxRs422IMasterMsg *txMsg, WxRs422IAdu *txAdu)
{
    if (txMsg->msgSubType >= WX_RS422_I_MSG_READ_DATA_BUTT) {
        return WX_RS422I_Master_INVALID_SUB_OPR_TYPE;
    }
    WxRs422IReadDataHandle *encodeInfo = &g_wxRs422IReadDataHandles[txMsg->msgSubType];
    /* the length 0 means that you not define the encode info, wtf! */
    if (encodeInfo->dataLen == 0) {
        return WX_RS422I_Master_READ_REQ_ENCODE_INFO_UNDEF;
    }
    txAdu->valueLen = 0;
    txAdu->value[txAdu->valueLen++] = encodeInfo->slaveDevice; /* slave address */
    txAdu->value[txAdu->valueLen++] = WX_MODBUS_FUNC_CODE_READ_DATA; /* func code */
    txAdu->value[txAdu->valueLen++] = (UINT8)((encodeInfo->dataAddr >> 8) & 0xff); /* data address hi */
    txAdu->value[txAdu->valueLen++] = (UINT8)(encodeInfo->dataAddr & 0xff); /* data address lo */
    txAdu->value[txAdu->valueLen++] = encodeInfo->dataLen; /* data len */
    /* to calc the crc value */
    UINT16 crcValue = WX_Modbus_Crc16(txAdu->adu, txAdu->valueLen);
    txAdu->value[txAdu->valueLen++] = (UINT8)((crcValue >> 8) & 0xff);   
    txAdu->value[txAdu->valueLen++] = (UINT8)(crcValue & 0xff); 
    /* 从站：1byte + 功能码：1byte + 数据长度：1byte + 数据： N byte + CRC: 2BYTE */
    txAdu->expectRspLen = 1 + 1 + 1 + encodeInfo->dataLen + WX_MODBUS_CRC_LEN;

    return WX_SUCCESS;
}