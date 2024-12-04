 
#include <wx_tasks.h>
#include "../frame/wx_msg_schedule.h"

#include "wx_failcode.h"
#include "wx_msg_common.h"
#include "wx_include.h"
#include "wx_frame.h"
WxModuleRouterList *g_wxModuleRouterList = NULL;

UINT32 WX_CreateModuleRouterList(VOID)
{
    if (g_wxModuleRouterList == NULL) {
        g_wxModuleRouterList = (WxModuleRouterList *)WX_Mem_Alloc(WxModuleRouterList, 1, sizeof(WxModuleRouterList));
        if (g_wxModuleRouterList == NULL) {
            boot_debug("Error Exit: memory allocation failed");
            return WX_MEM_ALLOC_FAIL;
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

/* shedule msg to core */
UINT32 WX_SheduleMsgToCore(WxModuleRouter *receiver, VOID *msg)
{
    /* TODO */
    return WX_SHEDULE_TO_CORE_NOT_SPT;
}

/* shedule msg to task */
UINT32 WX_SheduleMsgToTask(WxModuleRouter *receiver, VOID *msg)
{
    WxTask *task = receiver->belongTask;
    if (task == NULL) {
        wx_excp_cnt(WX_EXCP_TYPE_TASK_HANDLE_NULL);
        return WX_MSG_DISPATCH_NO_TASK_HANDLE;
    }
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


/* shedule msg */
UINT32 WX_MsgShedule(UINT8 sender, UINT8 receiver, VOID *msg)
{
	UINT32 ret;
    if (msg == NULL) {
        wx_excp_cnt(WX_EXCP_TYPE_MSG_NULL);
        return WX_MSG_DISPATCH_INVALID_MSG_HEADER;
    }

    if (g_wxModuleRouterList == NULL) {
        wx_excp_cnt(WX_EXCP_TYPE_MSG_ROUTER_LIST_NULL);
        return WX_MSG_DISPATCH_NO_MSG_ROUTER_LIST;
    }

    if (WX_IsValidModuleId(receiver) == FALSE) {
        wx_excp_cnt(WX_EXCP_TYPE_MSG_INVALID_RECVER);
        return WX_MSG_DISPATCH_INVALID_RECVER;
    }

    WxModuleRouter *dstRouter = &g_wxModuleRouterList->routers[receiver];
    /* the module's core ID must be valid  */
    if (dstRouter->coreId == 0) {
    	wx_excp_cnt(WX_EXCP_TYPE_MSG_ROUTER_UNREG);
    	return WX_MSG_SH_MODULE_NOT_REG_ROUTER;
    }
    /* if receiver is not in current core, shedule msg to core */
    if (WX_GetCurCoreId() != dstRouter->coreId) {
        ret = WX_SheduleMsgToCore(dstRouter, msg);
    } else {
        ret = WX_SheduleMsgToTask(dstRouter, msg);
    }

    return ret;
}
