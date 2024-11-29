#ifndef __WX_DEPLOY_TASKS_H__
#define __WX_DEPLOY_TASKS_H__
#include "wx_include.h"
#include "wx_deploy.h"
WxTask *WX_DeployTasks_QueryTask(CHAR *taskName);
UINT32 WX_DeployTasks(UINT8 coreId);
#endif
