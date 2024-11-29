
#include "wx_deploy_tasks_funcode.h"
#include "wx_id_def.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "wx_deploy_tasks.h"
#include "wx_msg_res_pool.h"
#include "wx_deploy.h"
/* 任务处理消息 */
UINT32 WX_Deply_TaskFuncCodeProcMsg(WxTask *task, WxMsg *evtMsg)
{
    UINT32 reciver = evtMsg->receiver;
    if (!WX_IsValidModuleId(reciver)) {
        return WX_ERR;
    }
    
    if (task->modules[reciver].entryFunc == NULL) {
        return WX_ERR;
    }

    return task->modules[reciver].entryFunc(&task->modules[reciver], evtMsg);
}

/* 这是一个通用的任务处理函数 */
VOID WX_Deply_TaskFuncCode(VOID *param)
{
    WxTask *task = param;
    UINT32 ret;
    WxMsg *evtMsg = NULL;
    for (;;) {
        /* 在这里阻塞等待消息的到来 */
        if (xQueueReceive(task->msgQueHandle, &evtMsg, portMAX_DELAY) == pdPASS) {
            ret = WX_Deply_TaskFuncCodeProcMsg(task, evtMsg);
            if (ret != WX_SUCCESS) {
                wx_fail_code_cnt(ret);
            }
            WX_FreeEvtMsg(&evtMsg);
        }
    }
}
