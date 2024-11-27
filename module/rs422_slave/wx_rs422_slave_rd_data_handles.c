#include "wx_rs422_salve.h"

/* 不同地址由自己的对应的读和编码操作 */
WxRs422SlaveRdDataHandle g_wxRs422SlaveRdDataHandles[WX_RS422_SLAVE_DATA_ADDR_BUTT] = {
    /* 读数据地址X                    获取数据函数  对数据编码的函数 */
    [WX_RS422_SLAVE_DATA_ADDR_X] = {NULL, NULL},
    /* if more please add here */
};

WxRs422SlaveRdDataHandle *WX_RS422Slave_GetRdDataHandles(UINT8 moduleId, UINT16 *handNum)
{
    /* only one module for now */
    *handNum = WX_RS422_SLAVE_DATA_ADDR_BUTT;
    return g_wxRs422SlaveRdDataHandles;
}