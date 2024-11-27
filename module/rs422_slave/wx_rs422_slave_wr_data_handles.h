
#include "wx_rs422_i_salve.h"

/* 不同地址有自己的对应的解码和写操作 */
WxRs422SlaveWrDataHandle g_wxRs422ISlaveDataWriteHandles[WX_RS422_SLAVE_DATA_ADDR_BUTT] = {
    [WX_RS422_SLAVE_DATA_ADDR_X] = {NULL, NULL},
    /* if more please add here */
};

WxRs422SlaveWrDataHandle *WX_RS422Slave_GetWrDataHandles(VOID)
{
    return g_wxRs422ISlaveDataWriteHandles;
}