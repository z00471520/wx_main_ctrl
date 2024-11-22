#ifndef __WX_TASK_DEPLOY_H__
#define __WX_TASK_DEPLOY_H__
#define TASK_EVT_MSG_NODE_NUM       8196 /* 一个任务的消息队列支持的消息节点数 */
/* 用于定义启动哪些core，重复的按照一个，最多支持4个， 取值使用 WxCoreId */
#define WX_SPT_PROC_MSG_TYPE_MAX_NUM 8 /* 可以根据需求拓展 */
#define WX_TASK_SPT_MSG_TYPE_NUM     128

/* 核ID定义 */
typedef enum {
    WX_CORE_NA = 0,
    WX_CORE_0 = 0x1,
    WX_CORE_1 = 0x2,
    WX_CORE_2 = 0x4,
    WX_CORE_3 = 0x8,
} WxCoreId;

/* 模块构建函数 */
typedef UINT32 (*WxModuleConstructFunc)(VOID *module);
typedef UINT32 (*WxModuleDestructFunc)(VOID *module);
typedef UINT32 (*WxModuleEntryFunc)(VOID *module, WxEvtMsg *evtMsg);

/* 模块部署信息 */
typedef struct {
    const CHAR *moduleName;   /* 模块名 */
    WxModuleId moduleId;      /* 模块ID */
    UINT32 coreIdMask;        /* 模块部署的核ID， 使用 WX_CORE定义 */
    const CHAR *taskName;     /* 模块运行的任务ID */
    WxModuleConstructFunc   constructFunc; /* 模块构建函数 */
    WxModuleDestructFunc    destructFunc;  /* 模块的析构函数 */
    WxModuleEntryFunc       entryFunc;     /* 模块消息处理函数 */
} WxModuleDeploy;

typedef struct {
    CHAR *moduleName;           /* 模块名 */
    WxModuleId moduleId;        /* 模块ID */
} WxModuleDef;

typedef struct tagWxTaskInfo {
    CHAR *taskName; /* 任务名字 */
    UINT8 coreId; /* 任务所属的核ID */
    UINT8 resv;
    UINT32 reserv;
    TaskHandle_t handle; /* 任务Handle */
    QueueHandle_t msgQueHandle; /* 消息队列的handle WxEvtMsg */
    WxModule modules[WX_MODULE_BUTT]; /* 该任务部署的模块信息 */
} WxTaskInfo;

/* 任务配置信息 */
typedef struct tagWxTaskDeployInfo{
    CHAR *taskName;            /* 任务的描述性名称。此参数主要用于方便调试，但也可用于 获取任务句柄。
                            任务名称的最大长度 由 FreeRTOSConfig.h 中的 configMAX_TASK_NAME_LEN 定义。 */
    UINT32 stackDepth;  /* 要分配用作任务堆栈的字数（不是字节数！）。
                            例如，如果 堆栈宽度为 16 位，uxStackDepth 为 100，则将分配 200 字节用作任务 堆栈。
                            再举一例，如果堆栈宽度为 32 位，uxStackDepth 为 400， 则将分配 1600 字节用作任务堆栈。
                            堆栈深度与堆栈宽度的乘积不得超过 size_t 类型变量所能包含的最大值。*/
   
    UBaseType_t priority; /* 创建的任务将以该指定优先级执行。 tskIDLE_PRIORITY */
    UINT16 coreIdMask;  /* 任务部署的CoreId 掩码，指定Bit置位表示部署到该核*/
} WxTaskDeploy;

/* 卫星模块信息 */
typedef struct {
    CHAR *moduleName;                   /* 模块名 */
    UINT8 coreId;                       /* module所属当前核ID, 取值详见： WxCoreId */
    WxModuleId moduleId;                /* 模块ID */
    VOID *moduleInfo;                   /* 模块自定义的模块信息，由模块构建者自行定义内容 */
    WxTaskInfo *belongTask;             /* 模块所属的任务 */
    WxModuleConstructFunc   constructFunc; /* 模块构建函数 */
    WxModuleDestructFunc    destructFunc;  /* 模块的析构函数 */
    WxModuleEntryFunc       entryFunc;     /* 模块消息处理函数 */
} WxModule;

#define WX_GetModuleName(m)         (((WxModule *)m)->moduleName)
#define WX_GetModuleInfo(m)         (((WxModule *)m)->moduleInfo)
#define WX_SetModuleInfo(m, v)      (((WxModule *)m)->moduleInfo = (v))
#define WX_GetModuleCoreId(m)       (((WxModule *)m)->coreId)

VOID WX_TaskFuncCode(VOID *param);
UINT32 WX_DeployTasks(WxCoreId coreId);
#endif