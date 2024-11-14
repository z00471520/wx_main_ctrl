#include "wx_include.h"
#include "wx_rs422_i_master_task.h"
#include "wx_uart_ns550.h"
WxRs422IMasterTask g_wxRs422IMasterTask = {0};

/*
 * This is the configuration of the RS422I-master
 **/
WxRs422IMasterTaskCfgInfo g_wxRs422IMasterCfgInfo = {
    .rs422DevId = 0,
    .taskPri = 5,
    .rs422Format.BaudRate = 9600,
    .rs422Format.DataBits = XUN_FORMAT_8_BITS,
    .rs422Format.Parity = XUN_FORMAT_NO_PARITY,
    .rs422Format.StopBits = XUN_FORMAT_1_STOP_BIT,
    .rs422IntrCfg.handle = WX_RS422I_Master_IntrHandler,
    .rs422IntrCfg.callBackRef = &g_wxRs422IMasterCfgInfo,
    .rs422IntrCfg.intrId = , /* 中断ID */
};

/* RS422I中断处理函数 */
void WX_RS422I_Master_IntrHandler(void *CallBackRef, u32 Event, unsigned int EventData)
{
    WxRs422IMasterTask *this = CallBackRef;
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

WxFailCode WX_RS422I_Master_EncodeAdu(WxRs422IMasterMsg *txMsg, WxRs422IAdu *txAdu)
{
    switch (txMsg->msgType) {
        case WX_RS422I_Master_MSG_READ_DATA:
            return WX_RS422I_Master_EncodeAduReadDataReq(txMsg, txAdu);
        case WX_RS422I_Master_MSG_WRITE_DATA: {
            return WX_RS422I_Master_EncodeAduWriteDataReq(txMsg, txAdu);
        }
        /* if more pleace add here */
        default: {
            return WX_RS422I_Master_UNSPT_TX_MSGTYPE;
        }
    }
}


/* 这个函数会阻塞直到发送完成或者异常 */
WxFailCode WX_RS422I_Master_TxAdu(WxRs422IMasterTask *this, WxModbusAdu *txAdu)
{
    /* 清空并预设预期接收的报文大小，防止任务被抢占来不及缓存导致串口消息丢失 */
    UINT32 recvCount; 
    do {
        recvCount = XUartNs550_Recv(&this->rs422Inst, rxAdu->adu, txAdu->expectRspLen);
    } while (recvCount);

    /* 首次发送消息会被缓存到实例，返回缓存了多少报文 */
    unsigned int sendCount = XUartNs550_Send(&this->rs422Inst, txAdu->adu, (unsigned int)txAdu->aduLen);
    if (sendCount == 0) {
        /* 是不可能出现发送0情况，这里算是异常了 */
        return WX_RS422I_Master_SNED_ADU_BUFFER_FAIL;
    }

    /* 这里会阻塞等待发送完成，如果长时间不完成则认为是异常 */
    if (xSemaphoreTake(this->aduTxFinishSemaphore, (TickType_t)WX_RS422I_MASTER_WAIT_TX_FINISH_TIME) == FALSE) {
        return WX_RS422I_Master_SEND_ADU_TIMEOUT,
    }

    return WX_SUCCESS;
}


/* 接收报文 */
WxFailCode WX_RS422I_Master_RxAdu(WxRs422IMasterTask *this)
{
    /* 这里会阻塞等待接收完成，如果长时间不完成则认为是异常 */
    if (xSemaphoreTake(this->aduRxFinishSemaphore, (TickType_t)WX_RS422I_MASTER_WAIT_RX_FINISH_TIME) == pdFALSE) {
        return WX_RS422I_Master_SEND_ADU_TIMEOUT,
    }

    return WX_SUCCESS;    
}

WxMsgType WX_RS422I_Master_GetRspMsgType(WxMsgType reqMsgType)
{
    switch (reqMsgType) {
        case WX_RS422I_Master_MSG_READ_DATA: {
            return WX_RS422I_Master_MSG_READ_DATA_RSP;
        }
        case WX_RS422I_Master_MSG_WRITE_DATA: {
            return WX_RS422I_Master_MSG_WRITE_DATA_RSP;
        }
        case WX_RS422I_Master_MSG_READ_FILE: {
            return WX_RS422I_Master_MSG_READ_FILE_RSP;
        }
        case WX_RS422I_Master_MSG_WRITE_FILE: {
            return WX_RS422I_Master_MSG_WRITE_FILE_RSP;
        }
        default:
            wx_excp_cnt(WX_EXCP_UNEXPECT_MSG_TYPE); 
            return reqMsgType;
    }
}

WxFailCode WX_RS422I_Master_ProcMsg(WxRs422IMasterTask *this, WxRs422IMasterMsg *msg)
{
    WxFailCode rc = WX_RS422I_Master_EncodeAdu(&this->rs422Msg, &this->txAdu);
    if (rc != WX_SUCCESS) {
        return rc;
    }
    /*
     * send the adu by RS422 buf
     */
    rc = WX_RS422I_Master_TxAdu(this, &this->txAdu);
    if (rc != WX_SUCCESS) {
        return rc;
    }
    /* 请求和相应是一条消息，只需要把消息类型反转一下即可，没有必须新建一个消息 */
    this->rs422Msg.msgType = WX_RS422I_Master_GetRspMsgType(his->rs422Msg.msgType);
    
    /* wait to recieve the resbonse */
    rc = WX_RS422I_Master_RxAdu(this);
    if (rc != WX_SUCCESS) {
        return rc;
    }
    
    /* decode the rx ADU into msg */
    rc = WX_RS422I_Master_DecodeAdu(this, this->rxAdu, &this->rs422Msg);
    if (rc != WX_SUCCESS) {
        return rc;
    }

    return rc;
}

/* 用于消息收发 */
VOID WX_RS422I_Master_MainTask(VOID *pvParameters)
{
    WxRs422IMasterTask *this = &g_wxRs422IMasterTask;
    while(1) {
        /* block to receive the msg util the msg arrive */
        if (xQueueReceive(this->msgQueHandle, &this->rs422Msg, (TickType_t)portMAX_DELAY) == pdTRUE) {
            this->rs422Msg.failCode = WX_RS422I_Master_ProcMsg(this, &this->rs422Msg);
            
        }
    }
}

/* 创建RS422I主机任务 */
WxFailCode WX_RS422I_Master_CreateTask(VOID)
{
    WxFailCode rc;
    WxRs422IMasterTaskCfgInfo *cfg = &g_wxRs422IMasterCfgInfo;
    WxRs422IMasterTask *this = &g_wxRs422IMasterTask;
    /* create the msg que to buff the msg to tx */
    if (this->msgQueHandle == 0) {
        this->msgQueHandle = xQueueCreate(WX_RS422I_MASTER_MSG_ITERM_MAX_NUM, sizeof(WxRs422IMasterMsg));
        if (this->msgQueHandle == 0) {
            wx_log(WX_CRITICAL, "Error Exit: xQueueCreate fail");
            return WX_RS422I_Master_CREATE_MSG_QUE_FAIL;
        }
    }
    rc = WX_RegMsgQue(WX_MODULE_RS422_I_MASTER, this->msgQueHandle, sizeof(WxRs422IMasterMsg));
    if (rc != WX_SUCCESS) {
        return rc;
    }
    /* the inst or rs422 used for uart data tx/rx */
    rc = WX_InitUartNs550(&this->rs422Inst, cfg->rs422DevId, cfg->rs422Format);
    if (rc != WX_SUCCESS) {
        return rc;
    }

    /* 设置中断 */
    rc = WX_SetupUartNs550Interrupt(&this->rs422Inst, &cfg->rs422IntrCfg);
    if (rc != WX_SUCCESS) {
        return rc;
    }

    /* 该二进制信号量用于表达RS422是否把ADU消息全部发送完毕 */
    this->aduTxFinishSemaphore = xSemaphoreCreateBinary();
    if (this->aduTxFinishSemaphore == NULL) {
        /* There was insufficient FreeRTOS heap available for the semaphore to
           be created successfully. */
        return WX_RS422I_Master_CREATE_TASK_FAIL_TXSEMAPHORE_NULL;
    }

    /* 该二进制信号量用于表达RS422是否把ADU消息全部发送完毕 */
    this->aduRxFinishSemaphore = xSemaphoreCreateBinary();
    if (this->aduRxFinishSemaphore == NULL) {
        /* There was insufficient FreeRTOS heap available for the semaphore to
           be created successfully. */
        return WX_RS422I_Master_CREATE_TASK_FAIL_RXSEMAPHORE_NULL;
    }

    
    wx_log(WX_DBG, "Create RS422 task success!");
    return WX_SUCCESS;
}