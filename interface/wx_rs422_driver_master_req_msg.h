#ifndef __WX_MSG_RS422_DRIVER_MASTER_REQ_H__
#define __WX_MSG_RS422_DRIVER_MASTER_REQ_H__
#include "wx_modbus.h"
#include "wx_msg_header.h"
typedef struct {
    WX_INHERIT_MSG_HEADER
    union {
        WxModbusAdu modbusAdu; /* WX_SUB_MSG_RS422_DRIVER_MASTER_TX_ADU */
    };
    
} WxRs422DriverMaserReqMsg;

#endif //__WX_MSG_RS422_DRIVER_MASTER_H__