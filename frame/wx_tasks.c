
#include <wx_tasks.h>
#include "wx_include.h"
#include "FreeRTOS.h"
#include "task.h"
#include "wx_msg_common.h"
#include "wx_frame.h"
#include "wx_tasks.h"
#include "wx_modbus.h"
#ifdef __A_TEST__
#include "wx_test.h"
#endif
WxTasks *g_wxTasks = NULL;

/* this is template to generate a task */
WxTaskDeploy g_wxTaskDeployInfo[] = {
    {
        .taskName = "task_main",   
        .msgQueDepth = 8192,         
        .stackDepth = 1600,         
        .priority = 7,            
        .coreIdMask = WX_CORE_ID_0 + WX_CORE_ID_1 + WX_CORE_ID_2 + WX_CORE_ID_3, 
        .taskHanlder = WX_TaskHandle,
    },
    {
         .taskName = "task_om",
         .msgQueDepth = 8192,       
         .stackDepth = 1600,
         .priority = 6,
         .coreIdMask = WX_CORE_ID_0 + WX_CORE_ID_1 + WX_CORE_ID_2 + WX_CORE_ID_3, 
         .taskHanlder = WX_TaskHandle,
    },
    {
         .taskName = "task_driver",  
         .msgQueDepth = 8192,
         .stackDepth = 1600, 
         .priority = 8,
         .coreIdMask = WX_CORE_ID_0 + WX_CORE_ID_1 + WX_CORE_ID_2 + WX_CORE_ID_3,
         .taskHanlder = WX_TaskHandle,
    },
    #ifdef __A_TEST__
    {
         .taskName = "task_test",  
         .msgQueDepth = 0,
         .stackDepth = 1600, 
         .priority = 5,
         .coreIdMask = WX_CORE_ID_0 + WX_CORE_ID_1 + WX_CORE_ID_2 + WX_CORE_ID_3,
         .taskHanlder = WX_TestHandle,
    },
    #endif 
    // /* if more please add here */
};

UINT32 WX_ProcTaskMsg(WxTask *task, WxMsg *evtMsg)
{
	wx_debug("task(%s) receive msg(%u, %u)", task->taskName, evtMsg->msgType, evtMsg->subMsgType);
    UINT32 reciver = evtMsg->receiver;
    if (!WX_IsValidModuleId(reciver)) {
        return WX_INVALID_MODULE_ID ;
    }

    if (task->modules[reciver].entryFunc == NULL) {
        return WX_ERR_NO_ENTRY_FUNC;
    }

    return task->modules[reciver].entryFunc(&task->modules[reciver], evtMsg);
}

VOID WX_TaskHandle(VOID *param)
{
    WxTask *task = (WxTask *)param;
    boot_debug("Task(%s) start...", task->taskName);
    UINT32 ret;
    WxMsg *evtMsg = NULL;
    for (;;) {
        if (xQueueReceive(task->msgQueHandle, &evtMsg, portMAX_DELAY) == pdPASS) {
            ret = WX_ProcTaskMsg(task, evtMsg);
            if (ret != WX_SUCCESS) {
                wx_fail_code_cnt(ret);
            }
            WX_FreeEvtMsg(&evtMsg);
        }
    }
}

WxTasks *WX_CreateTasks(UINT8 coreId, UINT32 taskNum)
{
    if (g_wxTasks == NULL) {
        g_wxTasks = WX_Mem_Alloc("WxTasks", 1, sizeof(WxTasks) + taskNum * sizeof(WxTask));
        if (g_wxTasks == NULL) {
            return g_wxTasks;
        }
        
    }
    g_wxTasks->taskNum = 0;
    g_wxTasks->maxTaskNum = taskNum;
    return g_wxTasks;
}

UINT32 WX_DestroyDeployTasks(VOID)
{
    if (g_wxTasks!= NULL) {
        WX_Mem_Free(g_wxTasks);
        g_wxTasks = NULL;
    }
    return WX_SUCCESS;
}

WxTask *WX_QueryTask(CHAR *taskName)
{
    WxTask *task = NULL;
    for (UINT32 i = 0; i < g_wxTasks->taskNum; i++) {
        if (strcmp(g_wxTasks->taskList[i].taskName, taskName) == 0) {
            task = &g_wxTasks->taskList[i];
            break;
        }
    }
    return task;
}

UINT32 WX_DeployTasks_DeployTask(WxTask *task, WxTaskDeploy *taskDeploy)
{
    WX_CLEAR_OBJ(task);
    task->taskName = taskDeploy->taskName;
    boot_debug("Deploy task(%s) start...", taskDeploy->taskName);

    if (taskDeploy->msgQueDepth > 0) {
        task->msgQueHandle = xQueueCreate(taskDeploy->msgQueDepth, sizeof(WxMsg *));
        if (task->msgQueHandle == NULL) {
            return WX_CREATE_MSG_QUE_FAIL;
        }
    }
    if (taskDeploy->taskHanlder == NULL) {
        return WX_INVALID_TASK_HANDLER;
    }

    if (taskDeploy->stackDepth > 0) {
        BaseType_t xReturned = xTaskCreate(taskDeploy->taskHanlder, taskDeploy->taskName, taskDeploy->stackDepth, 
            (VOID *) task, taskDeploy->priority, &task->handle);
        if (xReturned != pdPASS) {
            boot_debug("Error Exit: task(%s) create fail!", taskDeploy->taskName);
            return WX_TASK_CREATE_FAIL;
        }
    }
    boot_debug("Deploy task(%s) success!", taskDeploy->taskName);
    return WX_SUCCESS;   
}

UINT32 WX_DeployTasks(UINT8 curCoreId)
{
    boot_debug("Delploy tasks on core[%u] start...", curCoreId);
    UINT32 taskDeployNum = sizeof(g_wxTaskDeployInfo) / sizeof(g_wxTaskDeployInfo[0]);
    WxTasks *taskList = WX_CreateTasks(curCoreId, taskDeployNum);
    WxTaskDeploy *taskDeploy = NULL;
    UINT32 ret = WX_SUCCESS;
    for (UINT32 i = 0; i < taskDeployNum; i++) {
        taskDeploy = &g_wxTaskDeployInfo[i];
        if ((taskDeploy->coreIdMask & curCoreId) == 0) {
            continue;
        }

        ret = WX_DeployTasks_DeployTask(&taskList->taskList[taskList->taskNum], taskDeploy);
        if (ret != WX_SUCCESS) {
            return ret;
        }
        taskList->taskNum++;
    }
    boot_debug("Deploy tasks(%u) on core[%u] success!", taskList->taskNum, curCoreId);
    return WX_SUCCESS;
}
