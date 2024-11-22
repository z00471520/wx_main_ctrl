#ifndef WX_CAN_SLAVE_INTF_H
#define WX_CAN_SLAVE_INTF_H
#include "wx_can_driver.h"
#include "wx_msg_intf.h"
/******************************************************************************
 * CAN FRAME请求消息 WX_MSG_TYPE_CAN_FRAME
 *****************************************************************************/
// 消息内容为： WxCanFrame
typedef struct {
    WxEvtMsgHeader msgHead;
    WxCanFrame canFrame; /* WX_MSG_TYPE_CAN_FRAME:NA */
} WxCanFrameMsg;

#endif