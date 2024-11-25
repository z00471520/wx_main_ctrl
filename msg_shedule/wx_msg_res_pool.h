
#ifndef __WX_EVT_MSG_RES_POOL_H__
#define __WX_EVT_MSG_RES_POOL_H__
#include "wx_evt_msg.h"
#include "wx_msg_common.h"
/* 中断程序申请消息 */
WxMsg *WX_ApplyEvtMsgFromISR(WxMsgType msgType);
WxMsg *WX_ApplyEvtMsg(WxMsgType msgType);
/* 释放消息, msg释放后会设置为空 */
VOID WX_FreeEvtMsg(WxMsg **ppEvtMsg);
/* 中断程序 */
VOID WX_FreeEvtMsgFromISR(WxMsg **ppEvtMsg);
/* 创建消息资源池 */
UINT32 WX_CreateMsgResPool(UINT32 itemNum);
#endif