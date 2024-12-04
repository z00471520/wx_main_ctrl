
#ifndef WX_MSG_INTF_H
#define WX_MSG_INTF_H
#include "wx_typedef.h"
#define WX_EVT_MSG_DATA_SIZE 1500 /* default data size of event message */

/* 消息大类 */
typedef enum {
    WX_MSG_TYPE_INVALID,
    WX_MSG_TYPE_REMOTE_CTRL,        /* remote control msg， see wx_remote_ctrl_msg_def.h for detail */
    WX_MSG_TYPE_CAN_FRAME,          /* CAN FRAME消息，see wx_msg_can_frame_intf.h for detail */
    WX_MSG_TYPE_RS422_MASTER_DRIVER_REQ, /* WxRs422MasterDriverMsg */
    /* RS422 master */
    WX_MSG_TYPE_RS422_MASTER_ADU_RSP,
    WX_MSG_TYPE_RS422_MASTER_WR_DATA_REQ, /* WxRs422MasterWrDatReqMsg */
    WX_MSG_TYPE_RS422_MASTER_WR_DATA_RSP, /* WxRs422MasterWrDatRspMsg */
    WX_MSG_TYPE_RS422_MASTER_RD_DATA_REQ, /* WxRs422MasterRdDatReqMsg */
    WX_MSG_TYPE_RS422_MASTER_RD_DATA_RSP, /* WxRs422MasterRdDatRspMsg */
    WX_MSG_TYPE_RS422_SLAVE_RX_ADU_REQ,   /* WxRs422SalverRxAduMsg */
    WX_MSG_TYPE_RS422_SLAVE_TX_ADU_REQ,  /* WxRs422SlaveTxAduReq */
    WX_MSG_TYPE_BEBUG_REQ,
    WX_MSG_TYPE_UART_TX_DATA_REQ,
    WX_MSG_TYPE_UART_CFG_REQ,
    /* if more please add here */
    WX_MSG_TYPE_BUTT,
} WxMsgType;

#define WX_IsValidMsgType(t) ((t) > WX_MSG_TYPE_INVALID && (t) < WX_MSG_TYPE_BUTT)
/* 消息头定义 */
#define WX_INHERIT_MSG_HEADER                                       \
UINT8 sender;   /* message sender ID, see WxModuleId */           \
UINT8 receiver; /* message receiver ID, see WxModuleId */         \
UINT8 outEvent;  /* out event ID, */                                \
UINT8 isFromISR: 1; /* is ISR malloc the message, 1-yes, 0-no */ \
UINT8 resv: 7;  /* resv bit */                                        \
UINT16 msgType; /* message type, see WxMsgType */                 \
UINT16 subMsgType; /* sub message type */                                                  
   
/* 消息体定义 */
typedef struct tagWxMsg{
WX_INHERIT_MSG_HEADER
UINT8  msgData[0]; /* 最大长度为：WX_EVT_MSG_DATA_SIZE */
} WxMsg;
#endif