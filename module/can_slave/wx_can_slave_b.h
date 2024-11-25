#ifndef __WX_CAN_SALVE_B_H__
#define __WX_CAN_SALVE_B_H__
#include "wx_task_deploy.h"
UINT32 WX_CAN_SLAVE_B_Construct(VOID *module);
UINT32 WX_CAN_SLAVE_B_Destruct(VOID *module);
UINT32 WX_CAN_SLAVE_B_Entry(VOID *module, WxMsgType *evtMsg);
#endif