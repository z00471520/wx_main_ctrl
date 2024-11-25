#ifndef __WX_MODULE_ID_H__
#define __WX_MODULE_ID_H__
#include  "wx_include.h"
/******************************************************************************
 * 核ID定义 
 *****************************************************************************/
typedef enum {
    WX_CORE_ID_NA = 0,
    WX_CORE_ID_0 = 0x1,
    WX_CORE_ID_1 = 0x2,
    WX_CORE_ID_2 = 0x4,
    WX_CORE_ID_3 = 0x8,
} WxCoreId;

#define WX_CORE_ID_ALL 0xF
#define WX_CORE_NUM     4
#define WX_CORE_IDX_2_ID(coreIdx) ((UINT32)1 << (coreIdx))
inline UINT32 WX_CORE_ID_2_IDX(WxCoreId coreId)
{
    switch (coreId) {
        case WX_CORE_ID_0:
            return 0;
        case WX_CORE_ID_1:
            return 1;
        case WX_CORE_ID_2:
            return 2;
        case WX_CORE_ID_3:
            return 3;
        default:
            return ;
    }
}
#define WX_IS_VALID_CORE_ID(coreId) (((coreId) != WX_CORE_ID_NA) && ((coreId) & WX_CORE_ID_ALL == (coreId)))
/******************************************************************************
 * module ID
 *****************************************************************************/
typedef enum {

    WX_MODULE_INVALID, /* 无效ID */
    WX_MODULE_CAN_SLAVE_A,
    WX_MODULE_CAN_SLAVE_B,
    WX_MODULE_RS422_I_MASTER, /* 内部外设通信使用的RS422 */
    WX_MODULE_ZJ_SPI_DRIVER,
    WX_MODULE_DRIVER_CAN_A,
    WX_MODULE_DRIVER_CAN_B,
    WX_MODULE_DRIVER_RS422_MASTER,
    WX_MODULE_DRIVER_RS422_SLAVE,
    WX_MODULE_DIRVER_DBG_UART,
    WX_MODULE_DEBUG,
    /* if more please add here */
    WX_MODULE_BUTT,
} WxModuleId;

#define WX_IsValidModuleId(i) ((i) > WX_MODULE_INVALID && (i) < WX_MODULE_BUTT)

#endif
