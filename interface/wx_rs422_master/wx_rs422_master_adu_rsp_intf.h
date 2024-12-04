#ifndef __WX_RS422_MASTER_ADU_RSP_INTF_H
#define __WX_RS422_MASTER_ADU_RSP_INTF_H
#include "wx_typedef.h"
#include "wx_modbus.h"
#include "wx_msg_common.h"

/* WX_MSG_TYPE_RS422_MASTER_ADU_RSP */
typedef struct {
    UINT16 reqMsgType;  /* request function code */
    UINT16 reqSubMsgType;  /* request sub message type */
    UINT16 rspCode; /* responce code */
    UINT8 reqSender; /* request sender */
    WxModbusAdu rspAdu; /* response modbus adu when msg */
} WxRs422MasterDriverRspData;

typedef struct {
    WX_INHERIT_MSG_HEADER
    union {
        WxRs422MasterDriverRspData data; /* default 0 */
    };
} WxRs422MasterDriverRspMsg;

#endif
