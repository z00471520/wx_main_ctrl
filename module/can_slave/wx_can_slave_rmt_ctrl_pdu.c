#include "wx_can_slave_common.h"
#include "wx_can_slave_rmt_ctrl_pdu.h"
#include "wx_msg_res_pool.h"
#include "wx_msg_can_frame_intf.h"
#include "wx_msg_schedule.h"
UINT32 WX_CAN_SLAVE_DecRmtCtrlPduReset(WxCanSlaveModule *this, WxRmtCtrlPdu *pdu, WxRmtCtrlReqMsg *msg)
{
    return WX_SUCCESS;
}


/* 根据PDU解析出消息的内容，涉及到大小端转换 */
UINT32 WX_CAN_SLAVE_DecRmtCtrlPdu(WxCanSlaveModule *this, WxRmtCtrlPdu *pdu, WxRmtCtrlReqMsg *msg)
{
    UINT16 rmtCtrlCode = (UINT16)pdu->rmtCtrlCode; /* 遥控指令码 */
    if (rmtCtrlCode >= WX_RMT_CTRL_CODE_BUTT) {
        wx_critical(WX_EXCP_CAN_SLAVE_INVALID_CTRL_CODE, "Error Exit: unknown rmtCtrlCode(%u)", rmtCtrlCode);
        return WX_CAN_SLAVE_INVALID_RMT_CTRL_CODE;
    }
    /* 获取PDU解码函数 */
    WxRmtCtrlPduDecHandle handle = g_wxRmtCtrlReqHandles[rmtCtrlCode];
    if (handle == NULL) {
        return WX_CAN_SLAVE_CTRL_CODE_DEC_UNSPT;
    }
    return handle(this, pdu, msg);
}


/* 把PDU封装成CAN Frames, 以便后续发送 */
UINT32 WX_CAN_SLAVE_EncapPdu2CanFrames(WxCanSlaveModule *this, WxRmtCtrlPdu *pdu, WxCanFrameList *canFrameList)
{
    canFrameList->canFrameNum = 0;
    WxCanSlaveCfgInfo *cfgInfo = this->cfgInfo;
    if (cfgInfo == NULL) {
        return WX_ERR;
    }
    UINT8 canFrameDataLen = cfgInfo->selfDefCfg.canFrameDataLen;
    if (canFrameDataLen == 0) {
        return WX_ERR;
    }
    UINT32 messID = cfgInfo->selfDefCfg.messId;
    /* 向上取整 */
    UINT32 frameNum = (pdu->dataLen + canFrameDataLen - 1) % canFrameDataLen;
    if (frameNum > WX_CAN_DRIVER_FRAME_LIST_MAX_ITERM_NUM) {
        return WX_ERR;
    }

    WxCanFrame *curFrame = NULL;
    UINT16 cnt = 0;
    for (UINT32 i = 0; i < frameNum; i++) {
        curFrame = &canFrameList->canFrames[i];
        /* 清楚对象 */
        WX_CLEAR_OBJ(curFrame);
        curFrame->standardMessID = messID;
        curFrame->dataLengCode = canFrameDataLen;
        /* the pdu data to can frame data field */
        for (UINT32 j = 0; j < canFrameDataLen; j++) {
            curFrame->data[j] = (cnt < pdu->dataLen) ? pdu->data[cnt++] : 0;
        }
    }

    return WX_SUCCESS;
}

/* 把CAN FRAME发送到软件缓存队列中 */
UINT32 WX_CAN_SLAVE_SendFrame2CanIf(WxCanSlaveModule *this, WxCanFrame *frame)
{
    UINT32 ret;
    /* 申请消息 */
    WxCanFrameMsg *canFrameMsg = WX_ApplyEvtMsg(WX_MSG_TYPE_CAN_FRAME);
    if (canFrameMsg == NULL) {
        return WX_APPLY_EVT_MSG_ERR;
    }
    /* 初始化消息头 */
    WX_CLEAR_OBJ(&canFrameMsg->msgHead);
    /* 填写消息信息 */
    canFrameMsg->canFrame = *frame;
    canFrameMsg->msgHead.sender = this->moduleId;
    canFrameMsg->msgHead.receiver = (this->moduleId == WX_TASK_MODULE_CAN_SLAVE_A) ?
        WX_DRIVER_MODULE_CAN_SLAVE_A : WX_DRIVER_MODULE_CAN_SLAVE_B;
    canFrameMsg->msgHead.msgType = WX_MSG_TYPE_CAN_FRAME;
    canFrameMsg->msgHead.msgBodyLen = sizeof(WxCanFrame);
    /* 发送消息 */
    UINT32 ret = WX_MsgShedule(his->moduleId, canFrameMsg->msgHead.receiver, canFrameMsg);
    if (ret != WX_SUCCESS) {
        WX_FreeEvtMsg(&canFrameMsg);
    }

    return ret;
}

/* 把CAM FRAME集合发送给CANIF传输 */
UINT32 WX_CAN_SLAVE_SendCanFrameList2CanIf(WxCanSlaveModule *this, WxCanFrameList *canFrameList)
{
    UINT32 ret;
     /* 通知CAN DRIVER发送Frame */
    for (UINT32 i = 0; i < canFrameList->canFrameNum; i++) {
        /* 缓存满了，则把消息发送软件缓存队列中 */
        ret = WX_CAN_SLAVE_SendFrame2CanIf(this, &canFrameList->canFrames[i]);
        if (ret != WX_SUCCESS) {
            /* 这里出现问题，需要好好定位下，可能是发送中断写的有问题，或者是缓存太小了 */
            return ret;
        }
    }

    return WX_SUCCESS;
}

/* 把PDU发送到CAN IF */
UINT32 WX_CAN_SLAVE_SendPdu2CanIf(WxCanSlaveModule *this, WxRmtCtrlPdu *pdu)
{
    /* PDU封装为CAN FRAME */
    WxCanFrameList *canFrameList = &this->canFrameList;
    UINT32 ret = WX_CAN_SLAVE_EncapPdu2CanFrames(this, pdu, canFrameList);
    if (ret != WX_SUCCESS) {
        return ret;
    }

    ret = WX_CAN_SLAVE_SendCanFrameList2CanIf(this, canFrameList);
    if (ret != WX_SUCCESS) {
        return ret;
    }
   
    return WX_SUCCESS;
}

