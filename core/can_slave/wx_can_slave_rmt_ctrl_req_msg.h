#ifndef __WX_CAN_SLAVE_PROC_RMT_REQ_MSG_H__
#define __WX_CAN_SLAVE_PROC_RMT_REQ_MSG_H__
#include "wx_can_slave.h"

/*
 * 遥控消息子类型
 */
typedef enum {
    WX_RMT_CTRL_REQ_MSG_TYPE_RESET, /* 指示复位无数据 */
    WX_RMT_CTRL_REQ_MSG_TYPE_TELEMETRY_DATA, /* 遥测数据请求 */
    /* if more please */
    WX_RMT_CTRL_REQ_MSG_TYPE_BUTT,
} WxRmtCtrlReqMsgType;

/* 遥控消息定义 */
typedef struct {
    WxRmtCtrlReqMsgType type; /* 请求消息类型 */
    typedef union {
    };
} WxRmtCtrlReqMsg;


UINT32 WX_CAN_SLAVE_ProcRmtCtrlReqMsgReset(WxCanSlave *this, WxRmtCtrlReqMsg *msg);
#endif