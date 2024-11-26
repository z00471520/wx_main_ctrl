#include "wx_rs422_i_master_task.h"
#include "wx_rs422_i_master_enc_adu_read_data_req.h"

UINT32 WX_RS422_MASTER_DecRdDataX(UINT8 buff[], UINT8 buffSize, WxRs422MasterReadData *rspStruct)
{
    return WX_ERR;
}


UINT32 WX_RS422_MASTER_DecRdDataY(UINT8 buff[], UINT8 buffSize, WxRs422MasterReadData *rspStruct)
{
    return WX_ERR;
}

/* 不同读操作的编码信息, 方便添加我把所有的都放到这里了 */
WxRs422MasterRdDataHandle g_wxRs422MasterReadDataHandles[WX_RS422_MASTER_MSG_READ_DATA_BUTT] = {
                                     /* 提供数据的从机地址, 读数据地址U16， 读数据的长度（U8），读到数据的解码函数 */
    [WX_RS422_MASTER_MSG_READ_DATA_XXX] = {WX_RS422_MASTER_SLAVE_XXX, 0, sizeof(WxRs422MasterRdDataX), WX_RS422_MASTER_DecRdDataX},
    [WX_RS422_MASTER_MSG_READ_DATA_YYY] = {WX_RS422_MASTER_SLAVE_YYY, 0, sizeof(WxRs422MasterRdDataY), WX_RS422_MASTER_DecRdDataY},
};

/* 获取指定数据类型的解码函数 */
WxRs422MasterRdDateDecFunc WX_RS422_MASTER_GetDecodeFunc(WxRs422MasterRdDataType dataType)
{
    if (dataType >= WX_RS422_MASTER_MSG_READ_DATA_BUTT) {
        return NULL;
    }
    return g_wxRs422MasterReadDataHandles[dataType].decFunc;
}

UINT32 WX_RS422_MASTER_DecRdDataAdu(WxModbusAdu *rxAdu, WxRs422MasterReadData *readData)
{
    /* 检查ADU的长度是否合理 */
    if (rxAdu->valueLen < WX_MODBUS_ADU_RD_RSP_MIN_LEN) {
        return WX_RS422_MASTER_RECV_RSP_RD_VALUE_LEN_ERR;
    }
    UINT8 dataLen = rxAdu->value[WX_MODBUS_ADU_RD_RSP_DATA_LEN_IDX];
    if (dataLen + 3 + 2 != rxAdu->valueLen) {
        return WX_RS422_MASTER_RECV_RSP_RD_VALUE_LEN_ERR;
    }

    CHAR *data = &rxAdu->value[WX_MODBUS_ADU_RD_RSP_DATA_START_IDX];
    /* 获取读数据类型对应的解码函数 */
    WxRs422MasterRdDateDecFunc decFunc = WX_RS422_MASTER_GetDecodeFunc(rdDataRspMsg->subMsgType);
    if (decFunc == NULL) {
        return WX_RS422_MASTER_RECV_RSP_RD_GET_DEC_FUNC_FAIL;
    }
    
    /* 调用解码函数解码，把数据流解码到结构体  */
    return decFunc(data, dataLen, readData);
}