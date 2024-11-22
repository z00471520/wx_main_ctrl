#include "wx_include.h"
#include "wx_msg_intf.h"

typedef struct {
    QueueHandle_t evtMsgQue;
    WxEvtMsg evtMsgArray[0];
} WxEvtMsgPool;

WxEvtMsgPool *g_wxEvtMsgPool = NULL;
/* 创建EvtMsg的资源池 */
UINT32 WX_CreateEvtMsgResPool(UINT32 itemNum)
{
    if (itemNum == 0) {
        return WX_ERR;
    }

    if (g_wxEvtMsgPool != NULL) {
        return WX_SUCCESS;
    }
    /* 申请内存 */
    WxEvtMsgPool *evtMsgPool = WX_Mem_Alloc("EVTMSGPool", 1, sizeof(WxEvtMsgPool) + itemNum * sizeof(WxEvtMsg));
    /* 创建一个队列用于装消息的指针 */
    evtMsgPool->evtMsgQue = xQueueCreate((UBaseType_t)itemNum, (UBaseType_t)sizeof(WxEvtMsg *));
    if (evtMsgPool->evtMsgQue == NULL) {
        WX_Mem_Free(evtMsgPool);
        return NULL;
    }
    /* 把消息的指针发送到消息队列 */
    WxEvtMsg *evtMsg = NULL;
    for (UINT32 i = 0; i < itemNum; i++) {
        evtMsg = &evtMsgPool->evtMsgArray[i];
        xQueueSend(evtMsgPool->evtMsgQue, (void *)&evtMsg, (TickType_t)0);
    }

    g_wxEvtMsgPool = evtMsgPool;
    return WX_SUCCESS;
}

/* 中断程序申请消息 */
WxEvtMsg *WX_ApplyEvtMsgFromISR(WxMsgType msgType)
{
    WxEvtMsg *evtMsg = NULL;
    if (xQueueReceiveFromISR(g_wxEvtMsgPool->evtMsgQue, (void *)&evtMsg, (TickType_t)0) != pdPASS) {
        return NULL;
    }
    evtMsg->msgHead.msgType = msgType;
    return evtMsg;
}

WxEvtMsg *WX_ApplyEvtMsg(WxMsgType msgType) 
{
    WxEvtMsg *evtMsg = NULL;
    if (xQueueReceive(g_wxEvtMsgPool->evtMsgQue, (void *)&evtMsg, (TickType_t)0) != pdPASS) {
        return NULL;
    }

    return evtMsg;
}
/* 释放消息, msg释放后会设置为空 */
VOID WX_FreeEvtMsg(WxEvtMsg **ppEvtMsg)
{
    if (*ppEvtMsg == NULL) {
        return;
    }
    /* 归还到队列里面 */
    if (xQueueSend(evtMsgPool->evtMsgQue, (VOID *)ppEvtMsg, (TickType_t)0) != pdPASS) {
        return;
    }
    *ppEvtMsg = NULL;
    return;
}

/* 中断程序 */
VOID WX_FreeEvtMsgFromISR(WxEvtMsg **ppEvtMsg)
{
    if (*ppEvtMsg == NULL) {
        return;
    }
    /* 归还到队列里面 */
    if (xQueueSendFromISR(evtMsgPool->evtMsgQue, (VOID *)ppEvtMsg, (TickType_t)0) != pdPASS) {
        return;
    }
    *ppEvtMsg = NULL;
    return;
}
