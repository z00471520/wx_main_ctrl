#ifndef __WX_RS422_MASTER_ADU_RSP_INTF_H
#define __WX_RS422_MASTER_ADU_RSP_INTF_H
#include "wx_typedef.h"
#include "wx_modbus.h"
#include "wx_msg_common.h"
#include "wx_rs422_master_rsp_adu_intf.h"

/* WX_MSG_TYPE_RS422_MASTER_ADU_RSP */
typedef struct {
    WX_INHERIT_MSG_HEADER
    union {
        WxModbusAdu rspAdu; /* default 0 */
    };
} WxRs422MasterRspAduMsg;

#endif
