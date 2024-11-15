#include "wx_rs422_i_master_task.h"
WxFailCode WxRs422I_EncodeDataMMM(UINT8 buf[], UINT8 bufSize, VOID *data)
{
    return WX_ERR;
}

WxFailCode WxRs422I_EncodeDataMMM(UINT8 buf[], UINT8 bufSize, VOID *data)
{
    return WX_ERR;
}


/* 不同读操作的编码信息 */
WxRs422IWriteDataEncodeInfo g_wxRs422IReadDataHandles[WX_RS422_I_MSG_READ_DATA_BUTT] = {
    /* sub msg type                     slave address       encode func */
    [WX_RS422_I_MSG_WRITE_DATA_MMM] = {WX_RS422I_Master_SLAVE_XXX, WX_RS422I_Master_EncodeDataMMM},
    [WX_RS422_I_MSG_WRITE_DATA_NNN] = {WX_RS422I_Master_SLAVE_YYY, WX_RS422I_Master_EncodeDataNNN},
};

/* |salve address: 1byte| func code: 1byte| data addr: 2byte | data len：1byte | data: N | */
WxFailCode WX_RS422I_Master_EncodeAduWriteDataReq(WxRs422IMasterMsg *txMsg, WxRs422IAdu *txAdu)
{
    if (txMsg->msgSubType >= WX_RS422_I_MSG_WRITE_DATA_BUTT) {
        return WX_RS422I_Master_INVALID_SUB_OPR_TYPE;
    }
    WxRs422IWriteDataEncodeInfo *encodeInfo = &g_wxRs422IReadDataHandles[txMsg->msgSubType];
    if (encodeInfo->dataStrucEncodeFunc == NULL) {
        return WX_RS422I_Master_WR_REQ_ENCODE_FUNC_UNDEF;
    }
    txAdu->value[0] = encodeInfo->slaveDevice; /* slave address */
    txAdu->value[1] = WX_MODBUS_FUNC_CODE_WRITE_DATA; /* func code */
    txAdu->value[2] = (UINT8)((encodeInfo->dataAddr >> 8) & 0xff); /* data address hi */
    txAdu->value[3] = (UINT8)(encodeInfo->dataAddr & 0xff); /* data address lo */
    txAdu->valueLen = 4;
    txAdu->value[4] = 0; /* 4- it is the data len we init */

    /* the length size can be used to encode */
    UINT16 lelfLen = WX_RS422_ADU_MSX_SIZE - txAdu->valueLen - WX_MODBUS_CRC_LEN; /* the crc and adu len */
    WxFailCode rc = encodeInfo->dataStrucEncodeFunc(txAdu->value[txAdu->valueLen], lelfLen, &txMsg->msgBody[0]);
    if (rc != WX_SUCCESS) {
        return rc;
    }
    txAdu->valueLen += txAdu->value[4] + 1; /* 编码adu[4]存放的是长度，其本身占用1字节*/
    /* to calc the crc value */
    UINT16 crcValue = WX_Modbus_Crc16(txAdu->value, txAdu->valueLen);
    txAdu->value[txAdu->valueLen++] = (UINT8)((crcValue >> 8) & 0xff);   
    txAdu->value[txAdu->valueLen++] = (UINT8)(crcValue & 0xff); 
    txAdu->expectRspLen = txAdu->valueLen; /* 写多少字节，响应就应该是多少字节 */
    return WX_SUCCESS;
}
