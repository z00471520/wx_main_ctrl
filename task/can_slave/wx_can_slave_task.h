#ifndef __WX_CAN_SLAVE_TASK_H__
#define __WX_CAN_SLAVE_TASK_H__
#include "wx_can.h"
#include "wx_msg_remote_ctrl_intf.h"
/* 卫星遥控的指令码-需要根据卫星通信规范确定 */
typedef enum{
    WX_RMT_CTRL_CODE_RESET = 0, /* 复位请求 */
    WX_RMT_CTRL_CODE_DEVICE_SELF_CHECK = 1, /* 设备自检 */
    WX_RMT_CTRL_CODE_BUTT,
} WxRmtCtrlCodeDef;

typedef struct {
    UINT16 rmtCtrlCode; /* 遥控指令码 */
} WxRmtCtrlPdu;

VOID WX_CAN_SlaveTask(VOID *param);
UINT32 WX_CAN_SLAVE_DecodeRemoteCtrlMsg(WxCanSlaveTask *this, WxCanFrame *canFrame, WxRemoteCtrlMsg **ppRemoteCtrlMsg);
#endif