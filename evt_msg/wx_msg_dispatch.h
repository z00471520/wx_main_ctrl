#ifndef WX_MSG_DISPATCH_H
#define WX_MSG_DISPATCH_H
#include "wx_include.h"
#include "wx_msg_intf.h"
UINT32 WX_RegMsgQue(WxMsgReceiver receiver, QueueHandle_t queHandle, UINT32 itermSize);
UINT32 WX_MSG_Dispatch(WxMsgSender sender, WxMsgReceiver receiver, WxEvtMsgHeader *msgHead);
#endif