#ifndef __WX_RS422_MASTER_RD_DATA_REQ_INTF_H__
#define __WX_RS422_MASTER_RD_DATA_REQ_INTF_H__
#include "wx_typedef.h"
#include "wx_msg_common.h"
#include "wx_rs422_master_rd_data_rsp_intf.h"
typedef struct {
    WxRs422MasterRdDataType dataType; /* 读数据需要明确提供数据的从机地址 */
    UINT8 reReadTime; /* 读数据校验错误重读次数*/
    UINT8 rsv[3];
} WxRs422MasterRdDataReq

/******************************************************************************
 * RS422消息
 *****************************************************************************/
typedef struct {
    WX_INHERIT_MSG_HEADER
    WxRs422MasterRdDataReq req;
} WxRs422MasterRdDataReqMsg;

#endif
