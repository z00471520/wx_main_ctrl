#include "wx_can_slave_task.h"
typedef UINT32 (*WxCmdCodeDecHandle)(VOID);



WxCmdCodeDecHandle g_wxRmtMsgDecHandle[WX_RMT_CTRL_CODE_BUTT] =
{
    [WX_RMT_CTRL_CODE_RESET] = 
};
/*
 * 函数功能：遥控消息封装CAN frame中，需要根据接收到的CAN frame解码出遥控消息
 * 参数说明
 * canframe      - 待解码的canFrame输入 
 * remoteCtrlMsg - 解码出来的的remoteCtrlMsg，在返回OK的时候，可能有效
 * 返回值说明
 * WX_SUCCESS    本次解码OK，但是remoteCtrlMsg可能是无效，因为当前接收的帧还不能解码出最终消息
 * 其他          解码失败的原因
 **/
UINT32 WX_CAN_SLAVE_DecodeRemoteCtrlMsg(WxCanSlaveTask *this, WxCanFrame *canFrame, WxRemoteCtrlMsg *remoteCtrlMsg)
{
    /* TODO：解码出PDU */
    UINT16 instructCode = (UINT16)canFrame->data[0]; /* 指令码 */

    
    return;
}