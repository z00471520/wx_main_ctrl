#include "wx_util.h"

/* 参数合法性由调用者保证 */
UINT32 WX_CreateTask(TaskHandle_t *taskHandlePtr, WxTaskDeploy *taskCfg)
{
        /* Create the task, storing the handle. */
    BaseType_t xReturned = xTaskCreate(taskCfg->taskCode, taskCfg->name, taskCfg->stackDepth, 
        (VOID *) taskCfg->param, taskCfg->priority, taskHandlePtr);
    if (xReturned != pdPASS) {
        wx_log(WX_CRITICAL, "Error Exit: task(%s) create fail!", taskCfg->name);
        return WX_TASK_CREATE_FAIL;
    }

    wx_log(WX_NOTICE, "task(%s) create success!", taskCfg->name);
    return WX_SUCCESS;   
}

