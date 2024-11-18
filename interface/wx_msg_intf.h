
#ifndef WX_MSG_INTF_H
#define WX_MSG_INTF_H
#include "wx_typedef.h"
#define WX_MSG_DEFAULT_BODY_SIZE 1500 /* 默认的消息长度 */
/* 当前内核支持的消息处理模块定义 */
typedef enum {
    WX_MODULE_CAN_A,
    WX_MODULE_CAN_B,
    WX_MODULE_RS422_I_MASTER, /* 内部外设通信使用的RS422 */
    /* if more please add here */
    WX_MODULE_BUTT,
} WxMsgReceiver, WxMsgSender;

/* 消息大类 */
typedef enum {
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
    UINT32 transID;    /* 消息对应的事务ID */
    UINT8 sender;   /* 消息发送者 */
    UINT8 receiver; /* 消息接收者 */
   	UINT16 msgType;    /* 消息类型, 详见枚举 WxMsgType 定义 */
    UINT16 msgSubType; /* 消息子类型, 由大类确定 */
    UINT16 msgBodyLen; /* 消息体实际长度 --- msgBody的长度 */
    UINT16 outEvent;   /* 消息处理的出事件， WX_SUCCESS - 表示成功 */
    UINT8  msgBody[0];
} WxMsgHeader;

typedef struct {
    WxMsgHeader msgHead;
    UINT8 msgBody[WX_MSG_DEFAULT_BODY_SIZE]; /* 具体是啥由消息类型和子类型确定 */
} WxMsg;

#endif