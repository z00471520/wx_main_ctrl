
#include "wx_deploy_task_funcode.h"

/* 任务处理消息 */
UINT32 WX_Deply_TaskFuncCodeProcMsg(WxTask *task, WxEvtMsg *evtMsg)
{
    UINT32 reciver = evtMsg->msgHead.receiver;
    if (!WX_IsValidModuleId(reciver)) {
        return WX_ERR;
    }
    
    if (task->modules[reciver].entryFunc == NULL) {
        return WX_ERR;
    }

    return task->modules[reciver].entryFunc(&task->modules[reciver], evtMsg);
}

/* 普通任务均运行该函数 */
VOID WX_Deply_TaskFuncCode(VOID *param)
{
    WxTask *task = param;
    UINT32 ret;
    WxEvtMsg *evtMsg = NULL;
    for (;;) {
        /* 当前任务等待外界发送的消息 */
        if (xQueueReceive(task->msgQueHandle, &evtMsg, portMAX_DELAY) == pdPASS) {
            ret = WX_Deply_TaskFuncCodeProcMsg(task, evtMsg);
            if (ret != WX_SUCCESS) {
                wx_fail_code_cnt(ret);
            }
            WX_FreeEvtMsg(&evtMsg);
        }
    }
}