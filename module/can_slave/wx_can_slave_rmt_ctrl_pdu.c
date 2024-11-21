#include "wx_can_slave.h"


UINT32 WX_CAN_SLAVE_DecRmtCtrlPduReset(WxCanSlave *this, WxRmtCtrlPdu *pdu, WxRmtCtrlReqMsg *msg)
{
    return WX_SUCCESS;
}


/* 根据PDU解析出消息的内容，涉及到大小端转换 */
UINT32 WX_CAN_SLAVE_DecRmtCtrlPdu(WxCanSlave *this, WxRmtCtrlPdu *pdu, WxRmtCtrlReqMsg *msg)
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
UINT32 WX_CAN_SLAVE_EncapPdu2CanFrames(WxCanSlave *this, WxRmtCtrlPdu *pdu, WxCanFrameList *canFrameList)
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

/* 把CAM FRAME集合发送给CANIF传输 */
UINT32 WX_CAN_SLAVE_SendCanFrameList2CanIf(WxCanSlave *this, WxCanFrameList *canFrameList)
{
    UINT32 ret;
     /* 通知CAN DRIVER发送Frame */
    for (UINT32 i = 0; i < canFrameList->canFrameNum; i++) {
        /* 首先调用驱动进行发送，如果发送满了，则把消息发送缓存队列中 */
        ret = WX_CAN_DRIVER_SendFrame(&this->canInst, &canFrameList->canFrames[i]);
        if (ret == WX_SUCCESS) {
            continue; /* 能装就继续装 */
        } else if (ret == WX_CAN_DRIVER_CONFIG_TX_BUFF_FULL) {
            ret = WX_CAN_SLAVE_SendFrame2TxBuff(this, &canFrameList->canFrames[i]);
            if (ret != WX_SUCCESS) {
                /* 这里出现问题，需要好好定位下，可能是发送中断写的有问题，或者是缓存太小了 */
                return ret;
            }
        } else {
            return ret;
        }
    }

    return WX_SUCCESS;
}

/* 把PDU发送到CAN IF */
UINT32 WX_CAN_SLAVE_SendPdu2CanIf(WxCanSlave *this, WxRmtCtrlPdu *pdu)
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
