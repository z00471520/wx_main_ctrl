#ifndef __WX_CAN_SLAVE_PROC_RMT_RSP_MSG_H__
#define __WX_CAN_SLAVE_PROC_RMT_RSP_MSG_H__
#include "wx_can_slave.h"

/*
 * 閬ユ帶娑堟伅瀛愮被鍨�
 */
typedef enum {
    WX_RMT_CTRL_RSP_MSG_TYPE_RESET, /* 鎸囩ず澶嶄綅鏃犳暟鎹� */
    WX_RMT_CTRL_RSP_MSG_TYPE_TELEMETRY_DATA, /* 閬ユ祴鏁版嵁璇锋眰 */
    /* if more please */
    WX_RMT_CTRL_RSP_MSG_TYPE_BUTT,
} WxRmtCtrlRspMsgType;

/* 閬ユ帶娑堟伅瀹氫箟 */
typedef struct {
    WxRmtCtrlRspMsgType type; /* 璇锋眰娑堟伅绫诲瀷 */
    union {
    	UINT8 resv;
    };
} WxRmtCtrlRspMsg;

typedef UINT32 (*WxRmtCtrlRspMsgEncPduHandle)(WxCanSlave *this, WxRmtCtrlRspMsg *rspMsg, WxRmtCtrlPdu *pdu);
UINT32 WX_CAN_SLAVE_SendRspMsg2CanIf(WxCanSlave *this, WxRmtCtrlRspMsg *rspMsg);
#endif
