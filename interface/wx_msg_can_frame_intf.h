#ifndef WX_CAN_DRIVER_SLAVE_INTF_H
#define WX_CAN_DRIVER_SLAVE_INTF_H
#include "wx_can_driver.h"
#include "wx_msg_intf.h"
/******************************************************************************
 * CAN FRAME请求消息 WX_CAN_DRIVER_SLAVE_MSG_CAN_FRAME_REQ
 *****************************************************************************/
// 消息内容为： WxCanFrame
typedef struct {
    WxMsgHeader msgHead;
    /* 联合体由MSG header中的类型和子类型确定 */
    typedef union {
        WxCanFrame canFrame; /* WX_CAN_DRIVER_SLAVE_MSG_CAN_FRAME_REQ:NA */
        /* if more please add here 新增需要注明类型和子类型 */
    };
} WxCanFrameMsg;

#endif