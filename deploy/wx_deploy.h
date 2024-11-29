#ifndef __WX_DEPLOY_H__
#define __WX_DEPLOY_H__
#include "wx_include.h"
#include "wx_id_def.h"
 
UINT8 g_curCoreId;

#define INTC_DEVICE_ID		XPAR_SCUGIC_SINGLE_DEVICE_ID
/* 模块相关函数 */
typedef UINT32 (*WxModuleConstructFunc)(VOID *module);
typedef UINT32 (*WxModuleDestructFunc)(VOID *module);
typedef UINT32 (*WxModuleEntryFunc)(VOID *module, WxMsg *evtMsg);

typedef struct {
    CHAR *moduleName;   /* 模块名字 */
    WxModuleId moduleId;	/* 模块ID */
    WxCoreId coreId;	/* 模块部署的核ID */
    CHAR *taskName;     /* 模块部署的任务 */
    WxModuleConstructFunc constructFunc;    /* 模块构建函数 */
    WxModuleDestructFunc destructFunc;      /* 模块析构函数 */
    WxModuleEntryFunc entryFunc;            /* 模块消息处理函数 */
} WxModuleDeploy;

typedef struct tagWxTaskDeployInfo{
    CHAR *taskName;
    UINT32 msgQueDepth;
    UINT32 stackDepth;
    UINT16 priority;
    UINT16 coreIdMask;
} WxTaskDeploy;

/* 妯″潡瀹氫箟 */
typedef struct {
    CHAR *moduleName;                   /* 妯″潡鍚嶅瓧 */
    UINT8 coreId;                       /* 妯″潡閮ㄧ讲鐨勬牳ID,WxCoreId */
    WxModuleId moduleId;                /* 妯″潡ID */
    VOID *moduleInfo;                   /* 鑷畾涔夌殑妯″潡淇℃伅 */
    VOID *belongTask;             	    /* 妯″潡鎵�灞炵殑浠诲姟 */
    WxModuleConstructFunc   constructFunc; /* 妯″潡鐨勬瀯寤哄嚱鏁� */
    WxModuleDestructFunc    destructFunc;  /* 妯″潡鐨勬瀽鏋勫嚱鏁� */
    WxModuleEntryFunc       entryFunc;     /* 妯″潡鐨勫叆鍙ｅ嚱鏁� */
} WxModule;

typedef struct tagWxTask {
    CHAR *taskName;
    TaskHandle_t handle;
    QueueHandle_t msgQueHandle;
    UINT32 msgQueFullCnt;
    UINT8 coreId;
    UINT8 resv;
    UINT16 res16;
    WxModule modules[WX_MODULE_BUTT];
} WxTask;

typedef struct tagWxDeployTasks {
    UINT32 maxTaskNum;
    UINT32 taskNum;
    WxTask taskList[0];
} WxDeployTasks;

UINT32 WX_RegModuleRouter(WxModuleId moduleId, UINT8 coreId, 
    WxTask *belongTask, WxModule *belongModule);
/* 消息调度 */
UINT32 WX_MsgShedule(UINT8 sender, UINT8 receiver, VOID *msg);
XScuGic *WX_GetOrCreateScuGicInstance(VOID);
UINT32 WX_Deploy(UINT8 coreId);
#define WX_GetCurCoreId() g_curCoreId
#define WX_GetModuleName(m)         (((WxModule *)m)->moduleName)
#define WX_GetModuleInfo(m)         (((WxModule *)m)->moduleInfo)
#define WX_SetModuleInfo(m, v)      (((WxModule *)m)->moduleInfo = (v))
#define WX_GetModuleCoreId(m)       (((WxModule *)m)->coreId)
#define WX_GetModuleId(m)           (((WxModule *)m)->moduleId)
#endif
