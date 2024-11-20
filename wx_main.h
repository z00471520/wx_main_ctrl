
#ifndef WX_MAIN_CTRL_CPU0_H
#define WX_MAIN_CTRL_CPU0_H
#include "wx_include.h"

#define WX_MODULE_MSG_QUE_MAX_NUM 8 /* 一个模块最多支持8个消息队列 */

typedef struct {
    ;
} WxConstructPara;

typedef struct {
    ;
} WxDeconstructPara;

typedef struct {

} WxEntryPara; 

/* 模块构建函数 */
typedef UINT32 (*WxModuleConstructFunc)(WxConstructPara *para);
/* 模块析构函数 */
typedef UINT32 (*WxModuleDestructFunc)(WxDeconstructPara *para);
/* 模块消息处理入口参数 */
typedef UINT32 (*WxModuleEntryFunc)(WxEntryPara *para, QueueSetMemberHandle_t msgHandle);

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

/* 获取中断控制实例 */
INTC *WX_GetIntrCtrlInst(VOID);

#endif
