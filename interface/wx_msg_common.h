
#ifndef WX_MSG_INTF_H
#define WX_MSG_INTF_H
#include "wx_typedef.h"
#define WX_EVT_MSG_DATA_SIZE 1500 /* 默认的消息长度 */


/* 消息大类 */
typedef enum {
    WX_MSG_TYPE_INVALID,
    WX_MSG_TYPE_REMOTE_CTRL,        /* 遥控消息， see wx_remote_ctrl_msg_def.h for detail */
    WX_MSG_TYPE_CAN_FRAME,          /* CAN FRAME消息，see wx_msg_can_frame_intf.h for detail */
    WX_MSG_TYPE_RS422_MASTER_DRIVER, /* WxRs422MasterDriverMsg */
    /* RS422 master */
    WX_MSG_TYPE_RS422_MASTER_ADU_RSP,
    WX_MSG_TYPE_RS422_MASTER_WR_DATA_REQ, /* WxRs422MasterWrDatReqMsg */
    WX_MSG_TYPE_RS422_MASTER_WR_DATA_RSP, /* WxRs422MasterWrDatRspMsg */
    WX_MSG_TYPE_RS422_MASTER_RD_DATA_REQ, /* WxRs422MasterRdDatReqMsg */
    WX_MSG_TYPE_RS422_MASTER_RD_DATA_RSP, /* WxRs422MasterRdDatRspMsg */
    WX_MSG_TYPE_BEBUG,
    WX_MSG_TYPE_UART_TX_DATA_REQ,
    WX_MSG_TYPE_UART_CFG_REQ,
    /* if more please add here */
    WX_MSG_TYPE_BUTT,
} WxMsgType;

#define WX_IsValidMsgType(t) ((t) > WX_MSG_TYPE_INVALID && (t) < WX_MSG_TYPE_BUTT)

#define WX_INHERIT_MSG_HEADER                                   \
    UINT32 transID;  /* 消息对应的事务ID */                     \  
    UINT8 sender;     /* 消息发送模块ID, 详见: WxModuleId */ \
    UINT8 receiver; /* 消息接收模块ID, 详见: WxModuleId */ \
   	UINT16 msgType; /* 消息类型, 详见枚举 WxMsgType 定义 */ \
    UINT16 msgSubType; /* 消息子类型, 由大类确定 */ \
    UINT16 msgDataLen; /* 消息体实际长度 --- msgData的长度, 必选：不能超过，WX_MSG_DEFAULT_BODY_SIZE */\
    UINT8 outEvent;  /* 消息处理的出事件， WX_SUCCESS - 表示成功 */ \
    UINT8 isFromIsr: 1; /* 消息是否来自中断，1-来自中断，0-来自任务 */ \
    UINT8 resv: 7;  /* 保留位 */

typedef struct {
    WX_INHERIT_MSG_HEADER
    UINT8  msgData[0]; /* 最大长度为：WX_EVT_MSG_DATA_SIZE */
} WxMsg;
#endif