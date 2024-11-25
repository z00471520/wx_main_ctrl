#ifndef __WX_MSG_RS422_DRIVER_MASTER_REQ_H__
#define __WX_MSG_RS422_DRIVER_MASTER_REQ_H__
#include "wx_modbus.h"
#include "wx_msg_common.h"
/* WX_MSG_TYPE_RS422_MASTER_DRIVER */
typedef struct {
    WX_INHERIT_MSG_HEADER
    union {
        WxModbusAdu modbusAdu; /* WX_SUB_MSG_RS422_DRIVER_MASTER_TX_ADU */
    };
} WxRs422MasterDriverMsg;

#endif //__WX_MSG_RS422_DRIVER_MASTER_H__