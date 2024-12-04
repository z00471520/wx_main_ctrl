#ifndef __WX_FRAME_H__
#define __WX_FRAME_H__
#include "wx_msg_common.h"
#include "wx_id_def.h"
#include "wx_include.h"
#include "wx_frame.h"
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
    UINT32 resv32;
    VOID (*taskHanlder)(VOID *param);
} WxTaskDeploy;

/* 妯″潡瀹氫箟 */
typedef struct {
    CHAR *moduleName;                   /* Module name */
    UINT8 coreId;                       /* Module core ID */
    UINT8 resv;
    UINT16 res16;
    WxModuleId moduleId;                /* Module ID */
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

typedef struct tagWxTasks {
    UINT32 maxTaskNum;
    UINT32 taskNum;
    WxTask taskList[0];
} WxTasks;

typedef enum {
    WX_MSG_SEND_TO_CORE,    /* 娑堟伅鍙戦�佸埌鍏朵粬鏍� */
    WX_MSG_SEND_TO_TASK,    /* 娑堟伅鍙戦�佸埌鏈牳浠诲姟 */
} WxMsgSendMethod;

/* 妯″潡璺敱淇℃伅 */
typedef struct tagWxModuleRouter {
    UINT8 coreId; /* 鎵�灞炵殑鏍窱D */
    UINT8 resv;
    UINT16 res16;
    UINT32 resv32;
    WxTask *belongTask; /* 鎵�灞炵殑浠诲姟, WxTask */
    WxModule *belongModule; /* 鎵�灞炵殑妯″潡, 浣滀负鍏ュ弬 */
} WxModuleRouter;

typedef struct tagWxModuleRouterList {
    UINT8 coreId;                       /* 褰撳墠璺敱琛ㄦ墍灞炵殑鏍窱D */
    WxModuleRouter routers[WX_MODULE_BUTT]; /* 鍚勬ā鍧楁秷鎭殑璺敱 */
} WxModuleRouterList;

UINT32 WX_RegModuleRouter(WxModuleId moduleId, UINT8 coreId,
    WxTask *belongTask, WxModule *belongModule);
/* 娑堟伅璋冨害 */
UINT32 WX_MsgShedule(UINT8 sender, UINT8 receiver, VOID *msg);

VOID *WX_ApplyEvtMsgFromISR(WxMsgType msgType);
VOID *WX_ApplyEvtMsg(WxMsgType msgType);
VOID WX_FreeEvtMsg(WxMsg **pp);
VOID WX_FreeEvtMsgFromISR(WxMsg **pp);
UINT32 WX_CreateMsgResPool(VOID);

UINT32 WX_RegModuleRouter(WxModuleId moduleId, UINT8 coreId,
    WxTask *belongTask, WxModule *belongModule);
/* 消息调度 */
UINT32 WX_MsgShedule(UINT8 sender, UINT8 receiver, VOID *msg);
XScuGic *WX_GetOrCreateScuGicInstance(VOID);
UINT32 WX_SetupFrame(UINT8 coreId);
#define WX_GetCurCoreId() g_curCoreId
#define WX_GetModuleName(m)         (((WxModule *)m)->moduleName)
#define WX_GetModuleInfo(m)         (((WxModule *)m)->moduleInfo)
#define WX_SetModuleInfo(m, v)      (((WxModule *)m)->moduleInfo = (v))
#define WX_GetModuleCoreId(m)       (((WxModule *)m)->coreId)
#define WX_GetModuleId(m)           (((WxModule *)m)->moduleId)

#endif
