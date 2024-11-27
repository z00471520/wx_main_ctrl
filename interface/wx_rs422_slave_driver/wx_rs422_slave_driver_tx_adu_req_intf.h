#ifndef __WX_RS422_SLAVE_DRIVER_TX_ADU_REQ_INTF_H__
#define __WX_RS422_SLAVE_DRIVER_TX_ADU_REQ_INTF_H__
#include "wx_msg_common.h"
/* WX_MSG_TYPE_RS422_SLAVE_TX_ADU_REQ*/
typedef struct  {
    WX_INHERIT_MSG_HEADER
    WxModbusAdu txAdu; /* modbus adu to be sent bydriver */
} WxRs422SlaverDriverTxAduReq;

#endif //__WX_RS422_SLAVE_DRIVER_TX_ADU_MSG_INTF_H__