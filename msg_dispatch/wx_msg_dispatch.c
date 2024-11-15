#include "wx_msg_dispatch.h"
#include "wx_failcode.h"
#include "wx_msg_intf.h"

typedef struct {
    QueueHandle_t queHandle; /* 消息队列的handle */
    UINT32 itermSize;        /* 消息队列项尺寸（Byte） */
    UINT64 queFullCnt;       /* 消息队列满计数器 */
} WxMsgQueInfo;

/* 不同模块的消息队列信息， 由各模块自行注册，如果模块想要和其他模块通信则需要注册其消息接收队列 */
QueueHandle_t g_wxMsgQueHandles[WX_MODULE_BUTT] = {0};

UINT32 WX_RegMsgQue(WxMsgReceiver receiver, QueueHandle_t queHandle, UINT32 itermSize) 
{
    if (receiver >= WX_MODULE_BUTT) {
        return WX_MSG_QUE_REG_INVALID_RECVER;
    }
    g_wxMsgQueHandles[receiver].queHandle = queHandle;
    g_wxMsgQueHandles[receiver].itermSize = itermSize;

    return WX_SUCCESS;
}
/* 用于发送消息到指定的模块，消息头中的sender和receiver会发生变化 */
UINT32 WX_MSG_Dispatch(WxMsgSender sender, WxMsgReceiver receiver, WxMsgHeader *msgHead)
{
    if (msgHead == NULL) {
        return WX_MSG_DISPATCH_INVALID_MSG_HEADER;
    }
    msgHead->sender = sender;
    msgHead->receiver = receiver;
    if (receiver >= WX_MODULE_BUTT) {
        return WX_MSG_DISPATCH_INVALID_RECVER;
    }
    /* 获取接收模块的消息队列信息 */
    WxMsgQueInfo *msgQueInfo = &g_wxMsgQueHandles[receiver];
    if (msgQueInfo->queHandle == NULL) {
        return WX_MSG_DISPATCH_RECVER_UNREG_MSG_QUE;
    }

    /* 判断消息队列是否可以容纳该消息大小，如果不能容纳则返回错误 */
    if (msgQueInfo->itermSize > sizeof(WxMsgHeader) + msgHead->msgBodyLen) {
        return WX_MSG_DISPATCH_MSG_LEN_ERR;
    }

    if (xQueueSend(msgQueInfo->queHandle, (const void * )msgHead, (TickType_t)0 ) != pdTRUE) {
        msgQueInfo->queFullCnt++;
        return WX_MSG_DISPATCH_MSG_QUE_FULL;
    }

    return WX_SUCCESS;
}