#ifndef __WX_CAN_DRIVER_B_H__
#define __WX_CAN_DRIVER_B_H__
UINT32 WX_CAN_DRIVER_B_Construct(VOID *module);
UINT32 WX_CAN_DRIVER_B_Entry(VOID *module, WxMsgType *evtMsg);
UINT32 WX_CAN_DRIVER_B_Destruct(VOID *module);
#endif   // __WX_CAN_DRIVER_B_H__