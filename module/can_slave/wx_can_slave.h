#ifndef __WX_CAN_SLAVE_H__
#define __WX_CAN_SLAVE_H__
/* 卫星遥控的指令码-需要根据卫星通信规范确定 */
typedef enum{
    WX_RMT_CTRL_CODE_RESET = 0, /* 复位请求 */
    WX_RMT_CTRL_CODE_DEVICE_SELF_CHECK = 1, /* 设备自检 */
    WX_RMT_CTRL_CODE_BUTT,
} WxRmtCtrlCodeDef;

#define WX_CAN_DRIVER_FRAME_LIST_MAX_ITERM_NUM 200
typedef struct
{
    UINT32 canFrameNum; /* CAN Frame个数 */ 
    WxCanFrame canFrames[WX_CAN_DRIVER_FRAME_LIST_MAX_ITERM_NUM];
}WxCanFrameList;

typedef struct {
    UINT16 rmtCtrlCode; /* PDU对应遥控指令码 */
    UINT16 dataLen;     /* 遥控指令码对应的PDU数据长度 */
    UINT8 data[WX_CAN_SLAVE_PDU_DATA_SIZE]; /* 遥控指令码对应的PDU数据 */
} WxRmtCtrlPdu;

/* 解码PDU的函数 */
typedef UINT32 (*WxRmtCtrlPduDecHandle)(WxCanSlave *this, WxRmtCtrlPdu *pdu, WxRmtCtrlReqMsg *msg);
typedef UINT32 (*WxRmtCtrlReqMsgHandle)(WxCanSlave *this, WxRmtCtrlReqMsg *msg);
typedef struct {
    WxRmtCtrlReqMsgType msgType; /* 消息类型 */
    WxRmtCtrlPduDecHandle decHandle; /* 遥控指令码的对应的PDU解码函数，解析出消息 */
    WxRmtCtrlReqMsgHandle msgQueHandle; /* 消息处理Handle */
} WxRmtCtrlReqHandle;



typedef struct {
    UINT8 canFrameDataLen; /* CAN帧数据固定长度 */
    UINT8 moduleId;       /* 模块ID */
    UINT32 messId;    /* CAN消息ID */
    WxCanDriverCfg deviceCfgInfo; /* CAN设备配置信息 */ 
    WxCanDriverIntrCfg intrCfgInfo; /* 中断配置信息 */
} WxCanSlaveCfg;

/* CAN从机任务信息 */
typedef struct {
    WxModuleId moduleId;            /* 当前模块的ID */
    UINT32 resv;                    /* 保留字段 */
    WxRmtCtrlReqMsg reqMsg;         /* CAN 遥控请求请求 */
    WxRmtCtrlPdu reqPdu;            /* CAN Req PDU */
    WxRmtCtrlPdu rspPdu;            /* CAN RSP PDU */
    WxCanFrameList canFrameList;    /* 待发送canFrameList */
    WxCanSlaveCfg *cfgInfo;     /* 配置信息的指针 */
    QueueHandle_t txBuffQue;        /* 发送CANframe的缓冲队列 */
} WxCanSlave;

#endif //__WX_CAN_SLAVE_H__