#ifndef __WX_MSG_RS422_DRIVER_MASTER_H__
#define __WX_MSG_RS422_DRIVER_MASTER_H__
#include "wx_modbus.h"
#include "wx_msg_common.h"

typedef enum {
    WX_SUB_MSG_REQ_RS422_MASTER_DRIVER_TX_ADU = 0x01,
} WxRs422MasterDriverSubMsgId;

typedef struct {
    UINT16 msgType;     /* the msg type of adu */
    UINT16 subMsgType;  /* the sub msg type of adu */
    UINT8 sender; /* who is the orign sender */
    WxModbusAdu adu; /* the modbus adu to be sent */
} WxRs422MasterDriverTxDataReq;

/* WX_MSG_TYPE_RS422_MASTER_DRIVER_REQ */
typedef struct {
    /* the sender of the is wo sender the request */
    /* the submsgtype is  */
    WX_INHERIT_MSG_HEADER
    union {
        WxRs422MasterDriverTxDataReq reqTxData; /* WX_SUB_MSG_REQ_RS422_MASTER_DRIVER_TX_ADU */
    };
} WxRs422MasterDriverMsg;

#endif //__WX_MSG_RS422_DRIVER_MASTER_H__
