#ifndef __WX_CAN_SLAVE_H__
#define __WX_CAN_SLAVE_H__
#include "wx_can_driver.h"
#include "wx_msg_remote_ctrl_intf.h"
#include "wx_can_slave_rmt_ctrl_req_msg.h"
/* 卫星遥控的指令码-需要根据卫星通信规范确定 */
typedef enum{
    WX_RMT_CTRL_CODE_RESET = 0, /* 复位请求 */
    WX_RMT_CTRL_CODE_DEVICE_SELF_CHECK = 1, /* 设备自检 */
    WX_RMT_CTRL_CODE_BUTT,
} WxRmtCtrlCodeDef;

#define WX_CAN_SLAVE_PDU_DATA_SIZE 1500
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

/* CAN类型的定义 */
typedef enum {
    WX_CAN_DRIVER_TYPE_A,
    WX_CAN_DRIVER_TYPE_B,
    WX_CAN_DRIVER_TYPE_BUTT,
} WxCanTypeDef;

/* 自定义配置 */
typedef struct
{
    UINT8 canFrameDataLen; /* CAN帧数据固定长度 */
    UINT32 messId;    /* CAN消息ID */
    UINT32 maxMsgNum; /* 任务缓存CAN帧的个数 */
    /* if more please add here */
} WxCanSlaveSelfDefCfg;


typedef struct {
    WxCanSlaveSelfDefCfg selfDefCfg;
    WxTaskCfgInfo taskCfg;
    WxCanDeviceCfgInfo deviceCfgInfo; /* CAN设备配置信息 */ 
    WxCanIntrCfgInfo intrCfgInfo; /* 中断配置信息 */
} WxCanSlaveCfgInfo;

#define WX_CAN_DRIVER_FRAME_LIST_MAX_ITERM_NUM 200
typedef struct
{
    UINT32 canFrameNum; /* CAN Frame个数 */ 
    WxCanFrame canFrames[WX_CAN_DRIVER_FRAME_LIST_MAX_ITERM_NUM];
}WxCanFrameList;

/* CAN从机任务信息 */
typedef struct {
    QueueHandle_t msgQue;       /* 消息队列 */
    XCanPs canInst;             /* 当前任务处理的Can实例 */
    TaskHandle_t taskHandle;    /* 任务Handle */
    UINT8 canDataBuff;          /* CAN  */
    WxRmtCtrlReqMsg rmtCtrlmsg; /* CAN MSG */
    WxRmtCtrlPdu reqPdu;        /* CAN Req PDU */
    WxRmtCtrlPdu rspPdu;        /* CAN RSP PDU */
    WxCanFrameList canFrameList; /* 待发送canFrameList */
    WxCanSlaveCfgInfo *cfgInfo; /* 配置信息的指针 */
} WxCanSlave;

WxRmtCtrlReqHandle g_wxRmtCtrlReqHandles[WX_RMT_CTRL_CODE_BUTT];
VOID WX_CAN_DRIVER_SlaveTask(VOID *param);
UINT32 WX_CAN_SLAVE_DecodeRemoteCtrlMsg(WxCanSlave *this, WxCanFrame *canFrame, WxRmtCtrlReqMsg **ppRemoteCtrlMsg);
UINT32 WX_CAN_SLAVE_SendPdu2CanIf(WxCanSlave *this, WxRmtCtrlPdu *pdu);
#endif