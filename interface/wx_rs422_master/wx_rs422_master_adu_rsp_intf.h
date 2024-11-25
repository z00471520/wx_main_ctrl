#ifndef __WX_RS422_MASTER_ADU_RSP_INTF_H
#define __WX_RS422_MASTER_ADU_RSP_INTF_H
#include "wx_typedef.h"
#include "wx_modbus.h"
#include "wx_msg_common.h"
#include "wx_rs422_master_adu_rsp_intf.h"
/******************************************************************************
 * RS422消息
 *****************************************************************************/
typedef struct {
    WX_INHERIT_MSG_HEADER
    union {
        WxModbusAdu modbusAdu;
    };
} WxRs422MasterAduRspMsg;

#endif
