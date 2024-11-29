
#include "wx_deploy_tasks.h"
#include "wx_include.h"
#include "FreeRTOS.h"
#include "task.h"
#include "wx_deploy_tasks_funcode.h"
#include "wx_msg_common.h"
WxDeployTasks *g_wxDeployTasks = NULL;     /* 任务列表 */

/* 任务配置信息, 规定各核运行的任务 */
WxTaskDeploy g_wxTaskDeployInfo[] = {
    {
        .taskName = "task_main",    /* 主任务 */
        .msgQueDepth = 8192,        /* 任务的消息队列深度 */
        .stackDepth = 1600,         /* 任务的堆栈大小，详见说明 */
        .priority = 7,              /* 任务优先级 */
        .coreIdMask = WX_CORE_ID_0 + WX_CORE_ID_1 + WX_CORE_ID_2 + WX_CORE_ID_3, /* 任务部署到哪些CORE */
    },
    {
         .taskName = "task_om",    /* 操作维护任务 */
         .msgQueDepth = 8192,      /* 任务的消息队列深度 */
         .stackDepth = 1600, /* 任务的堆栈大小，详见说明 */
         .priority = 5, /* 任务优先级 */
         .coreIdMask = WX_CORE_ID_0 + WX_CORE_ID_1 + WX_CORE_ID_2 + WX_CORE_ID_3, /* 任务部署到哪些CORE */
    },
    {
         .taskName = "task_driver",    /* 驱动任务 */
         .msgQueDepth = 8192,      /* 任务的消息队列深度为0 */
         .stackDepth = 1600, /* 任务的堆栈大小，详见说明 */
         .priority = 8, /* 任务优先级 */
         .coreIdMask = WX_CORE_ID_0 + WX_CORE_ID_1 + WX_CORE_ID_2 + WX_CORE_ID_3, /* 任务部署到哪些CORE */
    },
    // /* if more please add here */
};

WxDeployTasks *WX_DeployTasks_Create(UINT8 coreId, UINT32 taskNum)
{
    if (g_wxDeployTasks == NULL) {
        g_wxDeployTasks = WX_Mem_Alloc("WxDeployTasks", 1, sizeof(WxDeployTasks) + taskNum * sizeof(WxTask));
        if (g_wxDeployTasks == NULL) {
            return g_wxDeployTasks;
        }
        
    }
    g_wxDeployTasks->taskNum = 0;
    g_wxDeployTasks->maxTaskNum = taskNum;
    return g_wxDeployTasks;
}

UINT32 WX_DeployTasks_Destroy(VOID)
{
    if (g_wxDeployTasks!= NULL) {
        WX_Mem_Free(g_wxDeployTasks);
        g_wxDeployTasks = NULL;
    }
    return WX_SUCCESS;
}

/* 根据名字查找一个任务 */
WxTask *WX_DeployTasks_QueryTask(CHAR *taskName)
{
    WxTask *task = NULL;
    for (UINT32 i = 0; i < g_wxDeployTasks->taskNum; i++) {
        if (strcmp(g_wxDeployTasks->taskList[i].taskName, taskName) == 0) {
            task = &g_wxDeployTasks->taskList[i];
            break;
        }
    }
    return task;
}

/* 部署一个任务 */
UINT32 WX_DeployTasks_DeployTask(WxTask *task, WxTaskDeploy *taskDeploy)
{
    WX_CLEAR_OBJ(task);
    task->taskName = taskDeploy->taskName;

    /* 创建任务消息队列 */
    if (taskDeploy->msgQueDepth > 0) {
        task->msgQueHandle = xQueueCreate(taskDeploy->msgQueDepth, sizeof(WxMsg *));
        if (task->msgQueHandle == NULL) {
            return WX_CREATE_MSG_QUE_FAIL;
        }
    }

    /* 初始化任务主函数 */
    if (taskDeploy->stackDepth > 0) {
        BaseType_t xReturned = xTaskCreate(WX_Deply_TaskFuncCode, taskDeploy->taskName, taskDeploy->stackDepth, 
            (VOID *) task, taskDeploy->priority, &task->handle);
        if (xReturned != pdPASS) {
            wx_critical(WX_EXCP_UNDEFINE, "Error Exit: core[%u] task(%s) create fail!", coreId, taskDeploy->taskName);
            return WX_TASK_CREATE_FAIL;
        }
    }
    wx_log(WX_NOTICE, "Create task(%s) success!", taskDeploy->taskName);
    return WX_SUCCESS;   
}

/* 部署指定核的任务 */
UINT32 WX_DeployTasks(UINT8 curCoreId)
{
    /* 创建消息资源池 */
    UINT32 ret = WX_CreateMsgResPool();
    if (ret != WX_SUCCESS) {
        return ret;
    } 
    UINT32 taskDeployNum = sizeof(g_wxTaskDeployInfo) / sizeof(g_wxTaskDeployInfo[0]);
    WxDeployTasks *taskList = WX_DeployTasks_Create(curCoreId, taskDeployNum);
    WxTaskDeploy *taskDeploy = NULL;
    for (UINT32 i = 0; i < taskDeployNum; i++) {
        taskDeploy = &g_wxTaskDeployInfo[i];
        /* 如果当前任务不需要部署到该核该不处理 */
        if ((taskDeploy->coreIdMask & curCoreId) == 0) {
            continue;
        }

        ret = WX_DeployTasks_DeployTask(&taskList->taskList[taskList->taskNum], taskDeploy);
        if (ret != WX_SUCCESS) {
            return ret;
        }
        taskList->taskNum++;
    }

    return WX_SUCCESS;
}
