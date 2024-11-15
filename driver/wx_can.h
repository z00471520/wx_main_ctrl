#ifndef WX_CAN_H
#define WX_CAN_H
#include "wx_include.h"

typedef struct {
    UINT32 deviceId; /* 设备ID */
    UINT8 baudPrescalar; /* 需要结合输入时钟的频率进行设置，用于确定最终的波特率 */
    UINT8 syncJumpWidth; /* SyncJumpWidth is the Synchronization Jump Width value to set. Valid values are from 0 to 3 */
    UINT8 timeSegment1;  /* TimeSegment1 is the Time Segment 1 value to set. Valid values are from 0 to 15 */
    UINT8 timeSegment2;  /* TimeSegment2 is the Time Segment 2 value to set. Valid values are from 0 to 7 */
    /* if more please add here */
} WxCanDeviceCfgInfo;

#endif