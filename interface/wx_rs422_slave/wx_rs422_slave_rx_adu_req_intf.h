#ifndef __WX_RS422_SLAVE_RX_ADU_REQ_INTF_H__
#define __WX_RS422_SLAVE_RX_ADU_REQ_INTF_H__
#include "wx_msg_common.h"
#include "wx_modbus.h"
/* WX_MSG_TYPE_RS422_SLAVE_RX_ADU_REQ */

typedef struct {
    WX_INHERIT_MSG_HEADER
    WxModbusAdu rxAdu; /* modbus adu received from driver */
} WxRs422SlaveRxAduReq;

#endif //__WX_RS422_SLAVE_RX_ADU_MSG_INTF_H__