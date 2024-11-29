#include "wx_include.h"

typedef struct {
    UINT32 argv[10];    // 参数数组，假设最多10个参数
    UINT32 argc;        // 数组长度
} WxRemoteDbgCmd;

typedef struct {
    WxMsgHead msgHead;    // 消息头
    WxRemoteCmd cmd;      // 命令内容
} WxRemoteCmdMsg;

UINT32 WX_DEBUG_SendRemoteDbgCmd(WxModule *this)
{
    UINT32 ret;
    /* 申请命令消息 */
    WxMsg *msg = WX_ApplyEvtMsg(WX_MSG_TYPE_REMOTE_DBG_CMD);
    if (msg == NULL) {
        return WX_APPLY_EVT_MSG_ERR;
    }

    /* 初始化消息头 */
    WX_CLEAR_OBJ(msg);

    /* 填写消息信息 */
    msg->sender = this->moduleId;
    msg->receiver = WX_MODULE_DRIVER_RS422_MASTER;  /* 发给其他核 */
    msg->msgType = WX_MSG_TYPE_BEBUG;
    msg->msgDataLen = sizeof(WxModbusAdu);

    /* 发送消息 */
    ret = WX_MsgShedule(this->moduleId, targetModuleId, cmdMsg);
    if (ret != WX_SUCCESS) {
        WX_FreeEvtMsg(&msg));
    }

    return ret;
}