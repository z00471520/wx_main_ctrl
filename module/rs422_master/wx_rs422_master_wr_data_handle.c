#include "wx_rs422_master_wr_data_req_intf.h"
#include "wx_include.h"
#include "wx_rs422_master.h"
#include "wx_modbus.h"

UINT8 WX_RS422_MASTER_EncodeWrDataX(WxModbusAdu *adu, WxRs422MasterWrData *wrData)
{
    WxRs422MasterWrDataX *x = &wrData->MMM;
    UINT8 dataLen = adu->valueLen;
    wx_debug("before X size=%u, aduLen=%u", sizeof(WxRs422MasterWrDataX), adu->valueLen);
    UINT32 ret = WX_SUCCESS;
    ret |= WX_MODBUS_ADU_ENCODE_BASIC(adu, x->data8);
    ret |= WX_MODBUS_ADU_ENCODE_BASIC(adu, x->data16);
    ret |= WX_MODBUS_ADU_ENCODE_BASIC(adu, x->data32);
    ret |= WX_MODBUS_ADU_ENCODE_BASIC(adu, x->data64);
    if (ret != WX_SUCCESS) {
        wx_excp_cnt(WX_RS422_MASTER_EXCP_CODE_ENCODE_ADU);
        return 0;
    }
    dataLen = adu->valueLen - dataLen;
    wx_debug("aft aduLen=%u encLen=%u", adu->valueLen, dataLen);
    return dataLen;
};


UINT8 WX_RS422_MASTER_EncodeWrDataY(WxModbusAdu *adu, WxRs422MasterWrData *wrData)
{
    wx_excp_cnt(WX_RS422_MASTER_EXCP_CODE_WR_DATA_Y);
    return 0;
}

/* messege wrdata proc handle */
WxRs422MasterWrDataEncHandle g_wxRs422MasterWrDataEncHandles[WX_RS422_MASTER_MSG_WRITE_BUTT] = {
    /* sub msg type                     slave address            data addr         data struct encode data stream func */
    [WX_RS422_MASTER_MSG_WRITE_MMM] = {WX_RS422_MASTER_SLAVE_ADDR_XXX, 1, WX_RS422_MASTER_EncodeWrDataX},
    [WX_RS422_MASTER_MSG_WRITE_NNN] = {WX_RS422_MASTER_SLAVE_ADDR_YYY, 2, WX_RS422_MASTER_EncodeWrDataY},
};

WxRs422MasterWrDataEncHandle* WX_RS422_MASTER_GetWrDataHandle(UINT16 subMsgType)
{
    if (subMsgType >= WX_RS422_MASTER_MSG_WRITE_BUTT) {
        return NULL;
    }
    return &g_wxRs422MasterWrDataEncHandles[subMsgType];
}

 
