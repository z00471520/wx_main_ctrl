#include "wx_include.h"
#include "wx_rs422_driver_master.h"
#include "wx_rs422_driver_master_req_msg.h"
WxRs422DriverMasterCfg g_rs422DriverMasterCfg = {

};

VOID WX_RS422_DRIVER_MASTER_SentRspAdu2Rs422Master(WxRs422DriverMaster *this, WxModbusAdu *rxAdu)
{
    UINT32 ret;
    /* 申请消息 */
    WxMsg *msg = WX_ApplyEvtMsg(WX_MSG_TYPE_CAN_FRAME);
    if (msg == NULL) {
        return;
    }
    /* 初始化消息头 */
    WX_CLEAR_OBJ(msg);
    /* 填写消息信息 */
    *(WxModbusAdu *)(msg->msgData) = *rxAdu;
    msg->sender = this->moduleId;
    msg->receiver = WX_MODULE_RS422_I_MASTER;
    msg->msgType = WX_MSG_TYPE_RS422_MASTER_ADU_RSP;
    msg->msgDataLen = sizeof(WxModbusAdu);
    /* 发送消息 */
    UINT32 ret = WX_MsgShedule(this->moduleId, msg->receiver, msg);
    if (ret != WX_SUCCESS) {
        WX_FreeEvtMsg(&msg);
    }
}

/* 处理接收到的PDU */
VOID WX_RS422_DRIVER_MASTER_ProcRecvAduFromISR(WxRs422DriverMaster *this)
{
    this->status = WX_RS422_MASTER_STATUS_IDLE;
    /* 处理接收到的报文 */
    WX_RS422_DRIVER_MASTER_SentRspAdu2Rs422Master(this,  &this->rxAdu);

    /* 发送消息通知RS422编解码模块处理 */
    WxModbusAdu *txAdu = &this->txAdu;
    /* 如果有消息待发送则继续发送 */
    if (xQueueReceiveFromISR(this->msgQueHandle, txAdu, 0) == pdPASS) {
        /* 清空并预设预期接收的报文大小，防止任务被抢占来不及缓存导致串口消息丢失 */
        UINT32 recvCount; 
        do {
            recvCount = XUartNs550_Recv(&this->rs422Inst, rxAdu->value, txAdu->expectRspLen);
        } while (recvCount);
        
        /* 首次发送消息会被缓存到实例，返回缓存了多少报文 */
        UINT32 sendCount = XUartNs550_Send(&this->rs422Inst, txAdu->value, (unsigned int)txAdu->valueLen);
        if (sendCount == 0) {
            /* 是不可能出现发送0情况，这里算是异常了 */
            return;
        }
        this->status = WX_RS422_MASTER_STATUS_TX_ADU;
    }

    return;
}


/* RS422I中断处理函数 */
VOID WX_RS422I_DRIVER_MASTER_IntrHandler(VOID *callBackRef, UINT32 event, UINT32 eventData)
{
    WxRs422DriverMaster *this = CallBackRef;
    /*
	 * All of the data has been sent.
	 */
	if (event == XUN_EVENT_SENT_DATA) {
        /* 发送完毕后切换到接收状态 */
        this->status = WX_RS422_MASTER_STATUS_RX_ADU;
	}

    /*
	 * All of the data has been received.
	 */
	if (event == XUN_EVENT_RECV_DATA) {
        
        this->rxAdu.valueLen = eventData;
        WX_RS422_DRIVER_MASTER_ProcRecvAduFromISR(this);
	}

	/*
	 * Data was received, but not the expected number of bytes, a
	 * timeout just indicates the data stopped for 4 character times.
	 */
	if (event == XUN_EVENT_RECV_TIMEOUT) {
        this->rxAdu.valueLen = eventData;
        WX_RS422_DRIVER_MASTER_ProcRecvAduFromISR(this);
	}
}

UINT32 WX_RS422_DRIVER_MASTER_ProcTxAduMsg(WxRs422DriverMaster *this, WxRs422MasterDriverMsg *msg)
{
    if (this->status != WX_RS422_MASTER_STATUS_IDLE) {
        /* 缓存待发送的消息 */
        if (xQueueSend(this->msgQueHandle, &msg->modbusAdu, 0) != pdPASS) {
            return WX_RS422_MASTER_CACHE_MSG_FAIL;
        }
        return WX_SUCCESS;
    }
    
    /* 清空并预设预期接收的报文大小，防止任务被抢占来不及缓存导致串口消息丢失 */
    UINT32 recvCount; 
    do {
        recvCount = XUartNs550_Recv(&this->rs422Inst, rxAdu->value, txAdu->expectRspLen);
    } while (recvCount);
    
    /* 首次发送消息会被缓存到实例，返回缓存了多少报文 */
    UINT32 sendCount = XUartNs550_Send(&this->rs422Inst, txAdu->value, (unsigned int)txAdu->valueLen);
    if (sendCount == 0) {
        /* 是不可能出现发送0情况，这里算是异常了 */
        return WX_RS422_MASTER_SNED_ADU_BUFFER_FAIL;
    }
    this->status = WX_RS422_MASTER_STATUS_TX_ADU;
    return WX_SUCCESS;
}


UINT32 WX_RS422_DRIVER_MASTER_Construct(VOID *module)
{
    UINT32 ret;
    WxRs422DriverMaster *this = WX_Mem_Alloc(WX_GetModuleName(module), 1, sizeof(WxRs422DriverMaster));
    if (this == NULL) {
        return WX_ERR;
    }
    WxRs422DriverMasterCfg *cfg = &g_rs422DriverMasterCfg;
    /* 创建RS422消息缓存队列 */
    this->msgQueHandle = xQueueCreate(cfg->msgQueItemNum, sizeof(modbusAdu));
    if (this->msgQueHandle == NULL) {
        wx_critical("Error Exit: xQueueCreate fail");
        return WX_RS422_MASTER_CREATE_MSG_QUE_FAIL;
    }
    /* the inst or rs422 used for uart data tx/rx */
    ret = WX_InitUartNs550(&this->rs422Inst, cfg->rs422DevId, cfg->rs422Format);
    if (ret != WX_SUCCESS) {
        return rc;
    }

    /* 设置中断 */
    ret = WX_SetupUartNs550Interrupt(&this->rs422Inst, WX_RS422I_DRIVER_MASTER_IntrHandler, cfg->intrId, this);
    if (rc != WX_SUCCESS) {
        return ret;
    }

    /* 设置上Module */
    WX_SetModuleInfo(module, this);
    return WX_SUCCESS;
}

UINT32 WX_RS422_DRIVER_MASTER_Entry(VOID *module, WxMsg *evtMsg)
{
    WxRs422DriverMaster *this = WX_GetModuleInfo(module);
    if (evtMsg->msgType != WX_MSG_TYPE_RS422_MASTER_DRIVER) {
        return WX_ERR;
    }
    /* 子消息类型 */
    switch (evtMsg->subMsgType) {
        case WX_SUB_MSG_RS422_DRIVER_MASTER_TX_ADU: {
            return WX_RS422_DRIVER_MASTER_ProcTxAduMsg(this, evtMsg);
        }
        default: {
            return WX_ERR;
        }
    }
}

UINT32 WX_RS422_DRIVER_MASTER_Destruct(VOID *module)
{
    WxRs422DriverMaster *this = WX_GetModuleInfo(module);
    if (this == NULL) {
        return WX_SUCCESS;
    }
    WX_Mem_Free(this);
    WX_SetModuleInfo(module, NULL);
    return WX_SUCCESS;
}