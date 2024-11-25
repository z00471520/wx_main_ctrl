#ifndef __WX_RS422_MASTER_RD_DATA_RSP_INTF_H__
#define __WX_RS422_MASTER_RD_DATA_RSP_INTF_H__
#include "wx_typedef.h"
#include "wx_modbus.h"
#include "wx_msg_common.h"

/******************************************************************************
 * 读数据响应
 ******************************************************************************/
typedef enum {
    WX_RS422_MASTER_MSG_READ_DATA_XXX = 0, /* 数据结构体为： WxRs422IDataXXXInfo */
    WX_RS422_MASTER_MSG_READ_DATA_YYY = 1, /* 数据结构体为： WxRs422IDataYYYInfo */
    WX_RS422_MASTER_MSG_READ_DATA_BUTT,    /* 占位符 */
} WxRs422MasterRdDataType;

/******************************************************************************
 * 读数据响应
 ******************************************************************************/
typedef struct {

} WxRs422MasterRdDataY;

typedef struct {
    
} WxRs422MasterRdDataX;


typedef struct {
    UINT32 failCode; /* 错误码 */
    UINT32 resv;     /* 保留 */
    union { 
        WxRs422MasterRdDataX dataX;
        WxRs422MasterRdDataY dataY;
        /* if more please add here */
    };
} WxRs422MsgReadDataRsp;

/******************************************************************************
 * RS422消息
 *****************************************************************************/
typedef struct {
    WX_INHERIT_MSG_HEADER
    WxRs422MsgReadDataRsp rsp;
} WxRs422MasterRdDataRspMsg;

#endif
