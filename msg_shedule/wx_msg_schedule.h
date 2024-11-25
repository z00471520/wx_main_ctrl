#ifndef __WX_MSG_SCHEDULE_H__
#define __WX_MSG_SCHEDULE_H__
#include "wx_include.h"
#include "wx_msg_intf.h"
#include "wx_task_deploy.h"

typedef enum {
    WX_MSG_SEND_TO_CORE,    /* 消息发送到其他核 */
    WX_MSG_SEND_TO_TASK,    /* 消息发送到本核任务 */
} WxMsgTypeSendMethod;

/* 路由项 */
typedef struct tagWxMsgTypeRouterListInfo {
    UINT8 coreId; /* 所属的核ID */
    UINT8 resv;
    UINT16 res16;
    UINT32 resv32;
    WxTask *belongTask; /* 所属的任务, WxTask */
    WxModule *belongModule; /* 所属的模块, 作为入参 */
} WxMsgTypeRouter;

/* 消息路由表 */
typedef struct tagWxMsgTypeRouterListInfo {
    UINT8 coreId;                       /* 当前路由表所属的核ID */
    WxMsgTypeRouter routers[WX_MODULE_BUTT]; /* 各模块消息的路由 */
} WxMsgTypeRouterList;

UINT32 WX_MsgShedule_RegRouter(WxModuleId receiver, UINT8 coreId, 
    WxTask *belongTask, WxModule *belongModule, WxModuleEntryFunc entryFunc);
/* 消息调度 */
UINT32 WX_MsgShedule(UINT8 sender, UINT8 receiver, VOID *msg);
#endif