#ifndef WX_TASK_H
#define WX_TASK_H
#include "wx_include.h"
#include "queue.h"
typedef struct {
    
};

typedef struct {
    
    UINT16 moduleId; /* 模块ID */
    UINT8 taskId;    /* 模块部署的任务ID */
    UINT8 coreId;    /* 模块部署的核ID */
    
    
    QueueSetMemberHandle_t moduleMsgHandle; /* 模块的消息队列或信号量的句柄 */

} WxCoreModuleDepolyInfo;

/* CORE模块部署配置，不同核可能部署不同的模块 */
typedef struct {
    UINT32 coreId;
    
} WxCoreDeployCfg;

typedef struct
{
    UINT32 coreId;
    QueueSetHandle_t queSet; /* 队列集 */      
} WxCore;

/* 初始化 */
UINT32 WX_CoreInit(UINT32 coreId);
/* wx_core主任务 */
VOID WX_CORE_MainFunc(VOID *para);
#endif
