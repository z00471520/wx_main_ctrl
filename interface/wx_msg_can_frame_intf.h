#ifndef WX_CAN_SLAVE_INTF_H
#define WX_CAN_SLAVE_INTF_H
#include "wx_can_driver.h"
#include "wx_msg_common.h"
/******************************************************************************
 * CAN FRAME请求消息 WX_MSG_TYPE_CAN_FRAME
 *****************************************************************************/
// 消息内容为： WxCanFrame
typedef struct {
    WX_INHERIT_MSG_HEADER
    WxCanFrame canFrame; /* WX_MSG_TYPE_CAN_FRAME:NA */
} WxCanFrameMsg;

#endif