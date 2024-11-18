#ifndef WX_CAN_SLAVE_TASK_H
#define WX_CAN_SLAVE_TASK_H
#include "wx_can.h"
typedef enum {
    WX_CAN_TYPE_A,
    WX_CAN_TYPE_B,
    WX_CAN_TYPE_BUTT,
} WxCanTypeDef;

typedef struct {
    WxCanDeviceCfgInfo deviceCfgInfo; /* CAN设备配置信息 */ 
    WxCanIntrCfgInfo intrCfgInfo; /* 中断配置信息 */
} WxCanSlaveCfgInfo;

/* CAN从机任务信息 */
typedef struct {
    ;
} WxCanSlaveTask;
#endif