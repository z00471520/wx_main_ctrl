#ifndef __WX_RS422_MASTER_WR_DATA_RSP_INTF_H__
#define __WX_RS422_MASTER_WR_DATA_RSP_INTF_H__
#include "wx_typedef.h"
#include "wx_msg_common.h"
#include "wx_rs422_master_wr_data_req_intf.h"

/* 写数据响应 */
typedef struct {
    WxRs422MasterWrDataType subMsgType;
    UINT32 failCode;
} WxRs422MasterWrDataRsp;

/******************************************************************************
 * RS422消息
 *****************************************************************************/
typedef struct {
    WX_INHERIT_MSG_HEADER
    WxRs422MasterWrDataRsp rsp;
} WxRs422MasterWrDatRspMsg;

#endif
