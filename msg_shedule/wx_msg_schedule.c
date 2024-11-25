#include "wx_msg_dispatch.h"
#include "wx_failcode.h"
#include "wx_msg_common.h"
#include "wx_task_deploy.h"
#include "wx_include.h"
WxMsgTypeRouterList *g_wxMsgRouterList = NULL;

/* 创建消息调度的路由表 */
UINT32 WX_MsgShedule_CreateMsgRouterList(void)
{
    if (g_wxMsgRouterList == NULL) {
        g_wxMsgRouterList = (WxMsgTypeRouterList *)WX_Mem_Alloc(WxMsgTypeRouterList, 1, sizeof(WxMsgTypeRouterList));
        if (g_wxMsgRouterList == NULL) {
            return WX_FAIL_MEM_ALLOC;
        }
        memset(g_wxMsgRouterList, 0, sizeof(WxMsgTypeRouterList));
    }
    g_wxMsgRouterList->coreId = WX_GetCurCoreId();
    return WX_SUCCESS;
}

UINT32 WX_MsgShedule_DestroyMsgRouterList(void)
{
    if (g_wxMsgRouterList == NULL) {
        return WX_SUCCESS;
    }
    WX_Mem_Free(g_wxMsgRouterList);
    g_wxMsgRouterList = NULL;
    return WX_SUCCESS;
}


UINT32 WX_MsgShedule_RegRouter(WxModuleId receiver, UINT8 coreId, 
    WxTask *belongTask, WxModule *belongModule, WxModuleEntryFunc entryFunc)
{
    if (g_wxMsgRouterList == NULL) {
        return WX_MSG_REG_NO_MSG_ROUTER_LIST;
    }
    if (!WX_IsValidModuleId(receiver)）{
        return WX_MSG_REG_INVALID_MODULE_ID;
    }
    WxMsgTypeRouter *this = g_wxMsgRouterList->routers[receiver];
    this->coreId = coreId;
    this->belongTask = belongTask;
    this->belongModule = belongModule;
    this->entryFunc = entryFunc;
    return WX_SUCCESS;
}

INLINE UINT32 WX_MsgShedule_CheckInput(UINT8 sender, UINT8 receiver, VOID *msg)
{
    if (msg == NULL) {
        return WX_MSG_DISPATCH_INVALID_MSG_HEADER;
    }
    if (WX_IsValidModuleId(sender) == FALSE) {
        return WX_MSG_DISPATCH_INVALID_SENDER;
    }
    if (WX_IsValidModuleId(receiver) == FALSE) {
        return WX_MSG_DISPATCH_INVALID_RECVER;
    }
    if (g_wxMsgRouterList == NULL) {
        return WX_MSG_DISPATCH_NO_MSG_ROUTER_LIST;
    }
    return WX_SUCCESS;
}
/* 计算消息的发送方式 */
WxMsgTypeSendMethod WX_MsgSchedule_CalcMsgSendMethod(WxMsgTypeRouter *sender, WxMsgTypeRouter *receiver)
{
    /* 核不同不能直接发送消息 */
    if (receiver->coreId != sender->coreId) {
        return WX_MSG_SEND_TO_CORE;
    } else {
        return WX_MSG_SEND_TO_TASK;
    }
}

/* 发送消息到其他核 TODO*/
UINT32 WX_MsgShedule_ToCore(WxMsgTypeRouter *receiver, VOID *msg)
{
    /* TODO */
    return WX_ERR;
}

/* 发送消息到任务 */
UINT32 WX_MsgShedule_ToTask(WxMsgTypeRouter *receiver, VOID *msg)
{
    WxTask *task = receiver->belongTask;
    WxMsg *msgHead = (WxMsg *)msg;
    UINT32 sendResult;
    if (msgHead->isFromIsr) {
        sendResult = xQueueSendFromISR(task->msgQueHandle, (const void * )&msg, (TickType_t)0);
    } else {
        sendResult = xQueueSend(task->msgQueHandle, (const void * )&msg, (TickType_t)0);
    }
    if (sendResult != pdTRUE) {
        task->msgQueFullCnt++;
        return WX_MSG_DISPATCH_MSG_QUE_FULL;
    }
    return WX_SUCCESS;
}


/*
 * 函数功能：消息调度，用于发送消息到指定的模块
 * 参数说明
 * sender：发送者模块ID
 * receiver：接收者模块ID
 * msg：消息体指针
 **/
UINT32 WX_MsgShedule(UINT8 sender, UINT8 receiver, VOID *msg)
{
    UINT32 ret = WX_MsgShedule_CheckInput(sender, receiver, msg);
    if (ret != WX_SUCCESS) {
        return ret;
    }

    WxMsg *msgHead = (WxMsg *)msg;
    msgHead->sender = sender;
    msgHead->receiver = receiver;
    WxMsgTypeRouter *dstRouter = &g_wxMsgRouterList->routers[receiver];
    WxMsgTypeRouter *srcRouter = &g_wxMsgRouterList->routers[sender];
    WxMsgTypeSendMethod sendMethod = WX_MsgSchedule_CalcMsgSendMethod(srcRouter, dstRouter);
    switch (sendMethod) {
        case WX_MSG_SEND_TO_CORE: {
            return WX_MsgShedule_ToCore(dstRouter, msg);
        }
        case WX_MSG_SEND_TO_TASK: {
            ret = WX_MsgShedule_ToTask(dstRouter, msg);
            return ret;
        }         
        default:
            return WX_MSG_DISPATCH_INVALID_SEND_METHOD;
    }
}