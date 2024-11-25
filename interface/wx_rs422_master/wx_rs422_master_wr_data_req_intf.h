#ifndef __WX_RS422_MASTER_WR_DATA_REQ_INTF_H__
#define __WX_RS422_MASTER_WR_DATA_REQ_INTF_H__
#include "wx_typedef.h"
#include "wx_modbus.h"
#include "wx_msg_common.h"

/******************************************************************************
 * 写数据请求子类型定义
******************************************************************************/
typedef enum {
    WX_RS422_MASTER_MSG_WRITE_MMM,  /* WxRs422MasterWrDataMMMInfo */
    WX_RS422_MASTER_MSG_WRITE_NNN,  /* WxRs422IDataNNNInfo */
    WX_RS422_MASTER_MSG_WRITE_BUTT,
} WxRs422MasterWrDataType;

/******************************************************************************
 * 写数据请求的结构体定义
******************************************************************************/
typedef struct {

} WxRs422MasterWrDataX;

typedef struct {
    
} WxRs422MasterWrDataY;

/******************************************************************************
 * RS422消息
 *****************************************************************************/
typedef struct {
    WX_INHERIT_MSG_HEADER
    union {
        WxRs422MasterWrDataX MMM;
        WxRs422MasterWrDataY NNN; 
    };
} WxRs422MasterWrDataReqMsg;

#endif
