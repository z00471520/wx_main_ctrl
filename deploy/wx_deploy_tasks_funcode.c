
#include "wx_deploy_tasks_funcode.h"
#include "wx_id_def.h"
#include "FreeRTOS.h"
#include "queue.h"
/* 浠诲姟澶勭悊娑堟伅 */
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

/* 鏅�氫换鍔″潎杩愯璇ュ嚱鏁� */
VOID WX_Deply_TaskFuncCode(VOID *param)
{
    WxTask *task = param;
    UINT32 ret;
    WxMsg *evtMsg = NULL;
    for (;;) {
        /* 褰撳墠浠诲姟绛夊緟澶栫晫鍙戦�佺殑娑堟伅 */
        if (xQueueReceive(task->msgQueHandle, &evtMsg, portMAX_DELAY) == pdPASS) {
            ret = WX_Deply_TaskFuncCodeProcMsg(task, evtMsg);
            if (ret != WX_SUCCESS) {
                wx_fail_code_cnt(ret);
            }
            WX_FreeEvtMsg(&evtMsg);
        }
    }
}
