#ifndef __WX_DEPLOY_TASKS_H__
#define __WX_DEPLOY_TASKS_H__
#include "wx_include.h"
typedef struct tagWxTaskDeployInfo{
    CHAR *taskName;
    UINT32 msgQueDepth;
    UINT32 stackDepth;
    UINT16 priority;
    UINT16 coreIdMask;
   
} WxTaskDeploy;

typedef struct tagWxTask {
    CHAR *taskName;
    TaskHandle_t handle;
    QueueHandle_t msgQueHandle;
    UINT32 msgQueFullCnt;
    UINT32 resv;
    WxModule modules[WX_MODULE_BUTT];
} WxTask;

typedef struct tagWxDeployTasks {
    UINT32 maxTaskNum;
    UINT32 taskNum;
    WxTask taskList[0];
} WxDeployTasks;

WxTask *WX_DeployTasks_QueryTask(CHAR *taskName);
UINT32 WX_DeployTasks(UINT8 coreId);
#endif
