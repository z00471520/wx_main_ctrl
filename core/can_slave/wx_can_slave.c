#include "wx_include.h"
#include "wx_can_slave.h"
#include "wx_can_driver.h"
#include "task.h"
#include "wx_task.h"
#include "wx_msg_can_frame_intf.h"
#include "wx_can_slave_rmt_ctrl_pdu.h"
#include "wx_can_slave_rmt_ctrl_msg.h"
#define WX_CAN_DRIVER_SLAVE_BUFF_FRAME_NUM 1024

WxCanSlave g_wxCanSlave[WX_CAN_DRIVER_TYPE_BUTT] = {0};
WxCanSlaveCfgInfo g_wxCanSlaveTaskCfg[WX_CAN_DRIVER_TYPE_BUTT] = {

    [WX_CAN_DRIVER_TYPE_A] = {
        /* 自定义配置 */
        .selfDefCfg.maxMsgNum = WX_CAN_DRIVER_SLAVE_BUFF_FRAME_NUM,
        /* 任务配置 */
        .taskCfg.name = "CAN-A",
        .taskCfg.param = &g_wxCanSlave[WX_CAN_DRIVER_TYPE_A],
        /* 设备配置 */
        .deviceCfgInfo.isEnable = FALSE, /* 是否使能 */
        .deviceCfgInfo.baudPrescalar = 0,
        /* 中断配置 */
        .intrCfgInfo.intrId = 0,
        .intrCfgInfo.callBackRef = 0,
    },
    [WX_CAN_DRIVER_TYPE_B] = {
         /* 自定义配置 */
        .selfDefCfg.maxMsgNum = WX_CAN_DRIVER_SLAVE_BUFF_FRAME_NUM,
        /* 任务配置 */
        .taskCfg.name = "CAN-B",
        .taskCfg.param = &g_wxCanSlave[WX_CAN_DRIVER_TYPE_B],
        /* 设备配置 */
        .deviceCfgInfo.isEnable = FALSE, /* 是否使能 */
        .deviceCfgInfo.baudPrescalar = 0,
        /* 中断配置 */
        .intrCfgInfo.intrId = 0,
        .intrCfgInfo.callBackRef = 0,
    },
};

/* 不同指令码对应的Handle */
WxRmtCtrlReqHandle g_wxRmtCtrlReqHandles[WX_RMT_CTRL_CODE_BUTT] = {
    /* [指令码] = {内部消息类型，指令码PDU解码函数, 消息处理函数} */
    [WX_RMT_CTRL_CODE_RESET] = {WX_RMT_CTRL_REQ_MSG_TYPE_RESET, WX_CAN_DRIVER_SLAVE_DecRmtCtrlPduReset, WX_CAN_DRIVER_SLAVE_ProcRmtCtrlReqMsgReset},
};


UINT32 WX_CAN_DRIVER_SLAVE_CheckCanFrameMsg(WxCanFrameMsg *msg)
{
    if (msg->msgHead.msgBodyLen != sizeof(WxCanFrame)) {
        wx_critical(WX_EXCP_INVALID_MSG_BODY_LEN, "Error Exit: Invalid msgLen(%u)", msg->msgHead.msgBodyLen);
        return WX_INVALID_MSG_BODY_LEN;
    }

    if (msg->canFrame.dataLengCode == 0) {
        wx_excp_cnt(WX_EXCP_CAN_FRAME_DATA_LEN_ERR);
        return WX_CAN_DRIVER_SLAVE_CAN_FRAME_DATA_LEN_ERR;
    }

    return WX_SUCCESS;
}

/* 
 * 函数功能: 解封装FRAME, 把CAN frame解析成PDU
 * 返回值说明
 * WX_SUCCESS - 解封装成功
 * WX_TO_BE_CONTINUE - 解封装未完待续
 * 其他：解封装异常
 * 备注：该模块需要根据最终的卫星协议来确定
 **/
UINT32 WX_CAN_DRIVER_SLAVE_DecapCanFrame(WxCanSlave *this, WxCanFrame *canFrame, WxRmtCtrlPdu *canPdu)
{
    UINT32 ret = WX_CAN_DRIVER_SLAVE_CheckCanFrameMsg(canFrame);
    if (ret != WX_SUCCESS) {
        return ret;
    }

    canPdu->rmtCtrlCode = canFrame->data[0];
    return WX_SUCCESS;
}





/* 处理中断发送的CAN FRAME */
UINT32 WX_CAN_DRIVER_SLAVE_ProcCanFramMsg(WxCanSlave *this, WxCanFrameMsg *canFrameMsg)
{
    /* 解封装CAN Frame */
    UINT32 ret = WX_CAN_DRIVER_SLAVE_DecapCanFrame(this, &canFrameMsg->canFrame, &this->reqPdu);
    if (ret != WX_SUCCESS) {
        /* 当前CAN Frame不足以解析出PDU */
        if (ret == WX_TO_BE_CONTINUE) {
            return WX_SUCCESS;
        }
        return ret;
    }
    
    /* 解封装出PDU后，进行解码 */
    WxRmtCtrlReqMsg *rmtCtrlmsg = &this->rmtCtrlmsg;
    ret = WX_CAN_DRIVER_SLAVE_DecRmtCtrlPdu(this, &this->reqPdu, rmtCtrlmsg);
    if (ret != WX_SUCCESS) {
        return ret;
    }

    /* 处理解码后的消息 */

}

/* 处理收到消息 */
VOID WX_CAN_DRIVER_SLAVE_ProcMsg(WxCanSlave *this, WxCanFrameMsg *msg)
{
    switch (msg->msgHead.msgType) {
        /* 收到CAN消息 */
        case WX_MSG_TYPE_CAN_FRAME: {
            WX_CAN_DRIVER_SLAVE_ProcCanFramMsg(this, msg);
        }
        /* if more please add here */
        default: {
            wx_log(WX_CRITICAL, "Error Exit: unknown msgtype(%u)", msg->msgHead.msgType);
            return WX_CAN_DRIVER_SLAVE_UNSPT_MSG_TYPE;
        }
    }
}

/* WX_CAN从机收发消息任务 */
VOID WX_CAN_DRIVER_SlaveTask(VOID *param)
{
    WxCanSlave *this = param;
    WxMsg msg;
    while(TRUE) {
        /* 死等收消息 */
        if (xQueueReceive(this->msgQue, (VOID *)&msg, portMAX_DELAY)) {
            WX_CAN_DRIVER_SLAVE_ProcMsg(this, msg);

        }
    }
}

/* 创建RS422I主机任务, 参数合法性由调用者保证 */
UINT32 WX_CAN_DRIVER_SALVE_CreateTask(WxCanSlave *this, WxCanSlaveCfgInfo *cfg)
{
    /* create the msg que to buff the recv can frame */
    if (this->msgQue == 0) {
        this->msgQue = xQueueCreate(cfg->maxMsgNum, sizeof(WxCanFrame));
        if (this->msgQue == 0) {
            wx_log(WX_CRITICAL, "Error Exit: xQueueCreate fail");
            return WX_CAN_DRIVER_SLAVE_CREATE_MSG_QUE_FAIL;
        }
    }

    /* 初始化设备 */
    UINT32 ret = WX_CAN_DRIVER_InitialDevice(&this->canInst, &cfg->deviceCfgInfo);
    if (ret != WX_SUCCESS) {
        return ret;
    }

    /* 设置中断 */
    ret = WX_CAN_DRIVER_SetupCanInterrupt(&this->canInst, &cfg->intrCfgInfo);
    if (ret != WX_SUCCESS) {
        return ret;
    }

    /* Create the task, storing the handle. */
    ret = WX_TASK_Create(&this->taskHandle, &cfg->taskCfg);
    return ret;
}