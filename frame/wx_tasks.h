#ifndef __WX_FRAME_TASKS_H__
#define __WX_FRAME_TASKS_H__
#include "wx_include.h"
#include "wx_frame.h"
VOID WX_TaskHandle(VOID *param);
WxTask *WX_QueryTask(CHAR *taskName);
UINT32 WX_DeployTasks(UINT8 coreId);
#endif
