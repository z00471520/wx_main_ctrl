
#include "wx_rs422_i_salve.h"

/* 不同地址有自己的对应的解码和写操作 */
WxRs422ISlaveDataWriteHandle g_wxRs422ISlaveDataWriteHandles[WX_RS422_I_SLAVE_DATA_ADDR_BUTT] = {
    [WX_RS422_I_SLAVE_DATA_ADDR_X] = {NULL, NULL},
    /* if more please add here */
};

WxRs422ISlaveDataWriteHandle *WX_RS422I_Slave_GetDataWriteHandle(WxRs422ISlaveDataAddr dataAddr)
{
    if (dataAddr >= WX_RS422_I_SLAVE_DATA_ADDR_BUTT) {
        return NULL;
    }
    return &g_wxRs422ISlaveDataReadHandles[dataAddr];
}