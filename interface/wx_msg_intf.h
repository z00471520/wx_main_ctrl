
#ifndef WX_MSG_INTF_H
#define WX_MSG_INTF_H
#include "wx_typedef.h"
#define WX_EVT_MSG_BODY_SIZE 1500 /* 默认的消息长度 */
/* 当前内核支持的消息处理模块定义 */
typedef enum {
    WX_MODULE_CAN_A,
    WX_MODULE_CAN_B,
    WX_MODULE_RS422_I_MASTER, /* 内部外设通信使用的RS422 */
    WX_MODULE_ZJ_SPI_DRIVER,
    /* if more please add here */
    WX_MODULE_BUTT,
} WxMsgReceiver, WxMsgSender;


#define WX_IsValidMsgType(t) ((t) > WX_MSG_TYPE_INVALID && (t) < WX_MSG_TYPE_BUTT)

/* 消息大类 */
typedef enum {
    WX_MSG_TYPE_INVALID,
    WX_MSG_TYPE_REMOTE_CTRL,        /* 遥控消息， see wx_remote_ctrl_msg_def.h for detail */
    WX_MSG_TYPE_CAN_FRAME,          /* CAN FRAME消息，see wx_msg_can_frame_intf.h.h for detail */
    WX_RS422I_MASTER_MSG_READ_DATA,        /* 读数据请求, 子类型：WxRs422IReadDataType,  消息体为：NA */
    WX_RS422I_MASTER_MSG_READ_DATA_RSP,    /* 读数据响应, 子类型：WxRs422IReadDataType,  消息体为：WxRs422IReadDataRsp */
    WX_RS422I_MASTER_MSG_WRITE_DATA,       /* 写数据请求, 子类型：WxRs422IWriteDataType, 消息体为: WxRs422IWriteData */
    WX_RS422I_MASTER_MSG_WRITE_DATA_RSP,   /* 写数据响应, 子类型: WxRs422IWriteDataType, NA */
    WX_RS422I_MASTER_MSG_READ_FILE,
    WX_RS422I_MASTER_MSG_READ_FILE_RSP,
    WX_RS422I_MASTER_MSG_WRITE_FILE,
    WX_RS422I_MASTER_MSG_WRITE_FILE_RSP,
    /* if more please add here */
    WX_MSG_TYPE_BUTT,
} WxMsgType;

/* 卫星内部传递的模块均需要以此方式进行 */
typedef struct {
    UINT32 transID;    /* 消息对应的事务ID */
    UINT8 sender;   /* 消息发送者 */
    UINT8 receiver; /* 消息接收者 */
   	UINT16 msgType;    /* 消息类型, 详见枚举 WxMsgType 定义 */
    UINT16 msgSubType; /* 消息子类型, 由大类确定 */
    UINT16 msgBodyLen; /* 消息体实际长度 --- msgBody的长度, 必选：不能超过，WX_MSG_DEFAULT_BODY_SIZE */
    UINT16 outEvent;   /* 消息处理的出事件， WX_SUCCESS - 表示成功 */
    UINT8  msgBody[0];
} WxEvtMsgHeader;

typedef union {

} WxMsgBody;

typedef struct {
    WxEvtMsgHeader msgHead;
    UINT8 msgBody[WX_EVT_MSG_BODY_SIZE]; /* 自定义的结构体不能超过该长度 */
} WxEvtMsg;

#endif