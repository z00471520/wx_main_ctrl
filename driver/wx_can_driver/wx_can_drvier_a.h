#ifndef __WX_CAN_DRIVER_A_H__
#define __WX_CAN_DRIVER_A_H__
UINT32 WX_CAN_DRIVER_A_Construct(VOID *module);
UINT32 WX_CAN_DRIVER_A_Entry(VOID *module, WxMsg *evtMsg);
UINT32 WX_CAN_DRIVER_A_Destruct(VOID *module);
#endif