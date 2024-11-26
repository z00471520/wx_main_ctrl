#include "wx_rs422_i_salve.h"

/* 不同地址由自己的对应的读和编码操作 */
WxRs422ISlaveDataReadHandle g_wxRs422ISlaveDataReadHandles[WX_RS422_I_SLAVE_DATA_ADDR_BUTT] = {
    [WX_RS422_I_SLAVE_DATA_ADDR_X] = {NULL, NULL},
    /* if more please add here */
};

WxRs422ISlaveDataReadHandle *WX_RS422I_Slave_GetDataReadHandle(WxRs422ISlaveDataAddr dataAddr)
{
    if (dataAddr >= WX_RS422_I_SLAVE_DATA_ADDR_BUTT) {
        return NULL;
    }
    return &g_wxRs422ISlaveDataReadHandles[dataAddr];
}