
#include "wx_task.h"
#include "wx_include.h"
#include "wx_task_deploy.h"
#include "wx_msg_intf.h"
#include "wx_evt_msg_res_pool.h"

/* 主控板模块定义 */
WxModuleDef g_wxModuleInfo[] = {
    /* 模块名(小写)   模块ID */
    {"module_can_slave",        WX_MODULE_ID_CAN_SLAVE},
};
/* 模块部署信息 */
WxModuleDeploy g_wxModuleDepolyInfos[] = {
    {
        "module_can_slave",     /* 模块名 g_wxModuleInfo */
        WX_CORE_0,              /* 模块运行的核  */
        "task_main",            /* 模块运行的核内任务名 */
        WX_CAN_SLAVE_Constuct, /* 模块构建函数-必选 */
        WX_CAN_SLAVE_Destruct,   /* 模块析构函数 */
        WX_CAN_SLAVE_Entry,     /* 模块消息 */
        {WX_MSG_TYPE_CAN_FRAME}, /* 支持的消息类型 */
    },
};

/* 任务配置信息, 规定各核运行的任务 */
WxTaskDeploy g_wxTaskDeployInfo[] = {
    {
        .taskName = "task_main",    /* 主任务 */
        .stackDepth = 1600,         /* 任务的堆栈大小，详见说明 */
        .priority = 7,              /* 任务优先级 */
        .coreIdMask = WX_CORE_0 + WX_CORE_1 + WX_CORE_2 + WX_CORE_3, /* 任务部署到哪些CORE */
    },
    {
         .taskName = "task_om",    /* 操作维护任务 */
         .stackDepth = 1600, /* 任务的堆栈大小，详见说明 */
         .priority = 5, /* 任务优先级 */
         .coreIdMask = WX_CORE_0 + WX_CORE_1 + WX_CORE_2 + WX_CORE_3, /* 任务部署到哪些CORE */
    },
    // /* if more please add here */
};

UINT32 WX_GetTaskDeployNum(VOID)
{
    return sizeof(g_wxTaskDeployInfo) / sizeof(g_wxTaskDeployInfo[0]);
}

/* 获取第I个部署，参数合法性由调用者保证 */
WxTaskDeploy *WX_GetTaskDeploy(UINT32 i)
{
    return &g_wxTaskDeployInfo[i];
}

/* 把一个任务部署到模块 */
UINT32 WX_DeployOneModule2Task(WxTaskInfo *task, WxModuleInfo *module, WxModuleDeploy *moduleDeploy)
{
    module->moduleName = moduleDeploy->moduleName;
    module->coreId = task->coreId;
    module->constructFunc    = moduleDeploy->constructFunc;
    module->destructFunc     = moduleDeploy->destructFunc;
    module->entryFunc        = moduleDeploy->entryFunc;
    module->belongTask = task;
    
    /* 构建函数必须有效 */
    if (moduleDeploy->constructFunc == NULL) {
        return WX_ERR;
    }
    /* 根据构建函数创建该模块 */
    UINT32 ret = moduleDeploy->constructFunc(module);
    if (ret != WX_SUCCESS) {
        return ret;
    }
    /* 如果模块消息处理入口函数有效则把映射消息类型到模块. 需要在构建完毕后执行 */
    if (moduleDeploy->entryFunc) {
        WxMsgType msgType;
        for (UINT32 i = 0; i < WX_SPT_PROC_MSG_TYPE_MAX_NUM; i++) {
            msgType = moduleDeploy->moduleSptMsgType[i];
            if (WX_IsValidMsgType(msgType)) {
                task->msgType2Module[msgType] = module;
            }
        }
    }

    return WX_SUCCESS;
}

/* 部署一个模块所有的Module */
UINT32 WX_DeployModules2Task(WxTaskInfo *task)
{
    /* 初始化所有的模块 */
    task->moduleNum = 0;
    WxModuleInfo *module = NULL;
    /* 根据模块部署 */
    WxModuleDeploy *moduleDeploy = NULL;
    /* 遍历模块部署信息 */
    for (UINT32 i = 0; i < sizeof(g_wxModuleDepolyInfos) / g_wxModuleDepolyInfos[0]; i++) {
        moduleDeploy = &g_wxModuleDepolyInfos[i];
        /* 如果当前模块没有部署在该任务则不处理 */
        if (strcmp(moduleDeploy->taskName, task->taskName) != 0) {
            continue;
        }
        /* 如果核ID不相等，则说明当前核任务不包含该模块 */
        if ((moduleDeploy->coreIdMask & task->coreId) == 0) {
            continue;
        }
        /* 如果任务部署的模块超过了最大值则属于异常，修改宏取值即可 */
        if (task->moduleNum >= WX_TASK_SPT_MODULE_NUM) {
            return WX_ERR;
        }
        module = &task->modules[task->moduleNum];
        UINT32 ret = WX_DeployOneModule2Task(task, module, moduleDeploy);
        if (ret != WX_SUCCESS) {
            wx_critical(X, "Error Exit: WX_DeployOneModule2Task(%u) fail(%u)", moduleDeploy->moduleName, ret);
            return ret;
        }
    }

    return WX_SUCCESS;
}

/* 部署一个任务 */
UINT32 WX_DeployOneTask(WxTaskDeploy *taskDeploy, UINT8 coreId)
{
    /* 创建消息资源池 */
    UINT32 ret = WX_CreateEvtMsgResPool(TASK_EVT_MSG_NODE_NUM);
    if (ret != WX_SUCCESS) {
        return ret;
    } 

    WxTaskInfo *task = WX_Mem_Alloc(taskDeploy->taskName, 1, sizeof(WxTaskInfo));
    if (task == NULL) {
        return WX_ERR;
    }
   
    WX_CLEAR_OBJ(task);
    task->taskName = taskDeploy->taskName;
    task->coreId = coreId;
    task->msgQueHandle = xQueueCreate(taskDeploy->msgQueItemNum, sizeof(WxEvtMsg));
    if (task->msgQueHandle = NULL) {
        return WX_ERR;
    }

    ret = WX_DeployModules2Task();
    if (ret != WX_SUCCESS) {
        return ret;
    }

    /* 初始化任务主函数 */
    BaseType_t xReturned = xTaskCreate(WX_TaskFuncCode, taskDeploy->taskName, taskDeploy->stackDepth, 
        (VOID *) task, taskDeploy->priority, &task->handle);
    if (xReturned != pdPASS) {
        wx_critical(WX_EXCP_UNDEFINE, "Error Exit: core[%u] task(%s) create fail!", coreId, taskDeploy->taskName);
        return WX_TASK_CREATE_FAIL;
    }
    wx_log(WX_NOTICE, "core[%u] create task(%s) success!", coreId, taskDeploy->taskName);
    return WX_SUCCESS;   
}

/* 部署指定核的任务 */
UINT32 WX_DeployTasks(WxCoreId coreId)
{
    UINT32 ret;
    WxTaskDeploy *taskDeploy = NULL;
    UINT32 taskDeployNum = WX_GetTaskDeployNum();
    for (UINT32 i = 0; i < taskDeployNum; i++) {
        taskDeploy = WX_GetTaskDeploy(i);
        /* 如果当前任务不需要部署到该核该不处理 */
        if ((taskDeploy->coreIdMask & coreId) == 0) {
            continue;
        }

        ret = WX_DeployOneTask(taskDeploy, (UINT8)coreId);
        if (ret != WX_SUCCESS) {
            return ret;
        }
    }

    return WX_SUCCESS;
}

/* 任务处理消息 */
VOID WX_TaskProcEvtMsg(WxTaskInfo *task,  WxEvtMsg *evtMsg)
{
    /* 检查消息类型 */
    if (!WX_IsValidMsgType(evtMsg->msgHead.msgType)) {
        /* 异常计数 */
        continue;
    }
    WxModuleInfo *module = task->msgType2Module[evtMsg->msgHead.msgType];
    /* 当前消息类型没有模块处理，属于异常 */
    if (module == NULL) {
        /* 异常计数 */
        continue;
    }
    return module->entryFunc(module, evtMsg);
}

/* 所有任务均运行该函数 */
VOID WX_TaskFuncCode(VOID *param)
{
    WxTaskInfo *task = param;
    UINT32 ret;
    WxEvtMsg *evtMsg = NULL;
    for (;;) {
        /* 当前任务等待外界发送的消息 */
        if (xQueueReceive(task->msgQueHandle, &evtMsg, portMAX_DELAY) == pdPASS) {
            ret = WX_TaskProcEvtMsg(task, evtMsg);
            WX_FreeEvtMsg(&evtMsg);
            if (ret != WX_SUCCESS) {
                wx_fail_code_cnt(ret);
            } 
        }
    }
}