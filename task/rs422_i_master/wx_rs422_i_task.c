#include "wx_typedef.h"
#include "wx_rs422_i_task.h"
#include "wx_failcode.h"
#include "wx_rs422_i_master_intf.h"
#include "wx_log.h"
#include "queue.h"
#include "wx_rs422_i_decode_adu.h"
WxRs422ITask g_wxRs422ITask = {0};

/* 中断的引用的获取 */
VOID *WX_GetRs422IIntrDataRef(VOID)
{
    return &g_wxRs422ITask;
}

/* RS422I中断处理函数 */
void WX_RS422I_IntrHandler(void *CallBackRef, u32 Event, unsigned int EventData)
{
    WxRs422ITask *this = CallBackRef;
    /*
	 * All of the data has been sent.
	 */
	if (Event == XUN_EVENT_SENT_DATA) {
        /* 接着发送实例缓存中的数据，如果发送为0则说明发送完毕，否则继续等待下次中断 */
        xSemaphoreGiveFromISR(this->aduTxFinishSemaphore);
	}

    /*
	 * All of the data has been received.
	 */
	if (Event == XUN_EVENT_RECV_DATA) {
        this->rxAdu.aduLen = EventData;
        xSemaphoreGiveFromISR(this->aduRxFinishSemaphore);
	}

	/*
	 * Data was received, but not the expected number of bytes, a
	 * timeout just indicates the data stopped for 4 character times.
	 */
	if (Event == XUN_EVENT_RECV_TIMEOUT) {
        this->rxAdu.isTimeout = TRUE;
        this->rxAdu.aduLen = EventData;
        xSemaphoreGiveFromISR(this->aduRxFinishSemaphore);
	}
}

/* 这个函数会阻塞直到发送完成或者异常 */
WxFailCode WX_RS422I_TxAdu(WxRs422ITask *this, WxRs422ITxAdu *txAdu)
{
    /* 清空并预设预期接收的报文大小，防止任务被抢占来不及缓存导致串口消息丢失 */
    UINT32 recvCount; 
    do {
        recvCount = XUartNs550_Recv(this->rs422InstPtr, rxAdu->adu, txAdu->expectRspLen);
    } while (recvCount);

    /* 首次发送消息会被缓存到实例，返回缓存了多少报文 */
    unsigned int sendCount = XUartNs550_Send(this->rs422InstPtr, txAdu->adu, (unsigned int)txAdu->aduLen);
    if (sendCount == 0) {
        /* 是不可能出现发送0情况，这里算是异常了 */
        return WX_RS422I_SNED_ADU_BUFFER_FAIL;
    }

    /* 这里会阻塞等待发送完成，如果长时间不完成则认为是异常 */
    if (xSemaphoreTake(this->aduTxFinishSemaphore, (TickType_t)WX_RS422I_WAIT_TX_FINISH_TIME) == FALSE) {
        return WX_RS422I_SEND_ADU_TIMEOUT,
    }

    return WX_SUCCESS;
}


/* 接收报文 */
WxFailCode WX_RS422I_RxAdu(WxRs422ITask *this)
{
    /* 这里会阻塞等待发送完成，如果长时间不完成则认为是异常 */
    if (xSemaphoreTake(this->aduTxFinishSemaphore, (TickType_t)WX_RS422I_WAIT_RX_FINISH_TIME) == pdFALSE) {
        return WX_RS422I_SEND_ADU_TIMEOUT,
    }

    return WX_SUCCESS;    
}

WxMsgType WX_RS422I_GetRspMsgType(WxMsgType reqMsgType)
{
    switch (reqMsgType) {
        case WX_RS422I_MSG_READ_DATA: {
            return WX_RS422I_MSG_READ_DATA_RSP;
        }
        case WX_RS422I_MSG_WRITE_DATA: {
            return WX_RS422I_MSG_WRITE_DATA_RSP;
        }
        case WX_RS422I_MSG_READ_FILE: {
            return WX_RS422I_MSG_READ_FILE_RSP;
        }
        case WX_RS422I_MSG_WRITE_FILE: {
            return WX_RS422I_MSG_WRITE_FILE_RSP;
        }
        default:
            wx_excp_cnt(WX_EXCP_UNEXPECT_MSG_TYPE); 
            return reqMsgType;
    }
}

WxFailCode WX_RS422I_ProcMsg(WxRs422ITask *this, WxRs422IMsg *msg)
{
    WxFailCode rc = WX_RS422I_EncodeAdu(&this->rs422Msg, &this->txAdu);
    if (rc != WX_SUCCESS) {
        return rc;
    }
    /*
     * send the adu by RS422 buf
     */
    rc = WX_RS422I_TxAdu(this, &this->txAdu);
    if (rc != WX_SUCCESS) {
        return rc;
    }
    /* 请求和相应是一条消息，只需要把消息类型反转一下即可，没有必须新建一个消息 */
    this->rs422Msg.msgType = WX_RS422I_GetRspMsgType(his->rs422Msg.msgType);
    
    /* wait to recieve the resbonse */
    rc = WX_RS422I_RxAdu(this);
    if (rc != WX_SUCCESS) {
        return rc;
    }
    
    /* decode the rx ADU into msg */
    rc = WX_RS422I_DecodeAdu(this, this->rxAdu, &this->rs422Msg);
    if (rc != WX_SUCCESS) {
        return rc;
    }

    return rc;
}

/* 用于消息收发 */
VOID WX_RS422I_MainTask(VOID *pvParameters)
{
    WxRs422ITask *this = &g_wxRs422ITask;
    while(1) {
        /* block to receive the msg util the msg arrive */
        if (xQueueReceive(this->msgQueHandle, &this->rs422Msg, (TickType_t)portMAX_DELAY) == pdTRUE) {
            this->rs422Msg.failCode = WX_RS422I_ProcMsg(this, &this->rs422Msg);
            
        }
    }
}

WxFailCode WX_RS422I_CreateTask(VOID)
{
    WxRs422ITask *this = &g_wxRs422ITask;
    /* create the msg que to buff the msg to tx */
    if (this->msgQueHandle == 0) {
        this->msgQueHandle = xQueueCreate(WX_RS422I_MSG_ITERM_MAX_NUM, sizeof(WxRs422IMsg));
        if (this->msgQueHandle == 0) {
            wx_log(WX_CRITICAL, "Error Exit: xQueueCreate fail");
            return WX_RS422I_CREATE_MSG_QUE_FAIL;
        }
    }

    /* the inst or rs422 used for uart data tx/rx */
    this->rs422InstPtr = WX_GetRs422IInstPtr();
    if (this->rs422InstPtr == NULL) {
        return WX_RS422I_CREATE_TASK_FAIL_INST_NULL;
    }

    /* 该二进制信号量用于表达RS422是否把ADU消息全部发送完毕 */
    this->aduTxFinishSemaphore = xSemaphoreCreateBinary();
    if (this->aduTxFinishSemaphore == NULL) {
        /* There was insufficient FreeRTOS heap available for the semaphore to
           be created successfully. */
        return WX_RS422I_CREATE_TASK_FAIL_TXSEMAPHORE_NULL;
    }
    wx_log(WX_DBG, "Create RS422 task success!");
    return WX_SUCCESS;
}