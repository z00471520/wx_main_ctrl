#include "wx_include.h"
#include "wx_msg_intf.h"
#define TASK_EVT_MSG_NODE_NUM           8196 /* 一个任务的消息队列支持的消息节点数 */
typedef struct {
    QueueHandle_t evtMsgQue;
    WxEvtMsg evtMsgArray[TASK_EVT_MSG_NODE_NUM];
} WxEvtMsgPool;

WxEvtMsgPool *g_wxEvtMsgPool = NULL;
/* 创建EvtMsg的资源池 */
UINT32 WX_CreateMsgResPool(VOID)
{
    if (g_wxEvtMsgPool != NULL) {
        return WX_SUCCESS;
    }
    /* 申请内存 */
    WxEvtMsgPool *evtMsgPool = WX_Mem_Alloc("WxEvtMsgPool", 1, sizeof(WxEvtMsgPool));
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
    WX_CLEAR_OBJ(evtMsg);
    evtMsg->msgType = msgType;
    evtMsg->isFromISR = TRUE;
    return evtMsg;
}

WxEvtMsg *WX_ApplyEvtMsg(WxMsgType msgType) 
{
    WxEvtMsg *evtMsg = NULL;
    if (xQueueReceive(g_wxEvtMsgPool->evtMsgQue, (void *)&evtMsg, (TickType_t)0) != pdPASS) {
        return NULL;
    }
    WX_CLEAR_OBJ(evtMsg);
    evtMsg->msgType = msgType;
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

