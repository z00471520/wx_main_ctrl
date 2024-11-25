#include "wx_rs422_master_wr_data_req_intf.h"
UINT32 WxRs422I_EncodeDataMMM(UINT8 buf[], UINT8 bufSize, VOID *data)
{
    return WX_ERR;
}

UINT32 WxRs422I_EncodeDataMMM(UINT8 buf[], UINT8 bufSize, VOID *data)
{
    return WX_ERR;
}


/* 不同读操作的编码信息 */
WxRs422MasterWrDataEnchHandle g_wxRs422MasterWrDataEncHandles[WX_RS422_MASTER_MSG_READ_DATA_BUTT] = {
    /* sub msg type                     slave address       data addr         data encode func */
    [WX_RS422_MASTER_MSG_WRITE_MMM] = {WX_RS422_MASTER_SLAVE_XXX, 0, WX_RS422_MASTER_EncodeDataMMM},
    [WX_RS422_MASTER_MSG_WRITE_NNN] = {WX_RS422_MASTER_SLAVE_YYY, 0, WX_RS422_MASTER_EncodeDataNNN},
};

/* |salve address: 1byte| func code: 1byte| data addr: 2byte | data len：1byte | data: N | */
UINT32 WX_RS422_MASTER_EncodeWrDataMsg2Adu(WxMsg *msg, WxModbusAdu *adu)
{
    if (msg->msgSubType >= WX_RS422_MASTER_MSG_WRITE_BUTT) {
        return WX_RS422_MASTER_INVALID_SUB_OPR_TYPE;
    }
    WxRs422MasterWrDataEnchHandle *handle = &g_wxRs422MasterWrDataEncHandles[txMsg->msgSubType];
    if (handle->encStruct == NULL) {
        return WX_RS422_MASTER_WR_REQ_ENCODE_FUNC_UNDEF;
    }
    UINT16 dataAddr = (UINT16)handle->dataAddr;
    adu->value[0] = handle->slaveDevice;              /* slave address */
    adu->value[1] = WX_MODBUS_FUNC_CODE_WRITE_DATA;    /* func code */
    adu->value[2] = (UINT8)((dataAddr >> 8) & 0xff);   /* data address hi */
    adu->value[3] = (UINT8)(dataAddr & 0xff);         /* data address lo */
    adu->valueLen = 4;

    /* the length size can be used to encode */
    UINT16 lelfLen = WX_MODBUS_ADU_MAX_SIZE - adu->valueLen - WX_MODBUS_CRC_LEN; /* the crc and adu len */
    /* 4 is the data length */
    adu->value[4] = handle->encStruct(&adu->value[4], lelfLen, &msg->msgData[0]);
    if (adu->value[4] == 0) {
        return WX_RS422_MASTER_WR_REQ_ENCODE_FAIL;
    }
    adu->valueLen += adu->value[4] + 1; /* 编码adu[4]存放的是长度，其本身占用1字节*/
    /* to calc the crc value */
    UINT16 crcValue = WX_Modbus_Crc16(adu->value, adu->valueLen);
    adu->value[adu->valueLen++] = (UINT8)((crcValue >> 8) & 0xff);   
    adu->value[adu->valueLen++] = (UINT8)(crcValue & 0xff); 
    adu->expectRspLen = adu->valueLen; /* 写多少字节，响应就应该是多少字节 */
    return WX_SUCCESS;
}
