#ifndef __WX_DEPLOY_TASKS_H__
#define __WX_DEPLOY_TASKS_H__
#include "wx_include.h"
/* 任务配置信息 */
typedef struct tagWxTaskDeployInfo{
    CHAR *taskName;            /* 任务的描述性名称。此参数主要用于方便调试，但也可用于 获取任务句柄。
                            任务名称的最大长度 由 FreeRTOSConfig.h 中的 configMAX_TASK_NAME_LEN 定义。 */
    UINT32 msgQueDepth; /* 消息队列的深度 */
    UINT32 stackDepth;  /* 要分配用作任务堆栈的字数（不是字节数！）。
                            例如，如果 堆栈宽度为 16 位，uxStackDepth 为 100，则将分配 200 字节用作任务 堆栈。
                            再举一例，如果堆栈宽度为 32 位，uxStackDepth 为 400， 则将分配 1600 字节用作任务堆栈。
                            堆栈深度与堆栈宽度的乘积不得超过 size_t 类型变量所能包含的最大值。*/
    UINT16 priority; /* 创建的任务将以该指定优先级执行。 tskIDLE_PRIORITY */
    UINT16 coreIdMask;  /* 任务部署的CoreId 掩码，指定Bit置位表示部署到该核*/
   
} WxTaskDeploy;

typedef struct tagWxTask {
    CHAR *taskName; /* 任务名字 */
    TaskHandle_t handle; /* 任务Handle */
    QueueHandle_t msgQueHandle; /* 消息队列的handle WxMsg */
    UINT32 msgQueFullCnt; /* 消息队列满次数 */
    UINT32 resv; /* 保留 */
    WxModule modules[WX_MODULE_BUTT]; /* 该任务部署的模块信息 */
} WxTask;

typedef struct tagWxDeployTasks {
    UINT32 maxTaskNum;  /* 最大任务数量 */
    UINT32 taskNum;     /* 当前任务数量 */
    WxTask taskList[0]; /* 任务列表 */
} WxDeployTasks;
WxTask *WX_DeployTasks_QueryTask(CHAR *taskName);
UINT32 WX_DeployTasks(UINT8 coreId);
#endif
