#include "wx_include.h"
#include "wx_can_slave_task.h"
#include "wx_can.h"
#include "task.h"
#include "wx_task.h"
#include "wx_msg_can_frame_intf.h"
#define WX_CAN_SLAVE_BUFF_FRAME_NUM 1024

/* CAN类型的定义 */
typedef enum {
    WX_CAN_TYPE_A,
    WX_CAN_TYPE_B,
    WX_CAN_TYPE_BUTT,
} WxCanTypeDef;

/* 自定义配置 */
typedef struct
{
    UINT32 maxMsgNum; /* 任务缓存CAN帧的个数 */
    /* if more please add here */
} WxCanSlaveSelfDefCfg;


typedef struct {
    WxCanSlaveSelfDefCfg selfDefCfg;
    WxTaskCfgInfo taskCfg;
    WxCanDeviceCfgInfo deviceCfgInfo; /* CAN设备配置信息 */ 
    WxCanIntrCfgInfo intrCfgInfo; /* 中断配置信息 */
} WxCanSlaveCfgInfo;

/* CAN从机任务信息 */
typedef struct {
    QueueHandle_t msgQue; /* 消息队列 */
    XCanPs canInst; /* 当前任务处理的Can实例 */
    TaskHandle_t taskHandle; /* 任务Handle */
    UINT8 canDataBuff; /* CAN  */
    WxRmtCtrlPdu rmtCtrlPdu; /* CAN PDU */
} WxCanSlaveTask;

WxCanSlaveTask g_canSlaveTask[WX_CAN_TYPE_BUTT] = {0};
WxCanSlaveCfgInfo g_wxCanSlaveTaskCfg[WX_CAN_TYPE_BUTT] = {

    [WX_CAN_TYPE_A] = {
        /* 自定义配置 */
        .selfDefCfg.maxMsgNum = WX_CAN_SLAVE_BUFF_FRAME_NUM,
        /* 任务配置 */
        .taskCfg.name = "CAN-A",
        .taskCfg.param = &g_canSlaveTask[WX_CAN_TYPE_A],
        /* 设备配置 */
        .deviceCfgInfo.isEnable = FALSE, /* 是否使能 */
        .deviceCfgInfo.baudPrescalar = 0,
        /* 中断配置 */
        .intrCfgInfo.intrId = 0,
        .intrCfgInfo.callBackRef = 0,
    },
    [WX_CAN_TYPE_B] = {
         /* 自定义配置 */
        .selfDefCfg.maxMsgNum = WX_CAN_SLAVE_BUFF_FRAME_NUM,
        /* 任务配置 */
        .taskCfg.name = "CAN-B",
        .taskCfg.param = &g_canSlaveTask[WX_CAN_TYPE_B],
        /* 设备配置 */
        .deviceCfgInfo.isEnable = FALSE, /* 是否使能 */
        .deviceCfgInfo.baudPrescalar = 0,
        /* 中断配置 */
        .intrCfgInfo.intrId = 0,
        .intrCfgInfo.callBackRef = 0,
    },
};

UINT32 WX_CAN_SLAVE_CheckCanFrameMsg(WxCanFrameMsg *msg)
{
    if (msg->msgHead.msgBodyLen != sizeof(WxCanFrame)) {
        wx_critical(WX_EXCP_INVALID_MSG_BODY_LEN, "Error Exit: Invalid msgLen(%u)", msg->msgHead.msgBodyLen);
        return WX_INVALID_MSG_BODY_LEN;
    }

    if (msg->canFrame.dataLengCode == 0) {
        wx_excp_cnt(WX_EXCP_CAN_FRAME_DATA_LEN_ERR);
        return WX_CAN_SLAVE_CAN_FRAME_DATA_LEN_ERR;
    }

    return WX_SUCCESS;
}

/* 解封装FRAME, 把CAN frame解析成PDU */
UINT32 WX_CAN_SLAVE_DecapCanFrame(WxCanSlaveTask *this, WxCanFrame *canFrame)
{
    UINT32 ret = WX_CAN_SLAVE_CheckCanFrameMsg(canFrame);
    if (ret != WX_SUCCESS) {
        return ret;
    }


}

/* 处理中断发送的CAN FRAME */
UINT32 WX_CAN_SLAVE_ProcCanFramMsg(WxCanSlaveTask *this, WxCanFrameMsg *canFrameMsg)
{
    /* 解封装CAN Frame */
    UINT32 ret = WX_CAN_SLAVE_DecapCanFrame(this, &canFrameMsg->canFrame);
    if (ret != WX_SUCCESS) {
        /* 当前CAN Frame不足以解析出PDU */
        if (ret == WX_TO_BE_CONTINUE) {
            return WX_SUCCESS;
        }
        return ret;
    }
    

    ret = WX_CAN_SLAVE_DecodeRemoteCtrlMsg(this, &msg->canFrame, )
}

/* 处理收到消息 */
VOID WX_CAN_SLAVE_ProcMsg(WxCanSlaveTask *this, WxCanFrameMsg *msg)
{
    switch (msg->msgHead.msgType) {
        /* 收到CAN消息 */
        case WX_MSG_TYPE_CAN_FRAME: {
            WX_CAN_SLAVE_ProcCanFramMsg(this, msg);
        }
        /* if more please add here */
        default: {
            wx_log(WX_CRITICAL, "Error Exit: unknown msgtype(%u)", msg->msgHead.msgType);
            return WX_CAN_SLAVE_UNSPT_MSG_TYPE;
        }
    }
}

/* WX_CAN从机收发消息任务 */
VOID WX_CAN_SlaveTask(VOID *param)
{
    WxCanSlaveTask *this = param;
    WxMsg msg;
    while(TRUE) {
        /* 死等收消息 */
        if (xQueueReceive(this->msgQue, (VOID *)&msg, portMAX_DELAY)) {
            WX_CAN_SLAVE_ProcMsg(this, msg);
        }
    }
}

/* 创建RS422I主机任务, 参数合法性由调用者保证 */
UINT32 WX_CAN_SALVE_CreateTask(WxCanSlaveTask *this, WxCanSlaveCfgInfo *cfg)
{
    /* create the msg que to buff the recv can frame */
    if (this->msgQue == 0) {
        this->msgQue = xQueueCreate(cfg->maxMsgNum, sizeof(WxCanFrame));
        if (this->msgQue == 0) {
            wx_log(WX_CRITICAL, "Error Exit: xQueueCreate fail");
            return WX_CAN_SLAVE_CREATE_MSG_QUE_FAIL;
        }
    }

    /* 初始化设备 */
    UINT32 ret = WX_CAN_InitialDevice(&this->canInst, &cfg->deviceCfgInfo);
    if (ret != WX_SUCCESS) {
        return ret;
    }

    /* 设置中断 */
    ret = WX_CAN_SetupCanInterrupt(&this->canInst, &cfg->intrCfgInfo);
    if (ret != WX_SUCCESS) {
        return ret;
    }

    /* Create the task, storing the handle. */
    ret = WX_TASK_Create(&this->taskHandle, &cfg->taskCfg);
    return ret;
}