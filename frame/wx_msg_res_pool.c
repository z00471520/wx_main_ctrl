#include "wx_include.h"
#include "wx_msg_common.h"
#include "FreeRTOS.h"
#include "queue.h"
#define TASK_EVT_MSG_NODE_NUM           8196 /* 娑擄拷娑擃亙鎹㈤崝锛勬畱濞戝牊浼呴梼鐔峰灙閺拷顖涘瘮閻ㄥ嫭绉烽幁顖濆Ν閻愯鏆�1锟�71锟�1锟�77 */
typedef struct {
    QueueHandle_t evtMsgQue;
    WxMsgType evtMsgArray[TASK_EVT_MSG_NODE_NUM];
} WxMsgTypePool;

WxMsgTypePool *g_wxEvtMsgPool = NULL;
/* 鍒涘缓娑堟伅璧勬簮姹� */
UINT32 WX_CreateMsgResPool(VOID)
{
	boot_debug("Create event msg start...");
    if (g_wxEvtMsgPool != NULL) {
        return WX_SUCCESS;
    }
    /* alloc mem */
    WxMsgTypePool *evtMsgPool = WX_Mem_Alloc("WxMsgTypePool", 1, sizeof(WxMsgTypePool));
    /* create a queue to store the msg ptr */
    evtMsgPool->evtMsgQue = xQueueCreate((UBaseType_t)TASK_EVT_MSG_NODE_NUM, (UBaseType_t)sizeof(WxMsg *));
    if (evtMsgPool->evtMsgQue == NULL) {
    	boot_debug("Error Exit: Create queue fail!");
        WX_Mem_Free(evtMsgPool);
        return WX_CREATE_QUEUE_FAIL;
    }
    /* add the msg point to the que for future alloc */
    WxMsg *evtMsg = NULL;
    for (UINT32 i = 0; i < TASK_EVT_MSG_NODE_NUM; i++) {
        evtMsg = (WxMsg *)&evtMsgPool->evtMsgArray[i];
        xQueueSend(evtMsgPool->evtMsgQue, (void *)&evtMsg, (TickType_t)0);
    }
    boot_debug("Create event msg success!");
    g_wxEvtMsgPool = evtMsgPool;
    return WX_SUCCESS;
}

/* 涓柇鐢宠 */
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
/* 闈炰腑鏂噴鏀� */
VOID WX_FreeEvtMsg(WxMsg **pp)
{
    if (*pp == NULL) {
        return;
    }
    /* 鑺傜偣鑺傜偣鍒版秷鎭槦鍒� */
    if (xQueueSend(g_wxEvtMsgPool->evtMsgQue, (VOID *)pp, (TickType_t)0) != pdPASS) {
        return;
    }
    *pp = NULL;
    return;
}

/* 涓柇閲婃斁娑堟伅鑺傜偣 */
VOID WX_FreeEvtMsgFromISR(WxMsg **pp)
{
    if (*pp == NULL) {
        return;
    }
    /* 鍙戦�佽妭鐐瑰埌娑堟伅闃熷垪 */
    if (xQueueSendFromISR(g_wxEvtMsgPool->evtMsgQue, (VOID *)pp, (TickType_t)0) != pdPASS) {
        return;
    }
    *pp = NULL;
    return;
}

