#ifndef __WX_CAN_SLAVE_H__
#define __WX_CAN_SLAVE_H__
#include "wx_include.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "wx_can_driver.h"
#include "wx_can_slave.h"

#define WX_CAN_SLAVE_PDU_DATA_SIZE 1500
/* the remote control code */
typedef enum {
    WX_RMT_CTRL_CODE_RESET = 0,
	WX_RMT_CTRL_CODE_DEVICE_SELF_CHECK = 1,
    WX_RMT_CTRL_CODE_BUTT,
} WxRmtCtrlCodeDef;


/*
 * 閬ユ帶娑堟伅瀛愮被鍨�
 */
typedef enum {
    WX_RMT_CTRL_REQ_MSG_TYPE_RESET, /* 鎸囩ず澶嶄綅鏃犳暟鎹� */
    WX_RMT_CTRL_REQ_MSG_TYPE_TELEMETRY_DATA, /* 閬ユ祴鏁版嵁璇锋眰 */
    /* if more please */
    WX_RMT_CTRL_REQ_MSG_TYPE_BUTT,
} WxRmtCtrlReqMsgType;

/* 閬ユ帶娑堟伅瀹氫箟 */
typedef struct {
    WxRmtCtrlReqMsgType type; /* 璇锋眰娑堟伅绫诲瀷 */
    union {
        UINT8 resv; /* 淇濈暀 */
    };
} WxRmtCtrlReqMsg;


#define WX_CAN_DRIVER_FRAME_LIST_MAX_ITERM_NUM 200
typedef struct tagWxCanFrameList
{
    UINT32 canFrameNum; /* CAN Frame number */
    WxCanFrame canFrames[WX_CAN_DRIVER_FRAME_LIST_MAX_ITERM_NUM];
} WxCanFrameList;

typedef struct {
    UINT16 rmtCtrlCode;
    UINT16 dataLen;
    UINT8 data[WX_CAN_SLAVE_PDU_DATA_SIZE];
} WxRmtCtrlPdu;

/* 钩子函数定义 */
typedef UINT32 (*WxRmtCtrlPduDecHandle)(VOID *this, WxRmtCtrlPdu *pdu, WxRmtCtrlReqMsg *msg);
typedef UINT32 (*WxRmtCtrlReqMsgHandle)(VOID *this, WxRmtCtrlReqMsg *msg);
typedef struct {
    WxRmtCtrlReqMsgType msgType; /* 娑堟伅绫诲瀷 */
    WxRmtCtrlPduDecHandle decHandle; /* 閬ユ帶鎸囦护鐮佺殑瀵瑰簲鐨凱DU瑙ｇ爜鍑芥暟锛岃В鏋愬嚭娑堟伅 */
    WxRmtCtrlReqMsgHandle msgQueHandle; /* 娑堟伅澶勭悊Handle */
} WxRmtCtrlReqHandle;

WxRmtCtrlReqHandle g_wxRmtCtrlReqHandles[WX_RMT_CTRL_CODE_BUTT];
typedef struct {
    UINT8 canFrameDataLen; 	/* can frame data len */
    UINT8 moduleId;       	/* module id */
    UINT32 messId;    		/* message id */
} WxCanSlaveCfg;

typedef struct {
    WxModuleId moduleId;
    UINT32 resv;
    WxRmtCtrlReqMsg reqMsg;         /* remote ctrl req msg */
    WxRmtCtrlPdu reqPdu;            /* CAN Req PDU */
    WxRmtCtrlPdu rspPdu;            /* CAN RSP PDU */
    WxCanFrameList canFrameList;    /* can frame list to be sent */
    WxCanSlaveCfg *cfgInfo;     	/* point to the config */
} WxCanSlave;

UINT32 WX_CAN_SLAVE_Construct(VOID *module);
UINT32 WX_CAN_SLAVE_Destruct(VOID *module);
UINT32 WX_CAN_SLAVE_Entry(VOID *module, WxMsg *evtMsg);
#endif //__WX_CAN_SLAVE_H__
