#include "wx_include.h"

typedef struct {
    UINT32 argv[10];    // 鍙傛暟鏁扮粍锛屽亣璁炬渶澶�10涓弬鏁�
    UINT32 argc;        // 鏁扮粍闀垮害
} WxRemoteDbgCmd;



UINT32 WX_DEBUG_SendRemoteDbgCmd(VOID *this)
{
#if 0
    UINT32 ret;
    /* 鐢宠鍛戒护娑堟伅 */
    WxMsg *msg = WX_ApplyEvtMsg(WX_MSG_TYPE_REMOTE_DBG_CMD);
    if (msg == NULL) {
        return WX_APPLY_EVT_MSG_ERR;
    }

    /* 鍒濆鍖栨秷鎭ご */
    WX_CLEAR_OBJ(msg);

    msg->sender = this->moduleId;
    msg->receiver = WX_MODULE_RS422_MASTER_DRIVER;  /* 鍙戠粰鍏朵粬鏍� */
    msg->msgType = WX_MSG_TYPE_BEBUG;
    msg->msgDataLen = sizeof(WxModbusAdu);

    ret = WX_MsgShedule(this->moduleId, targetModuleId, cmdMsg);
    if (ret != WX_SUCCESS) {
        WX_FreeEvtMsg(&msg));
    }

    return ret;
#endif
    return WX_NOT_SUPPORT;
}
