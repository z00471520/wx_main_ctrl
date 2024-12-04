#include "wx_can_slave.h"
#include "wx_can_slave_rmt_ctrl_pdu.h"
#include "wx_msg_can_frame_intf.h"
#include "wx_frame.h"
 
// WxCanSlave *canSlave = this;
UINT32 WX_CAN_SLAVE_DecRmtCtrlPduReset(VOID *this, WxRmtCtrlPdu *pdu, WxRmtCtrlReqMsg *msg)
{
    return WX_SUCCESS;
}

/* 瑙ｇ爜鎺ユ敹鍒拌繙绋嬮仴鎺DU */
UINT32 WX_CAN_SLAVE_DecRmtCtrlPdu(WxCanSlave *this, WxRmtCtrlPdu *pdu, WxRmtCtrlReqMsg *msg)
{
    UINT16 rmtCtrlCode = (UINT16)pdu->rmtCtrlCode; /* 闁儲甯堕幐鍥︽姢閻拷 */
    if (rmtCtrlCode >= WX_RMT_CTRL_CODE_BUTT) {
        wx_critical("Error Exit: unknown rmtCtrlCode(%u)", rmtCtrlCode);
        return WX_CAN_SLAVE_INVALID_RMT_CTRL_CODE;
    }
    /* 鑾峰彇鎸囦护鐮佸搴旂殑瑙ｇ爜handle  */
    WxRmtCtrlReqHandle *handle = &g_wxRmtCtrlReqHandles[rmtCtrlCode];
    if (handle == NULL) {
        return WX_CAN_SLAVE_CTRL_CODE_DEC_UNSPT;
    }
    return handle->decHandle(this, pdu, msg);
}

/* 缂栫爜PDU鏁版嵁鍒癈AN甯ф暟鎹� */
UINT32 WX_CAN_SLAVE_EncapPdu2CanFrames(WxCanSlave *this, WxRmtCtrlPdu *pdu, WxCanFrameList *canFrameList)
{
    canFrameList->canFrameNum = 0;
    WxCanSlaveCfg *cfgInfo = this->cfgInfo;
    if (cfgInfo == NULL) {
        return WX_CAN_SLAVE_CFG_INFO_UN_INIT;
    }
    UINT8 canFrameDataLen = cfgInfo->canFrameDataLen;
    if (canFrameDataLen == 0) {
        return WX_CAN_SLAVE_CAN_FRAME_LEN_ZERO;
    }
    UINT32 messID = cfgInfo->messId;
    /* CALCULATE THE NUMBER OF CAN FRAME NEEDED */
    UINT32 frameNum = (pdu->dataLen + canFrameDataLen - 1) % canFrameDataLen;
    if (frameNum > WX_CAN_DRIVER_FRAME_LIST_MAX_ITERM_NUM) {
        return WX_CAN_SLAVE_FRAME_NUM_EXCEED_LIMIT;
    }

    WxCanFrame *curFrame = NULL;
    UINT16 cnt = 0;
    for (UINT32 i = 0; i < frameNum; i++) {
        curFrame = &canFrameList->canFrames[i];
        /* 濞撳懏顨熺�电钖� */
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

UINT32 WX_CAN_SLAVE_SendFrame2CanIf(WxCanSlave *this, WxCanFrame *frame)
{
    UINT32 ret;
    WxCanFrameMsg *canFrameMsg = (WxCanFrameMsg *)WX_ApplyEvtMsg(WX_MSG_TYPE_CAN_FRAME);
    if (canFrameMsg == NULL) {
        return WX_APPLY_EVT_MSG_ERR;
    }
    WX_CLEAR_OBJ((WxMsg *)canFrameMsg);
    canFrameMsg->canFrame = *frame;
    canFrameMsg->sender = this->moduleId;
    canFrameMsg->receiver = (this->moduleId == WX_MODULE_CAN_SLAVE_A) ?
        WX_MODULE_DRIVER_CAN_A : WX_MODULE_DRIVER_CAN_B;
    canFrameMsg->msgType = WX_MSG_TYPE_CAN_FRAME;

    ret = WX_MsgShedule(this->moduleId, canFrameMsg->receiver, canFrameMsg);
    if (ret != WX_SUCCESS) {
        WX_FreeEvtMsg((WxMsg **)&canFrameMsg);
    }

    return ret;
}

UINT32 WX_CAN_SLAVE_SendCanFrameList2CanIf(WxCanSlave *this, WxCanFrameList *canFrameList)
{
    UINT32 ret;
    for (UINT32 i = 0; i < canFrameList->canFrameNum; i++) {
        ret = WX_CAN_SLAVE_SendFrame2CanIf(this, &canFrameList->canFrames[i]);
        if (ret != WX_SUCCESS) {
            return ret;
        }
    }

    return WX_SUCCESS;
}

UINT32 WX_CAN_SLAVE_SendPdu2CanIf(WxCanSlave *this, WxRmtCtrlPdu *pdu)
{
    /* translate the pdu to can frame list */
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

