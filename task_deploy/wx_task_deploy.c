
#include "wx_task.h"
#include "wx_include.h"
#include "wx_task_deploy.h"
/* 主控板模块定义 */
WxModuleDef g_wxModuleInfo[] = {
    /* 模块名(小写)   模块ID */
    {"module_can_slave_a",       WX_MODULE_ID_CAN_SLAVE_A},
    {"module_can_slave_b",       WX_MODULE_ID_CAN_SLAVE_B},
};
/* 模块部署信息 */
WxModuleDeploy g_wxModuleDepolyInfos[] = {
    /* 模块名        模块运行的核                                                    模块运行的任务名，     模块构建函数 */
    {"module_can_slave_a", WX_CORE(WX_CORE_0, WX_CORE_NA, WX_CORE_NA, WX_CORE_NA), "task_main", WX_CAN_SLAVE_ProcCanFrameMsg},
    {"module_can_slave_b", WX_CORE(WX_CORE_0, WX_CORE_NA, WX_CORE_NA, WX_CORE_NA), "task_main", WX_CAN_SLAVE_ProcCanFrameMsg},
};

/* 任务配置信息, 规定各核运行的任务 */
WxTaskDeploy g_wxTaskDeployInfo[] = {
    {
        .taskName = "task_main",    /* 主任务 */
        .stackDepth = 1600,         /* 任务的堆栈大小，详见说明 */
        .priority = 7,              /* 任务优先级 */
        .coreIdMask = WX_CORE_0 + WX_CORE_1 + WX_CORE_2 + WX_CORE_3, /* 任务部署到哪些CORE */
    },
    // {
    //     .taskName = "task_om",    /* 操作维护任务 */
    //     .stackDepth = 1600, /* 任务的堆栈大小，详见说明 */
    //     .priority = 5, /* 任务优先级 */
    //     .coreIdMask = WX_CORE_0 + WX_CORE_1 + WX_CORE_2 + WX_CORE_3, /* 任务部署到哪些CORE */
    // },
    /* if more please add here */
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

/* 根据消息队列handle的确定对应的消息处理入口函数, 一个消息队列只能被仅且一个模块处理 */
WxModuleProcMsgFunc WX_GetMsgProcFunc(WxTaskInfo *this, QueueSetMemberHandle_t msgQueHandle)
{
    /* 遍历映射关系 */
    WxTaskQueSetInfo *queSet = &this->queSet;
    for (UINT32 i = 0; i < queSet->msgQueNum;i++) {
        if (queSet->msgQue[i]->handle == msgQueHandle) {
            return queSet->msgQue[i]->msgHandle;
        }
    }

    return NULL;
}

/* 把消息队列添加到模块，参数合法性由调用者保证 */
UINT32 WX_RegMsgQue2Module(WxModuleInfo *module, WxMsgQueRegReq *req)
{   
    /* 如果模块消息队列个数则不准再注册 */
    if (module->msgQueNum >= WX_MODULE_MAX_QUE_NUM) {
        return WX_ERR;
    }
    if (req->msgHandle == NULL) {
        return WX_ERR;
    }
    WxMsgQue *newQue = &module->msgQue[module->msgQueNum];

    newQue->handle = xQueueCreate((UBaseType_t)req->itemNum, (UBaseType_t)req->itemSize);
    if (newQue->handle == NULL) {
        wx_critical(X, "Error Exit: create msg que(%s) fail", req->desc);
        return WX_ERR;
    }
    newQue->desc = req->desc;
    newQue->itemNum = req->itemNum;
    newQue->itemSize = req->itemSize;
    newQue->param = req->param;
    newQue->msgHandle = req->msgHandle;
    module->msgQueNum++;
    return WX_SUCCESS;
}

/* 所有任务均运行该函数 */
VOID WX_TaskFuncCode(VOID *param)
{
    WxTaskInfo *this = param;
    QueueSetMemberHandle_t activeMem;
    WxModuleProcMsgFunc msgProcFunc = NULL;
    WxEntryPara para = {0};
    UINT32 ret;
    for (;;) {
        /* 等待外界发送的消息 */
        para->msgQueHandle = xQueueSelectFromSet(this->queSet.handle, portMAX_DELAY);
        if (para->msgQueHandle == NULL) {
            continue;
        }

        /* 读出消息 */
        MSG = 
        /* 获取消息队列对应的处理入口函数 */
        msgProcFunc = WX_GetMsgProcFunc(para->msgQueHandle);
        if (msgProcFunc == NULL) {
            /* 一般不能出，除了属于异常，看看注册表是不是写的有问题 */
            wx_excp_cnt(WX_EXCP_CORE_TASK_ENTRY_UNFOUND);
            continue;
        }
        
        /* 调用消息处理入口函数处理该消息队列 */
        ret = msgProcFunc(para, &MSG);
        if (ret != WX_SUCCESS) {
            wx_fail_code_cnt(ret);
        }
    }

}

UINT32 WX_InitTaskModule(WxTaskInfo *task, WxModuleInfo *module, WxModuleDeploy *moduleDeploy)
{
    module->moduleName = moduleDeploy->moduleName;
    module->coreId = task->coreId;
    module->destructFunc = moduleDeploy->destructFunc;
    module->entryFunc    = moduleDeploy->entryFunc;
    if (moduleDeploy->construcFunc == NULL) {
        return WX_ERR;
    }
    UINT32 ret = moduleDeploy->construcFunc(module);
    if (ret != WX_SUCCESS) {
        return ret;
    }
    return WX_SUCCESS;
}

UINT32 WX_InitTaskModules(WxTaskInfo *task)
{
    /* 初始化所有的模块 */
    task->moduleNum = 0;
    WxModuleInfo *module = NULL;
    /* 根据模块部署 */
    WxModuleDeploy *moduleDeploy = NULL;
    for (UINT32 i = 0; i < sizeof(g_wxModuleDepolyInfos)/g_wxModuleDepolyInfos[0]; i++) {
        moduleDeploy = &g_wxModuleDepolyInfos[i];
        /* 如果当前模块没有部署在该任务则不处理 */
        if (strcmp(moduleDeploy->taskName, task->taskName) != 0) {
            continue;
        }
        /* 如果核ID不相等，则说明当前核任务不包含该模块 */
        if ((moduleDeploy->coreIdMask & task->coreId) == 0) {
            continue;
        }
        if (task->moduleNum >= WX_TASK_MAX_MODULE_NUM) {
            return WX_ERR;
        }
        module = &task->modules[task->moduleNum];
        UINT32 ret = WX_InitTaskModule(task, module, moduleDeploy);
        if (ret != WX_SUCCESS) {
            wx_critical(X, "Error Exit: WX_InitTaskModule(%u) fail(%u)", moduleDeploy->moduleName, ret);
            return ret;
        }

        if (module->msgQueNum == 0) {
            wx_warning(X, "Warning: the module(%s) do not give the msgque", moduleDeploy->moduleName);
        }
    }

    return WX_SUCCESS;
}

UINT32 WX_InitTaskQueSet(WxTaskInfo *task)
{
    WxTaskQueSetInfo *queSet = &task->queSet;
    queSet->evtQueLen = 0;
    WxMsgQue *msgQue = NULL; 
    /* 把所有模块的MsgQue搬移到Task下面, 方便查找 */
    for (UINT32 i = 0; i < task->moduleNum; i++) {
        for (UINT32 j = 0; j < task->modules[i].msgQueNum; i++) {
            msgQue = &task->modules[i].msgQue;
            /* 把该任务的消息队列指针存放起来 */
            if (queSet->msgQueNum >= WX_TASK_MAX_QUE_NUM) {
                return WX_ERR;
            }
            queSet->msgQue[queSet->msgQueNum++] = msgQue;
            queSet->evtQueLen += msgQue->itemNum;
        }
    }
    if (queSet->evtQueLen == 0) {
        return WX_ERR;
    }

    /* 创建队列集 */
    queSet->handle = xQueueCreateSet((const UBaseType_t)queSet->evtQueLen);
    if (queSet->handle == NULL) {
        return WX_ERR;
    }
    return WX_SUCCESS;
}

/* 部署一个任务 */
UINT32 WX_InitTask(WxTaskDeploy *taskDeploy, UINT32 coreId)
{
    UINT32 ret;
    WxTaskInfo *task = WX_Mem_Alloc(taskDeploy->taskName, 1, sizeof(WxTaskInfo));
    if (task == NULL) {
        return WX_ERR;
    }

    WX_CLEAR_OBJ(task);
    task->taskName = taskDeploy->taskName;
    task->coreId = coreId;
    ret = WX_InitTaskModules(task);
    if (ret != WX_SUCCESS) {
        WX_Mem_Free(task);
        return ret;
    }

    /* 初始化任务的消息集 */
    ret = WX_InitTaskQueSet(task);
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
UINT32 WX_InitTasks(UINT32 coreId)
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

        ret = WX_InitTask(taskDeploy, coreId);
        if (ret != WX_SUCCESS) {
            return ret;
        }
    }

    return WX_SUCCESS;
}
