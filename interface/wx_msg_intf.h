
#ifndef WX_MSG_INTF_H
#define WX_MSG_INTF_H
#include "wx_typedef.h"
#define WX_EVTMSG_DATA_SIZE 1500 /* 默认的消息长度 */
/* 当前内核支持的消息处理模块定义 */
typedef enum {
    WX_MODULE_CAN_A,
    WX_MODULE_CAN_B,
    WX_MODULE_RS422_I_MASTER, /* 内部外设通信使用的RS422 */
    WX_MODULE_ZJ_SPI_DRIVER,
    /* if more please add here */
    WX_MODULE_BUTT,
} WxMsgReceiver, WxMsgSender;

/* 消息大类 */
typedef enum {
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
} WxMsgType;

typedef struct {
    ;
}EvtMsgBody;

typedef struct {
    UINT16 msgType; /* 消息类类型（WxMsgType） */
    UINT16 msgVer;  /* 消息版本号 */
    UINT32 resv; /* 预留字段 */
    EvtMsgBody *msgBody;  /* 消息体指针，具体结构体由消息类型自定义 */
} WxEvtMsg;



#endif