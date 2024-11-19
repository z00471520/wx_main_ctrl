#ifndef __WX_MSG_REMOTE_CTRL_INTF_H__
#define __WX_MSG_REMOTE_CTRL_INTF_H__
#include "wx_msg_intf.h"
/*
 * 遥控消息子类型
 */
typedef enum {
    WX_RMT_CTRL_MSG_TYPE_RESET_REQ, /* 指示复位无数据 */
    WX_RMT_CTRL_MSG_TYPE_RESET_RSP, /* 指示复位操作后回复响应无数据 */
    WX_RMT_CTRL_MSG_TYPE_TELEMETRY_DATA_REQ, /* 遥测数据请求 */
    WX_RMT_CTRL_MSG_TYPE_TELEMETRY_DATA_RSP, /* 遥测数据响应 */
    
    /* if more please */
    WX_RMT_CTRL_MSG_TYPE_BUTT,
} WxRmtCtrlMsgType;


typedef struct {
    WxMsgHeader msgHead;
    typedef union {
        
    };
} WxRemoteCtrlMsg;

#endif