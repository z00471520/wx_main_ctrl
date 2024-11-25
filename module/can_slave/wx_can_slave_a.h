#ifndef __WX_CAN_SALVE_A_H__
#define __WX_CAN_SALVE_A_H__
#include "wx_task_deploy.h"
UINT32 WX_CAN_SLAVE_A_Construct(VOID *module);
UINT32 WX_CAN_SLAVE_A_Destruct(VOID *module);
UINT32 WX_CAN_SLAVE_A_Entry(VOID *module, WxMsg *evtMsg);
#endif