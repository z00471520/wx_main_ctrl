#include "wx_msg_dispatch.h"
#include "wx_failcode.h"
#include "wx_msg_intf.h"
#include "wx_task_deploy.h"
WxMsgRouterList *g_wxMsgRouterList = NULL;

/* 创建消息调度的路由表 */
UINT32 WX_MsgShedule_CreateMsgRouterList(void)
{
    if (g_wxMsgRouterList == NULL) {
        g_wxMsgRouterList = (WxMsgRouterList *)WX_Mem_Alloc(WxMsgRouterList, 1, sizeof(WxMsgRouterList));
        if (g_wxMsgRouterList == NULL) {
            return WX_FAIL_MEM_ALLOC;
        }
        memset(g_wxMsgRouterList, 0, sizeof(WxMsgRouterList));
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
    WxMsgRouter *this = g_wxMsgRouterList->routers[receiver];
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
INLINE WxMsgSendMethod WX_MsgSchedule_CalcMsgSendMethod(WxMsgRouter *sender, WxMsgRouter *receiver)
{
    /* 核不同不能直接发送消息 */
    if (receiver->coreId != sender->coreId) {
        return WX_MSG_SEND_TO_CORE;
    }
    /* 如果是驱动任务，驱动任务没有缓存队列（这里的指针不可能为NULL） */
    return (receiver->belongTask->msgQueHandle) ?  WX_MSG_SEND_TO_TASK : WX_MSG_SEND_TO_MODULE;
}

/* 发送消息到其他核 TODO*/
UINT32 WX_MsgShedule_ToCore(WxMsgRouter *receiver, VOID *msg)
{
    /* TODO */
    return WX_ERR;
}

/* 发送消息到任务 */
INLINE UINT32 WX_MsgShedule_ToTask(WxMsgRouter *receiver, VOID *msg)
{
    WxTask *task = receiver->belongTask;
    if (xQueueSend(task->msgQueHandle, (const void * )&msg, (TickType_t)0 ) != pdTRUE) {
        task->msgQueFullCnt++;
        return WX_MSG_DISPATCH_MSG_QUE_FULL;
    }
    return WX_SUCCESS;
}

/* 发送到模块 */
INLINE UINT32 WX_MsgShedule_ToModule(WxMsgRouter *receiver, VOID *msg)
{
    WxModule *module = receiver->belongModule;
    if (module == NULL) {
        return WX_MSG_DISPATCH_RECVER_UNREG_MSG_ROUTER;
    }
    if (module->entryFunc == NULL) {
        return WX_MSG_DISPATCH_RECVER_UNREG_MSG_ROUTER;
    }
    return module->entryFunc(module,msg);
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

    WxEvtMsgHeader *msgHead = (WxEvtMsgHeader *)msg;
    msgHead->sender = sender;
    msgHead->receiver = receiver;
    WxMsgRouter *dstRouter = &g_wxMsgRouterList->routers[receiver];
    WxMsgRouter *srcRouter = &g_wxMsgRouterList->routers[sender];
    WxMsgSendMethod sendMethod = WX_MsgSchedule_CalcMsgSendMethod(srcRouter, dstRouter);
    switch (sendMethod) {
        case WX_MSG_SEND_TO_CORE:
            return WX_MsgShedule_ToCore(dstRouter, msg);
        case WX_MSG_SEND_TO_TASK:            
            return WX_MsgShedule_ToTask(dstRouter, msg);
        case WX_MSG_SEND_TO_MODULE:
            return WX_MsgShedule_ToModule(dstRouter, msg);
        default:
            return WX_MSG_DISPATCH_INVALID_SEND_METHOD;
    }
}