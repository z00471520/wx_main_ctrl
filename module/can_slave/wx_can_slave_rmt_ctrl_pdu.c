#include "wx_can_slave.h"
#include "wx_can_slave_rmt_ctrl_pdu.h"
#include "wx_msg_res_pool.h"
#include "wx_msg_can_frame_intf.h"
#include "wx_msg_schedule.h"
UINT32 WX_CAN_SLAVE_DecRmtCtrlPduReset(WxCanSlave *this, WxRmtCtrlPdu *pdu, WxRmtCtrlReqMsg *msg)
{
    return WX_SUCCESS;
}


/* 鏍规嵁PDU瑙ｆ瀽鍑烘秷鎭殑鍐呭锛屾秹鍙婂埌澶у皬绔浆鎹� */
UINT32 WX_CAN_SLAVE_DecRmtCtrlPdu(WxCanSlave *this, WxRmtCtrlPdu *pdu, WxRmtCtrlReqMsg *msg)
{
    UINT16 rmtCtrlCode = (UINT16)pdu->rmtCtrlCode; /* 閬ユ帶鎸囦护鐮� */
    if (rmtCtrlCode >= WX_RMT_CTRL_CODE_BUTT) {
        wx_critical(WX_EXCP_CAN_SLAVE_INVALID_CTRL_CODE, "Error Exit: unknown rmtCtrlCode(%u)", rmtCtrlCode);
        return WX_CAN_SLAVE_INVALID_RMT_CTRL_CODE;
    }
    /* 鑾峰彇PDU瑙ｇ爜鍑芥暟 */
    WxRmtCtrlPduDecHandle handle = g_wxRmtCtrlReqHandles[rmtCtrlCode];
    if (handle == NULL) {
        return WX_CAN_SLAVE_CTRL_CODE_DEC_UNSPT;
    }
    return handle(this, pdu, msg);
}


/* 鎶奝DU灏佽鎴怌AN Frames, 浠ヤ究鍚庣画鍙戦�� */
UINT32 WX_CAN_SLAVE_EncapPdu2CanFrames(WxCanSlave *this, WxRmtCtrlPdu *pdu, WxCanFrameList *canFrameList)
{
    canFrameList->canFrameNum = 0;
    WxCanSlaveCfg *cfgInfo = this->cfgInfo;
    if (cfgInfo == NULL) {
        return WX_ERR;
    }
    UINT8 canFrameDataLen = cfgInfo->selfDefCfg.canFrameDataLen;
    if (canFrameDataLen == 0) {
        return WX_ERR;
    }
    UINT32 messID = cfgInfo->selfDefCfg.messId;
    /* 鍚戜笂鍙栨暣 */
    UINT32 frameNum = (pdu->dataLen + canFrameDataLen - 1) % canFrameDataLen;
    if (frameNum > WX_CAN_DRIVER_FRAME_LIST_MAX_ITERM_NUM) {
        return WX_ERR;
    }

    WxCanFrame *curFrame = NULL;
    UINT16 cnt = 0;
    for (UINT32 i = 0; i < frameNum; i++) {
        curFrame = &canFrameList->canFrames[i];
        /* 娓呮瀵硅薄 */
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

/* 鎶奀AN FRAME鍙戦�佸埌杞欢缂撳瓨闃熷垪涓� */
UINT32 WX_CAN_SLAVE_SendFrame2CanIf(WxCanSlave *this, WxCanFrame *frame)
{
    UINT32 ret;
    /* 鐢宠娑堟伅 */
    WxCanFrameMsg *canFrameMsg = WX_ApplyEvtMsg(WX_MSG_TYPE_CAN_FRAME);
    if (canFrameMsg == NULL) {
        return WX_APPLY_EVT_MSG_ERR;
    }
    /* 鍒濆鍖栨秷鎭ご */
    WX_CLEAR_OBJ((WxMsg *)canFrameMsg);
    /* 濉啓娑堟伅淇℃伅 */
    canFrameMsg->canFrame = *frame;
    canFrameMsg->sender = this->moduleId;
    canFrameMsg->receiver = (this->moduleId == WX_MODULE_CAN_SLAVE_A) ?
        WX_MODULE_DRIVER_CAN_A : WX_MODULE_DRIVER_CAN_B;
    canFrameMsg->msgType = WX_MSG_TYPE_CAN_FRAME;
    /* 鍙戦�佹秷鎭� */
    UINT32 ret = WX_MsgShedule(his->moduleId, canFrameMsg->msgHead.receiver, canFrameMsg);
    if (ret != WX_SUCCESS) {
        WX_FreeEvtMsg(&canFrameMsg);
    }

    return ret;
}

/* 鎶奀AM FRAME闆嗗悎鍙戦�佺粰CANIF浼犺緭 */
UINT32 WX_CAN_SLAVE_SendCanFrameList2CanIf(WxCanSlave *this, WxCanFrameList *canFrameList)
{
    UINT32 ret;
     /* 閫氱煡CAN DRIVER鍙戦�丗rame */
    for (UINT32 i = 0; i < canFrameList->canFrameNum; i++) {
        /* 缂撳瓨婊′簡锛屽垯鎶婃秷鎭彂閫佽蒋浠剁紦瀛橀槦鍒椾腑 */
        ret = WX_CAN_SLAVE_SendFrame2CanIf(this, &canFrameList->canFrames[i]);
        if (ret != WX_SUCCESS) {
            /* 杩欓噷鍑虹幇闂锛岄渶瑕佸ソ濂藉畾浣嶄笅锛屽彲鑳芥槸鍙戦�佷腑鏂啓鐨勬湁闂锛屾垨鑰呮槸缂撳瓨澶皬浜� */
            return ret;
        }
    }

    return WX_SUCCESS;
}

/* 鎶奝DU鍙戦�佸埌CAN IF */
UINT32 WX_CAN_SLAVE_SendPdu2CanIf(WxCanSlave *this, WxRmtCtrlPdu *pdu)
{
    /* PDU灏佽涓篊AN FRAME */
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

