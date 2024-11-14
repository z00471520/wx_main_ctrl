#ifndef WX_MSG_DISPATCH_H
#define WX_MSG_DISPATCH_H
#include "WX_"
WxFailCode WX_RegMsgQue(WxMsgReceiver receiver, QueueHandle_t queHandle, UINT32 itermSize) ;
WxFailCode WX_MSG_Dispatch(WxMsgSender sender, WxMsgReceiver receiver, WxMsgHeader *msgHead);
#endif