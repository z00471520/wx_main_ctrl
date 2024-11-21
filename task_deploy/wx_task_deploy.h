#ifndef __WX_TASK_DEPLOY_H__
#define __WX_TASK_DEPLOY_H__

typedef enum {
    /* 主控板上的模块ID */
    WX_MODULE_ID_RS422_I_SLAVE = 1,
    WX_MODULE_ID_RS422_I_MASTER = 2,
    WX_MODULE_ID_CAN_SLAVE_A = 3,
    WX_MODULE_ID_CAN_SLAVE_B = 4,
} WxModuleId;

/* 核ID定义 */
typedef enum {
    WX_CORE_NA = 0,
    WX_CORE_0 = 0x1,
    WX_CORE_1 = 0x2,
    WX_CORE_2 = 0x4,
    WX_CORE_3 = 0x8,
} WxCoreId;

#define WX_MODULE_MAX_QUE_NUM 8     /* 一个模块最多支持8个消息队列，暂定 */
#define WX_TASK_MAX_MODULE_NUM 64   /* 一个任务支持的最多模块数据 */
#define WX_TASK_MAX_QUE_NUM (WX_TASK_MAX_MODULE_NUM * WX_MODULE_MAX_QUE_NUM)     /* 一个任务队列集支持的最多消息队列 */
/* 用于定义启动哪些core，重复的按照一个，最多支持4个， 取值使用 WxCoreId */
#define WX_CORE(c0,c1,c2,c3) ((((UINT32)1) << (c0)) + (((UINT32)1) << (c1)) + (((UINT32)1) << (c2)) + (((UINT32)1) << (c3))) 

/* 模块部署信息 */
typedef struct {
    const CHAR *moduleName;  /* 模块名 */
    UINT8 coreIdMask;         /* 模块部署的核ID， 使用 WX_CORE定义 */
    const CHAR *taskName;     /* 模块运行的任务ID */
    WxModuleConstructFunc construcFunc; /* 模块构建函数 */
} WxModuleDeploy;

typedef struct {
    CHAR *moduleName;           /* 模块名 */
    WxModuleId moduleId;        /* 模块ID */
} WxModuleDef;

typedef struct tagWxTaskQueSetInfo {
    QueueSetHandle_t handle; /* 任务的消息队列集Handle */
    UINT32 evtQueLen; /* 任务的消息所有队列成员个数累加 */
    UINT32 msgQueNum; /* 消息队列的个数 */
    WxMsgQue *msgQue[WX_TASK_MAX_QUE_NUM]; /* 消息队列信息指针 */
} WxTaskQueSetInfo;

typedef struct tagWxTaskInfo {
    CHAR *taskName; /* 任务名字 */
    UINT8 coreId; /* 任务所属的核ID */
    UINT32 moduleNum; /* 任务模块数 */
    WxModuleInfo modules[WX_TASK_MAX_MODULE_NUM]; /* 任务模块列表 */
    TaskHandle_t handle; /* 任务Handle */
    WxTaskQueSetInfo queSet; /* 队列集信息 */
} WxTaskInfo;

/* 任务配置信息 */
typedef struct tagWxTaskDeployInfo{
    CHAR * taskName;            /* 任务的描述性名称。此参数主要用于方便调试，但也可用于 获取任务句柄。
                            任务名称的最大长度 由 FreeRTOSConfig.h 中的 configMAX_TASK_NAME_LEN 定义。 */
    UINT32 stackDepth;  /* 要分配用作任务堆栈的字数（不是字节数！）。
                            例如，如果 堆栈宽度为 16 位，uxStackDepth 为 100，则将分配 200 字节用作任务 堆栈。
                            再举一例，如果堆栈宽度为 32 位，uxStackDepth 为 400， 则将分配 1600 字节用作任务堆栈。
                            堆栈深度与堆栈宽度的乘积不得超过 size_t 类型变量所能包含的最大值。*/
   
    UBaseType_t priority; /* 创建的任务将以该指定优先级执行。 tskIDLE_PRIORITY */
    UINT16 coreIdMask;  /* 任务部署的CoreId 掩码，指定Bit置位表示部署到该核*/
} WxTaskDeploy;

typedef struct {
    QueueSetMemberHandle_t msgQueHandle; /* 消息获取 */
    /* if more please addr */
} WxMsgPara;

/* 模块消息处理入口参数 param为注册WxMsgQue时候指定的 */
typedef UINT32 (*WxModuleProcMsgFunc)(VOID *param, VOID *msg);

/* 卫星消息队列 */
typedef struct {
    CHAR *desc;     /* 消息队列描述 */
    QueueHandle_t handle; /* 模块某消息队列的handle */
    UINT32 itemNum;  /* 消息队列中Item的个数，用于队列集的创建 */
    UINT32 itemSize; /* 消息队列中Item的尺寸，Byte */
    WxModuleProcMsgFunc msgHandle;    /* 模块消息处理Handle */
    VOID *procMsgFuncParam; /* 消息队列非空时，WxModuleProcMsgFunc传递入param */
} WxMsgQue;

typedef struct {
    CHAR *desc;     /* 消息队列描述 */
    UINT32 itemNum;  /* 消息队列中Item的个数，用于队列集的创建 */
    UINT32 itemSize; /* 消息队列中Item的尺寸，Byte */
    WxModuleProcMsgFunc msgHandle;    /* 消息处理Handle */
    VOID *param; /* 消息处理处理Handle的param */
} WxMsgQueRegReq;

typedef struct {
    CHAR *moduleName;                   /* 模块名 */
    UINT8 coreId;                       /* module所属当前核ID, 取值详见： WxCoreId */
    WxModuleId moduleId;                /* 模块ID */
    UINT32 msgQueNum;                   /* 模块消息队列个数，由模块构建函数指定 */
    WxMsgQue msgQue[WX_MODULE_MAX_QUE_NUM]; /* 模块消息队列的信息，由模块构建函数指定 */
    VOID *selfDefBuff; /* 模块自定义的buff，由模块构建者常见，自行定义内容 */
} WxModuleInfo;

/* 模块构建函数 */
typedef UINT32 (*WxModuleConstructFunc)(WxModuleInfo *module);
/* 向当前模块注册消息，注册前必须创建OK(WX_CreateMsgQue) */
UINT32 WX_RegMsgQue2Module(WxModuleInfo *module, WxMsgQueRegReq *req);

#endif