#include "wx_include.h"
#include "wx_msg_intf.h"
#define TASK_EVT_MSG_NODE_NUM           8196 /* 一个任务的消息队列支持的消息节点数 */
typedef struct {
    QueueHandle_t evtMsgQue;
    WxMsgType evtMsgArray[TASK_EVT_MSG_NODE_NUM];
} WxMsgTypePool;

WxMsgTypePool *g_wxEvtMsgPool = NULL;
/* 创建EvtMsg的资源池 */
UINT32 WX_CreateMsgResPool(VOID)
{
    if (g_wxEvtMsgPool != NULL) {
        return WX_SUCCESS;
    }
    /* 申请内存 */
    WxMsgTypePool *evtMsgPool = WX_Mem_Alloc("WxMsgTypePool", 1, sizeof(WxMsgTypePool));
    /* 创建一个队列用于装消息的指针 */
    evtMsgPool->evtMsgQue = xQueueCreate((UBaseType_t)itemNum, (UBaseType_t)sizeof(WxMsgType *));
    if (evtMsgPool->evtMsgQue == NULL) {
        WX_Mem_Free(evtMsgPool);
        return NULL;
    }
    /* 把消息的指针发送到消息队列 */
    WxMsgType *evtMsg = NULL;
    for (UINT32 i = 0; i < itemNum; i++) {
        evtMsg = &evtMsgPool->evtMsgArray[i];
        xQueueSend(evtMsgPool->evtMsgQue, (void *)&evtMsg, (TickType_t)0);
    }

    g_wxEvtMsgPool = evtMsgPool;
    return WX_SUCCESS;
}

/* 中断程序申请消息 */
WxMsgType *WX_ApplyEvtMsgFromISR(WxMsgType msgType)
{
    WxMsgType *evtMsg = NULL;
    if (xQueueReceiveFromISR(g_wxEvtMsgPool->evtMsgQue, (void *)&evtMsg, (TickType_t)0) != pdPASS) {
        return NULL;
    }
    WX_CLEAR_OBJ(evtMsg);
    evtMsg->msgType = msgType;
    evtMsg->isFromISR = TRUE;
    return evtMsg;
}

WxMsgType *WX_ApplyEvtMsg(WxMsgType msgType) 
{
    WxMsgType *evtMsg = NULL;
    if (xQueueReceive(g_wxEvtMsgPool->evtMsgQue, (void *)&evtMsg, (TickType_t)0) != pdPASS) {
        return NULL;
    }
    WX_CLEAR_OBJ(evtMsg);
    evtMsg->msgType = msgType;
    return evtMsg;
}
/* 释放消息, msg释放后会设置为空 */
VOID WX_FreeEvtMsg(WxMsgType **ppEvtMsg)
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
VOID WX_FreeEvtMsgFromISR(WxMsgType **ppEvtMsg)
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

