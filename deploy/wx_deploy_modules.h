#ifndef __WX_DEPLOY_MODULES_H__
#define __WX_DEPLOY_MODULES_H__
#include "wx_include.h"
#include "wx_id_def.h"

#define INTC_DEVICE_ID		XPAR_INTC_0_DEVICE_ID
/* 模块构建函数 */
typedef UINT32 (*WxModuleConstructFunc)(VOID *module);
typedef UINT32 (*WxModuleDestructFunc)(VOID *module);
typedef UINT32 (*WxModuleEntryFunc)(VOID *module, WxMsg *evtMsg);

/* 模块部署信息 */
typedef struct {
    const CHAR *moduleName;   /* 模块名 */
    WxModuleId moduleId;      /* 模块ID */
    WxCoreId coreId;          /* 模块部署的核ID，一个 模块仅能部署在一个核上 */
    const CHAR *taskName;     /* 模块运行的任务名字，如果是驱动模块则指定为NULL */
    WxModuleConstructFunc constructFunc;    /* 模块构建函数 */
    WxModuleDestructFunc destructFunc;      /* 模块的析构函数 */
    WxModuleEntryFunc entryFunc;            /* 模块消息处理函数 */
} WxModuleDeploy;

/* 卫星模块信息 */
typedef struct {
    CHAR *moduleName;                   /* 模块名 */
    UINT8 coreId;                       /* module所属当前核ID, 取值详见： WxCoreId */
    WxModuleId moduleId;                /* 模块ID */
    VOID *moduleInfo;                   /* 模块自定义的模块信息，由模块构建者自行定义内容 */
    WxTask *belongTask;             /* 模块所属的任务 */
    WxModuleConstructFunc   constructFunc; /* 模块构建函数 */
    WxModuleDestructFunc    destructFunc;  /* 模块的析构函数 */
    WxModuleEntryFunc       entryFunc;     /* 模块消息处理函数 */
} WxModule;

#define WX_GetModuleName(m)         (((WxModule *)m)->moduleName)
#define WX_GetModuleInfo(m)         (((WxModule *)m)->moduleInfo)
#define WX_SetModuleInfo(m, v)      (((WxModule *)m)->moduleInfo = (v))
#define WX_GetModuleCoreId(m)       (((WxModule *)m)->coreId)
#define WX_GetModuleId(m)           (((WxModule *)m)->moduleId)
#endif // __WX_DEPLOY_MODULE_H__