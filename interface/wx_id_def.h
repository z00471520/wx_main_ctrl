#ifndef __WX_ID_DEF_H__
#define __WX_ID_DEF_H__
/******************************************************************************
 * ��ID����
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
#define WX_IS_VALID_CORE_ID(coreId) (((coreId) != WX_CORE_ID_NA) && ((coreId) & WX_CORE_ID_ALL == (coreId)))
/******************************************************************************
 * module ID
 *****************************************************************************/
typedef enum {
    WX_MODULE_INVALID, /* this is invalid module ID */
    WX_MODULE_TEST, /* for test */
    WX_MODULE_CAN_MASTER,
    WX_MODULE_CAN_SLAVE_A,
    WX_MODULE_CAN_SLAVE_B,
    WX_MODULE_RS422_MASTER,
    WX_MODULE_RS422_SLAVE,
    WX_MODULE_ZJ_SPI_DRIVER,
    WX_MODULE_DRIVER_CAN_A,
    WX_MODULE_DRIVER_CAN_B,
    WX_MODULE_DRIVER_RS422_MASTER,
    WX_MODULE_DRIVER_RS422_SLAVE,
    WX_MODULE_DIRVER_DBG_UART,
    WX_MODULE_DEBUG_C0,
    WX_MODULE_DEBUG_C1,
    WX_MODULE_DEBUG_C2,
    WX_MODULE_DEBUG_C3,
    /* if more please add here */
    WX_MODULE_BUTT,
} WxModuleId;

#define WX_IsValidModuleId(i) ((i) > WX_MODULE_INVALID && (i) < WX_MODULE_BUTT)

#endif
