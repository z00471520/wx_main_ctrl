 
#include "wx_failcode.h"
#include "wx_msg_common.h"
#include "wx_deploy_tasks.h"
#include "wx_include.h"
#include "wx_deploy.h"
WxModuleRouterList *g_wxModuleRouterList = NULL;

/* 创建消息调度的路由表 */
UINT32 WX_CreateModuleRouterList(void)
{
    if (g_wxModuleRouterList == NULL) {
        g_wxModuleRouterList = (WxModuleRouterList *)WX_Mem_Alloc(WxModuleRouterList, 1, sizeof(WxModuleRouterList));
        if (g_wxModuleRouterList == NULL) {
            return WX_FAIL_MEM_ALLOC;
        }
        memset(g_wxModuleRouterList, 0, sizeof(WxModuleRouterList));
    }
    g_wxModuleRouterList->coreId = WX_GetCurCoreId();
    return WX_SUCCESS;
}

UINT32 WX_DestroyModuleRouterList(void)
{
    if (g_wxModuleRouterList == NULL) {
        return WX_SUCCESS;
    }
    WX_Mem_Free(g_wxModuleRouterList);
    g_wxModuleRouterList = NULL;
    return WX_SUCCESS;
}

/* 注册模块的调度路由 */
UINT32 WX_RegModuleRouter(WxModuleId moduleId, UINT8 coreId, 
    WxTask *belongTask, WxModule *belongModule)
{
    if (g_wxModuleRouterList == NULL) {
        return WX_MSG_REG_NO_MSG_ROUTER_LIST;
    }
    if (!WX_IsValidModuleId(moduleId)) {
        return WX_MSG_REG_INVALID_MODULE_ID;
    }
    WxModuleRouter *this = &g_wxModuleRouterList->routers[moduleId];
    this->coreId = coreId;
    this->belongTask = belongTask;
    this->belongModule = belongModule;
    return WX_SUCCESS;
}

UINT32 WX_CheckSheduleReq(UINT8 sender, UINT8 receiver, VOID *msg)
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
    if (g_wxModuleRouterList == NULL) {
        return WX_MSG_DISPATCH_NO_MSG_ROUTER_LIST;
    }
    return WX_SUCCESS;
}
/* 计算消息的发送方式 */
WxMsgSendMethod WX_MsgSchedule_CalcMsgSendMethod(WxModuleRouter *sender, WxModuleRouter *receiver)
{
    /* 核不同不能直接发送消息 */
    if (receiver->coreId != sender->coreId) {
        return WX_MSG_SEND_TO_CORE;
    } else {
        return WX_MSG_SEND_TO_TASK;
    }
}

/* 发送消息到其他核 TODO*/
UINT32 WX_SheduleMsgToCore(WxModuleRouter *receiver, VOID *msg)
{
    /* TODO */
    return WX_ERR;
}

/* 发送消息到任务 */
UINT32 WX_SheduleMsgToTask(WxModuleRouter *receiver, VOID *msg)
{
    WxTask *task = receiver->belongTask;
    WxMsg *msgHead = (WxMsg *)msg;
    UINT32 sendResult;
    if (msgHead->isFromISR) {
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
    UINT32 ret = WX_CheckSheduleReq(sender, receiver, msg);
    if (ret != WX_SUCCESS) {
        return ret;
    }

    WxMsg *msgHead = (WxMsg *)msg;
    msgHead->sender = sender;
    msgHead->receiver = receiver;
    WxModuleRouter *dstRouter = &g_wxModuleRouterList->routers[receiver];
    WxModuleRouter *srcRouter = &g_wxModuleRouterList->routers[sender];
    WxMsgSendMethod sendMethod = WX_MsgSchedule_CalcMsgSendMethod(srcRouter, dstRouter);
    switch (sendMethod) {
        case WX_MSG_SEND_TO_CORE: {
            return WX_SheduleMsgToCore(dstRouter, msg);
        }
        case WX_MSG_SEND_TO_TASK: {
            ret = WX_SheduleMsgToTask(dstRouter, msg);
            return ret;
        }         
        default:
            return WX_MSG_DISPATCH_INVALID_SEND_METHOD;
    }
}
