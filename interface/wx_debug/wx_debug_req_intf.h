#ifndef __WX_DEBUG_REQ_INTF_H__
#define __WX_DEBUG_REQ_INTF_H__
#include "wx_msg_common.h"
#define WX_DEBUG_CMD_MAX_LEN 248
typedef struct {
    UINT32 cmdLen;
    UINT8 cmdData[WX_DEBUG_CMD_MAX_LEN];
} WxDbgCmd;

/* 消息内容 */
typedef struct {
    WX_INHERIT_MSG_HEADER
    WxDbgCmd dbgCmd; /* this is the debug command */
} WxDebugReqMsg;

#endif // __WX_DEBUG_REQ_INTF_H__