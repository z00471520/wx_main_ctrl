#ifndef __WX_CAN_SLAVE_H__
#define __WX_CAN_SLAVE_H__
#include "wx_include.h"
#define WX_CAN_SLAVE_PDU_DATA_SIZE 1500
/* 鍗槦閬ユ帶鐨勬寚浠ょ爜-闇�瑕佹牴鎹崼鏄熼�氫俊瑙勮寖纭畾 */
typedef enum {
    WX_RMT_CTRL_CODE_RESET = 0, /* 澶嶄綅璇锋眰 */
    WX_RMT_CTRL_CODE_DEVICE_SELF_CHECK = 1, /* 璁惧鑷 */
    WX_RMT_CTRL_CODE_BUTT,
} WxRmtCtrlCodeDef;

#define WX_CAN_DRIVER_FRAME_LIST_MAX_ITERM_NUM 200
typedef struct tagWxCanFrameList
{
    UINT32 canFrameNum; /* CAN Frame涓暟 */
    WxCanFrame canFrames[WX_CAN_DRIVER_FRAME_LIST_MAX_ITERM_NUM];
} WxCanFrameList;

typedef struct {
    UINT16 rmtCtrlCode; /* PDU瀵瑰簲閬ユ帶鎸囦护鐮� */
    UINT16 dataLen;     /* 閬ユ帶鎸囦护鐮佸搴旂殑PDU鏁版嵁闀垮害 */
    UINT8 data[WX_CAN_SLAVE_PDU_DATA_SIZE]; /* 閬ユ帶鎸囦护鐮佸搴旂殑PDU鏁版嵁 */
} WxRmtCtrlPdu;

/* 瑙ｇ爜PDU鐨勫嚱鏁� */
typedef UINT32 (*WxRmtCtrlPduDecHandle)(WxCanSlave *this, WxRmtCtrlPdu *pdu, WxRmtCtrlReqMsg *msg);
typedef UINT32 (*WxRmtCtrlReqMsgHandle)(WxCanSlave *this, WxRmtCtrlReqMsg *msg);
typedef struct {
    WxRmtCtrlReqMsgType msgType; /* 娑堟伅绫诲瀷 */
    WxRmtCtrlPduDecHandle decHandle; /* 閬ユ帶鎸囦护鐮佺殑瀵瑰簲鐨凱DU瑙ｇ爜鍑芥暟锛岃В鏋愬嚭娑堟伅 */
    WxRmtCtrlReqMsgHandle msgQueHandle; /* 娑堟伅澶勭悊Handle */
} WxRmtCtrlReqHandle;

typedef struct {
    UINT8 canFrameDataLen; /* CAN甯ф暟鎹浐瀹氶暱搴� */
    UINT8 moduleId;       /* 妯″潡ID */
    UINT32 messId;    /* CAN娑堟伅ID */
    WxCanDriverCfg deviceCfgInfo; /* CAN璁惧閰嶇疆淇℃伅 */
    WxCanDriverIntrCfg intrCfgInfo; /* 涓柇閰嶇疆淇℃伅 */
} WxCanSlaveCfg;

/* CAN浠庢満浠诲姟淇℃伅 */
typedef struct {
    WxModuleId moduleId;            /* 褰撳墠妯″潡鐨処D */
    UINT32 resv;                    /* 淇濈暀瀛楁 */
    WxRmtCtrlReqMsg reqMsg;         /* CAN 閬ユ帶璇锋眰璇锋眰 */
    WxRmtCtrlPdu reqPdu;            /* CAN Req PDU */
    WxRmtCtrlPdu rspPdu;            /* CAN RSP PDU */
    WxCanFrameList canFrameList;    /* 寰呭彂閫乧anFrameList */
    WxCanSlaveCfg *cfgInfo;     /* 閰嶇疆淇℃伅鐨勬寚閽� */
    QueueHandle_t txBuffQue;        /* 鍙戦�丆ANframe鐨勭紦鍐查槦鍒� */
} WxCanSlave;

#endif //__WX_CAN_SLAVE_H__
