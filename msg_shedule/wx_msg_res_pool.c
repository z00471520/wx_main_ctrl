#include "wx_include.h"
#include "wx_msg_common.h"
#include "FreeRTOS.h"
#include "queue.h"
#define TASK_EVT_MSG_NODE_NUM           8196 /* 涓�涓换鍔＄殑娑堟伅闃熷垪鏢�寔鐨勬秷鎭妭鐐规暄1�71ￄ1�77 */
typedef struct {
    QueueHandle_t evtMsgQue;
    WxMsgType evtMsgArray[TASK_EVT_MSG_NODE_NUM];
} WxMsgTypePool;

WxMsgTypePool *g_wxEvtMsgPool = NULL;
/* 创建消息资源池 */
UINT32 WX_CreateMsgResPool(VOID)
{
    if (g_wxEvtMsgPool != NULL) {
        return WX_SUCCESS;
    }
    /* 申请节点 */
    WxMsgTypePool *evtMsgPool = WX_Mem_Alloc("WxMsgTypePool", 1, sizeof(WxMsgTypePool));
    /* 创建一个消息队列用于维护节点指针 */
    evtMsgPool->evtMsgQue = xQueueCreate((UBaseType_t)TASK_EVT_MSG_NODE_NUM, (UBaseType_t)sizeof(WxMsg *));
    if (evtMsgPool->evtMsgQue == NULL) {
        WX_Mem_Free(evtMsgPool);
        return WX_CREATE_QUEUE_FAIL;
    }
    /* 把节点指针加入到消息队列中 */
    WxMsg *evtMsg = NULL;
    for (UINT32 i = 0; i < TASK_EVT_MSG_NODE_NUM; i++) {
        evtMsg = (WxMsg *)&evtMsgPool->evtMsgArray[i];
        xQueueSend(evtMsgPool->evtMsgQue, (void *)&evtMsg, (TickType_t)0);
    }

    g_wxEvtMsgPool = evtMsgPool;
    return WX_SUCCESS;
}

/* 中断申请 */
VOID *WX_ApplyEvtMsgFromISR(WxMsgType msgType)
{
    WxMsg *evtMsg = NULL;
    if (xQueueReceiveFromISR(g_wxEvtMsgPool->evtMsgQue, (void *)&evtMsg, (TickType_t)0) != pdPASS) {
        return NULL;
    }
    WX_CLEAR_OBJ(evtMsg);
    evtMsg->msgType = msgType;
    evtMsg->isFromISR = TRUE;
    return evtMsg;
}

VOID *WX_ApplyEvtMsg(WxMsgType msgType) 
{
    WxMsg *evtMsg = NULL;
    if (xQueueReceive(g_wxEvtMsgPool->evtMsgQue, (void *)&evtMsg, (TickType_t)0) != pdPASS) {
        return NULL;
    }
    WX_CLEAR_OBJ(evtMsg);
    evtMsg->msgType = msgType;
    return evtMsg;
}
/* 非中断释放 */
VOID WX_FreeEvtMsg(WxMsg **pp)
{
    if (*pp == NULL) {
        return;
    }
    /* 节点节点到消息队列 */
    if (xQueueSend(g_wxEvtMsgPool->evtMsgQue, (VOID *)pp, (TickType_t)0) != pdPASS) {
        return;
    }
    *pp = NULL;
    return;
}

/* 中断释放消息节点 */
VOID WX_FreeEvtMsgFromISR(WxMsg **pp)
{
    if (*pp == NULL) {
        return;
    }
    /* 发送节点到消息队列 */
    if (xQueueSendFromISR(g_wxEvtMsgPool->evtMsgQue, (VOID *)pp, (TickType_t)0) != pdPASS) {
        return;
    }
    *pp = NULL;
    return;
}

