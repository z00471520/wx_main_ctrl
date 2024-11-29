#include "wx_rs422_master_wr_data_req_intf.h"
#include "wx_include.h"
#include "wx_rs422_master.h"
UINT8 WX_RS422_MASTER_EncodeWrDataX(UINT8 buf[], UINT8 bufSize, VOID *data)
{
    return WX_ERR;
}

UINT8 WX_RS422_MASTER_EncodeWrDataX(UINT8 buf[], UINT8 bufSize, VOID *data)
{
    return WX_ERR;
}


/* messege wrdata proc handle */
WxRs422MasterWrDataEncHandle g_wxRs422MasterWrDataEncHandles[WX_RS422_MASTER_MSG_WRITE_BUTT] = {
    /* sub msg type                     slave address            data addr         data encode func */
    [WX_RS422_MASTER_MSG_WRITE_MMM] = {WX_RS422_MASTER_SLAVE_ADDR_XXX, 0, WX_RS422_MASTER_EncodeWrDataX},
    [WX_RS422_MASTER_MSG_WRITE_NNN] = {WX_RS422_MASTER_SLAVE_ADDR_YYY, 0, WX_RS422_MASTER_EncodeWrDataX},
};

WxRs422MasterWrDataEncHandle* WX_RS422_MASTER_GetWrDataHandle(UINT16 subMsgType)
{
    if (subMsgType >= WX_RS422_MASTER_MSG_WRITE_BUTT) {
        return NULL;
    }
    return &g_wxRs422MasterWrDataEncHandles[subMsgType];
}

 
