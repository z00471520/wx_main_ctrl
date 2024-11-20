#ifndef __WX_SHEDULE_FRAME_H__
#define __WX_SHEDULE_FRAME_H__

#define WX_MODULE_MSG_QUE_MAX_NUM 8 /* 一个模块最多支持8个消息队列 */
typedef enum {
    /* 主控板上的模块 */
    WX_MODULE_ID_RS422_I_SLAVE = 1,
    WX_MODULE_ID_RS422_I_MASTER = 2,
    WX_MODULE_ID_CAN_SLAVE_A = 3,
    WX_MODULE_ID_CAN_SLAVE_B = 4,
}WxModuleId;

typedef struct {
    CHAR *moduleName;
    WxModuleId moduleId;
} WxModuleInfo;

typedef struct {
    UINT8 resv;
} WxConstructPara;

typedef struct {
    UINT8 resv;
} WxDeconstructPara;

typedef struct {
    QueueSetMemberHandle_t msgQueHandle;
} WxEntryPara; 

/* 模块构建函数 */
typedef UINT32 (*WxModuleConstructFunc)(WxConstructPara *para);
/* 模块析构函数 */
typedef UINT32 (*WxModuleDestructFunc)(WxDeconstructPara *para);
/* 模块消息处理入口参数 */
typedef UINT32 (*WxModuleEntryFunc)(WxEntryPara *para);

/* 模块部署信息 */
typedef struct {
    const CHAR *moduleName;  /* 模块名 */
    UINT8 coreId;       /* 模块部署的核ID */
    const CHAR *taskName;     /* 模块运行的任务ID */
    WxModuleConstructFunc construcFunc; /* 模块构建函数 */
    WxModuleDestructFunc  destructFunc; /* 模块析构函数 */
    WxModuleEntryFunc     entryFunc;    /* 模块消息处理函数 */
    WxModuleDestructFunc  WX_MODULE_MSG_QUE_MAX_NUM 
} WxModuleDeployInfo;



#endif