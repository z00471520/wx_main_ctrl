#ifndef __WX_MSG_SCHEDULE_H__
#define __WX_MSG_SCHEDULE_H__
#include "wx_include.h"
#include "wx_msg_common.h"
#include "wx_deploy.h"

typedef enum {
    WX_MSG_SEND_TO_CORE,    /* 消息发送到其他核 */
    WX_MSG_SEND_TO_TASK,    /* 消息发送到本核任务 */
} WxMsgSendMethod;

/* 模块路由信息 */
typedef struct tagWxModuleRouter {
    UINT8 coreId; /* 所属的核ID */
    UINT8 resv;
    UINT16 res16;
    UINT32 resv32;
    WxTask *belongTask; /* 所属的任务, WxTask */
    WxModule *belongModule; /* 所属的模块, 作为入参 */
} WxModuleRouter;

/* 模块消息路由表 */
typedef struct tagWxModuleRouterList {
    UINT8 coreId;                       /* 当前路由表所属的核ID */
    WxModuleRouter routers[WX_MODULE_BUTT]; /* 各模块消息的路由 */
} WxModuleRouterList;

UINT32 WX_RegModuleRouter(WxModuleId moduleId, UINT8 coreId, 
    WxTask *belongTask, WxModule *belongModule);
/* 消息调度 */
UINT32 WX_MsgShedule(UINT8 sender, UINT8 receiver, VOID *msg);
#endif
