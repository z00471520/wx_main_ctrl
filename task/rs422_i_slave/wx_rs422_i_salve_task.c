#include "wx_include.h"
#include "wx_rs422_i_salve_task.h"

WxRs422ISlaveTask g_wxRs422ISlaveTask = {0};

/*
 * This is the configuration of the RS422I-master
 **/
WxRs422ISlaveTaskCfgInfo g_wxRs422ISlaveCfgInfo = {
    .waitTxFinishBlockTime = 0, /* portMAX_DELAY: 死等 */
    .waitRxFinishBlockTime = portMAX_DELAY,
    .salveAddr = 10,
    .rs422DevId = 0,
    .taskPri = 5,
    .rs422Format.BaudRate = 9600,
    .rs422Format.DataBits = XUN_FORMAT_8_BITS,
    .rs422Format.Parity = XUN_FORMAT_NO_PARITY,
    .rs422Format.StopBits = XUN_FORMAT_1_STOP_BIT,
    .rs422IntrCfg.handle = WX_RS422I_Master_IntrHandler,
    .rs422IntrCfg.callBackRef = &g_wxRs422ISlaveCfgInfo,
    .rs422IntrCfg.intrId = , /* 中断ID */
};
/* 等待发送完成的定时器 portMAX_DELAY: 死等 */
#define WX_RS422I_SLAVE_WAIT_TX_FINISH_TIME  (g_wxRs422ISlaveCfgInfo.waitTxFinishBlockTime) 
#define WX_RS422I_SLAVE_WAIT_RX_FINISH_TIME  (g_wxRs422ISlaveCfgInfo.waitRxFinishBlockTime)
#define WX_RS422I_SLAVE_ADDR                 (g_wxRs422ISlaveCfgInfo.salveAddr)  

/* 发送ADU */
WxFailCode WX_RS422I_Slave_SendAdu(WxRs422ISlaveTask *this, WxModbusAdu *txAdu)
{
    /* 首次发送消息会被缓存到实例，返回缓存了多少报文 */
    unsigned int sendCount = XUartNs550_Send(&this->rs422Inst, txAdu->adu, (unsigned int)txAdu->valueLen);
    if (sendCount == 0) {
        /* 是不可能出现发送0情况，这里算是异常了 */
        return WX_RS422I_SLAVE_SEND_ADU_FAIL;
    }

    /* 这里会阻塞等待发送完成，如果长时间不完成则认为是异常 */
    if (xSemaphoreTake(this->aduTxFinishSemaphore, (TickType_t)WX_RS422I_SLAVE_WAIT_TX_FINISH_TIME) == FALSE) {
        if (WX_RS422I_SLAVE_WAIT_TX_FINISH_TIME == 0) {
            return WX_SUCCESS;
        }
        return WX_RS422I_SLAVE_SEND_ADU_TIMEOUT,
    }

    return WX_SUCCESS;
}

/* 发送异常响应 */
WxFailCode WX_RS422I_Slave_SendExcpRsp(WxRs422ISlaveTask *this, WxRs422ISlaveMsg *req, UINT8 excpCode)
{
    this->txAdu.valueLen = 0;
    
}

/* 发送读数据响应， 如果失败请务必设置异常码以便外部统一处理 */
WxFailCode WX_RS422I_Slave_ProcReadDataReq(WxRs422ISlaveTask *this, WxModbusAdu *rxAdu, WxRs422ISlaveMsg *req)
{
    WxFailCode rc;
    /* 获取读数据的handle */
    WxRs422ISlaveDataReadHandle *handle = WX_RS422I_Slave_GetDataReadHandle(req->dataAddr);
    if (handle == NULL) {
        req->excpCode = WX_RS422_SLAVE_EXCP_CODE_UNSPT_DATA_ADDR;
        return WX_RS422I_SLAVE_DATA_READ_HANDLE_NULL;
    }
    /* 检查handle中的钩子函数是否提供 */
    if (handle->readData == NULL || handle->encAdu == NULL) {
        return WX_RS422I_Slave_SendExcpRsp(this, req, WX_RS422_SLAVE_EXCP_CODE_RD_HANDLE_UNFINE);
    }
    /* 获取数据 */
    rc = handle->readData(&req->data);
    if (rc != WX_SUCCESS) {
        return WX_RS422I_Slave_SendExcpRsp(this, req, WX_RS422_SLAVE_EXCP_CODE_RD_DATA_GET_FAIL);
    }
    /* 数据编码 */
    rc = handle->encAdu(&req->data, req, &this->txAdu);
    if (rc != WX_SUCCESS) {
        return WX_RS422I_Slave_SendExcpRsp(this, req, WX_RS422_SLAVE_EXCP_CODE_RD_DATA_ENC_FAIL);
    }

    /* 这里不返回失败 */
    rc = WX_RS422I_Slave_SendAdu(this, &this->txAdu);
    if (rc != WX_SUCCESS) {
        wx_excp_cnt(WX_EXCP_RS422_SLAVE_RD_SEND_ADU_FAIL);
    }

    return rc;
}

/* 从机处理写数据请求 */
WxFailCode WX_RS422I_Slave_ProcWriteDataReq(WxRs422ISlaveTask *this, WxModbusAdu *rxAdu, WxRs422ISlaveMsg *req)
{
    /* 获取写Handle */
    WxRs422ISlaveDataWriteHandle *handle = WX_RS422I_Slave_GetDataWriteHandle(req->dataAddr);
    if (handle == NULL) {
        return WX_RS422I_Slave_SendExcpRsp(this, req, WX_RS422_SLAVE_EXCP_CODE_UNSPT_DATA_ADDR);
    }
    /* 检查Handles是否为空 */
    if (handle->decAdu == NULL || handle->writeData == NULL) {
        return WX_RS422I_Slave_SendExcpRsp(this, req, WX_RS422_SLAVE_EXCP_CODE_WR_HANDLE_UNFINE);
    }
    /* 数据解码 */
    WxFailCode rc = handle->decAdu(&this->rxAdu, &req->data);
    if (rc != WX_SUCCESS) {
        return WX_RS422I_Slave_SendExcpRsp(this, req, WX_RS422_SLAVE_EXCP_CODE_WR_DEC_FAIL);
    }
    /* 数据写到本地 */
    rc = handle->writeData(&req->data);
    if (rc != WX_SUCCESS) {
        return WX_RS422I_Slave_SendExcpRsp(this, req, WX_RS422_SLAVE_EXCP_CODE_WR_DATA_FAIL);
    }
    /* 把数据原封不动的发送处理 */
    this->txAdu = this->rxAdu;
    rc = WX_RS422I_Slave_SendAdu(this, &this->txAdu);
    if (rc != WX_SUCCESS) {
        wx_excp_cnt(WX_EXCP_RS422_SLAVE_WR_SEND_ADU_FAIL);
    }

    return rc;
}

/* 从机处理写文件请求 */
WxFailCode WX_RS422I_Slave_ProcReadFileReq(WxRs422ISlaveTask *this, WxModbusAdu *rxAdu, WxRs422ISlaveMsg *req)
{
    req->excpCode = WX_RS422_SLAVE_EXCP_CODE_RD_FILE_UNSPT;
    return WX_RS422I_SLAVE_FILE_READ_UNSPT;
}

/* 从机处理读文件请求 */
WxFailCode WX_RS422I_Slave_ProcWriteFileReq(WxRs422ISlaveTask *this, WxModbusAdu *rxAdu, WxRs422ISlaveMsg *req)
{
    req->excpCode = WX_RS422_SLAVE_EXCP_CODE_WR_FILE_UNSPT;
    return WX_RS422I_SLAVE_FILE_WRITE_UNSPT;
}

/* 从机处理接收到ADU */
WxFailCode WX_RS422I_Slave_ProcRxAdu(WxRs422ISlaveTask *this, WxModbusAdu *rxAdu)
{
    WxFailCode rc = WX_SUCCESS;
    /* 检查长度 */
    if (rxAdu->valueLen < WX_MODBUS_ADU_MIN_SIZE) {
        /* 此处增加计数器 */
        return WX_RS422I_SLAVE_RECV_ADU_UNEXPECT_LEN;
    }
    /* msg 用于存储结果解码 */
    WxRs422ISlaveMsg *req = &this->msg;
    /* 初始化为未知错误 */
    req->excpCode = WX_RS422_SLAVE_EXCP_CODE_UNKNOWN;
    /* 不是本设备的从机地址则不处理 */
    req->slaveAddr = rxAdu->value[WX_MODBUS_ADU_SLAVE_ADDR_IDX];
    if (req->slaveAddr != WX_RS422I_SLAVE_ADDR) {
        return WX_SUCCESS;
    }
    req->funcCode = rxAdu->value[WX_MODBUS_ADU_FUNC_CODE_IDX];
    /* CRC校验失败发送错误影响 */
    if (WX_Modbus_AduCrcCheck(rxAdu) != WX_SUCCESS) {
        return WX_RS422I_Slave_SendExcpRsp(this, req, WX_RS422_SLAVE_EXCP_CODE_CRC_ERR);
    }

    /* 根据请求发送响应 */
    switch (req->funcCode) {
        case WX_MODBUS_FUNC_CODE_READ_DATA:
            return WX_RS422I_Slave_ProcReadDataReq(this, rxAdu, req);
        case WX_MODBUS_FUNC_CODE_WRITE_DATA:
            return WX_RS422I_Slave_ProcWriteDataReq(this, rxAdu, req);
        case WX_MODBUS_FUNC_CODE_READ_FILE:
            return WX_RS422I_Slave_ProcReadFileReq(this, rxAdu, req);
        case WX_MODBUS_FUNC_CODE_WRITE_FILE:
            return WX_RS422I_Slave_ProcWriteFileReq(this, rxAdu, req);
        default: {
            return WX_RS422I_Slave_SendExcpRsp(this, req, WX_RS422_SLAVE_EXCP_CODE_UNSPT_FUNC_CODE);
        }
    }
}

/* 初始化消息  */
VOID WX_RS422I_Slave_InitSlaveMsg(WxRs422ISlaveMsg *this)
{
    this->dataAddr = 0;
    this->funcCode = 0;
    this->excpCode = WX_RS422_SLAVE_EXCP_CODE_OK;
}

/* 用于消息收发 */
VOID WX_RS422I_Slave_MainTask(VOID *pvParameters)
{
    WxRs422ISlaveTask *this = &g_wxRs422ISlaveTask;
    UINT32 recvCount;
    WxFailCode rc;
    UINT8 slaveAddr;
    while(1) {
         /* 清空并预设预期接收的报文大小（256），防止任务被抢占来不及缓存导致串口消息丢失 */
        do {
            recvCount = XUartNs550_Recv(&this->rs422Inst, this->rxAdu.adu, WX_MODBUS_ADU_MAX_SIZE);
        } while (recvCount);

        /* 这里会阻塞等待接收完成，如果长时间不完成则认为是异常 */
        if (xSemaphoreTake(this->aduRxFinishSemaphore, (TickType_t)WX_RS422I_SLAVE_WAIT_RX_FINISH_TIME) == pdTRUE) {
            
            /* 处理dataReq请求(内部会发送响应) */
            rc = WX_RS422I_Slave_ProcRxAdu(this, &this->rxAdu);
            if (rc != WX_SUCCESS) {
                continue;
            }
        }
       
    }
}

/* 创建RS422I主机任务 */
WxFailCode WX_RS422I_Slave_CreateTask(VOID)
{
    WxFailCode rc;
    WxRs422ISlaveTaskCfgInfo *cfg = &g_wxRs422ISlaveCfgInfo;
    WxRs422ISlaveTask *this = &g_wxRs422ISlaveTask;
    /* create the msg que to buff the msg to tx */
    if (this->msgQueHandle == 0) {
        this->msgQueHandle = xQueueCreate(WX_RS422I_MASTER_MSG_ITERM_MAX_NUM, sizeof(WxRs422IMasterMsg));
        if (this->msgQueHandle == 0) {
            wx_log(WX_CRITICAL, "Error Exit: xQueueCreate fail");
            return WX_RS422I_Master_CREATE_MSG_QUE_FAIL;
        }
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

    /* 创建任务 */
    
    wx_log(WX_DBG, "Create RS422 task success!");
    return WX_SUCCESS;
}