#include "wx_include.h"
#include "wx_rs422_i_salve_task.h"

WxRs422ISlaveTask g_wxRs422ISlaveTask = {0};
/*
 * This is the configuration of the RS422I-master
 **/
WxRs422ISlaveTaskCfgInfo g_wxRs422ISlaveCfgInfo = {
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

WxFailCode WX_RS422I_Slave_DecodeAdu()
{

}

WxFailCode WX_RS422I_Slave_SendAdu(WxRs422ISlaveTask *this, WxModbusAdu *adu)
{
    ;
}

/* 发送异常响应 */
WxFailCode WX_RS422I_Slave_SendExcpRsp(WxRs422ISlaveTask *this, UINT8 funcCode, UINT8 excpCode)
{

}

/* 发送读数据响应， 如果失败请务必设置异常码以便外部统一处理 */
WxFailCode WX_RS422I_Slave_ProcReadDataReq(WxRs422ISlaveTask *this, WxRs422ISlaveMsg *req)
{
    WxRs422ISlaveDataReadHandle *handle = WX_RS422I_Slave_GetDataReadHandle(req->dataAddr);
    if (handle == NULL) {
        req->excpCode = WX_RS422_SLAVE_EXCP_CODE_UNSPT_DATA_ADDR;
        return WX_RS422I_SLAVE_DATA_READ_HANDLE_NULL;
    }
    /* 首先调用handle获取数据 */
    if (handle->readData == NULL || handle->encAdu == NULL) {
        req->excpCode = WX_RS422_SLAVE_EXCP_CODE_RD_HANDLE_UNFINE;
        return WX_RS422I_SLAVE_DATA_READ_HANDLE_UNFINE;
    }
    /* 获取数据 */
    WxFailCode rc = handle->readData(&req->data);
    if (rc != WX_SUCCESS) {
        req->excpCode = WX_RS422_SLAVE_EXCP_CODE_RD_DATA_GET_FAIL;
        return WX_RS422I_SLAVE_DATA_READ_GET_FAIL;
    }
    /* 数据编码 */
    rc = handle->readData(&req->data, &this->txAdu);
    if (rc != WX_SUCCESS) {
        req->excpCode = WX_RS422_SLAVE_EXCP_CODE_RD_DATA_ENC_FAIL;
        return WX_RS422I_SLAVE_DATA_READ_ENC_FAIL;
    }

    rc = WX_RS422I_Slave_SendAdu(this, &this->txAdu);
    if (rc != WX_SUCCESS) {
        req->excpCode = WX_RS422_SLAVE_EXCP_CODE_RD_SEND_RSP_FAIL;
        wx_excp_cnt(WX_EXCP_RS422_SLAVE_RD_SEND_ADU_FAIL);
        return rc;
    }

    return rc;
}

/* 从机处理写数据请求 */
WxFailCode WX_RS422I_Slave_ProcWriteDataReq(WxRs422ISlaveTask *this, WxRs422ISlaveMsg *req)
{
    /* 获取写Handle */
    WxRs422ISlaveDataWriteHandle *handle = WX_RS422I_Slave_GetDataWriteHandle(req->dataAddr);
    if (handle == NULL) {
        req->excpCode = WX_RS422_SLAVE_EXCP_CODE_UNSPT_DATA_ADDR;
        return WX_RS422I_SLAVE_DATA_WRITE_HANDLE_NULL;
    }
    /* 检查Handle */
    if (handle->decAdu == NULL || handle->writeData == NULL) {
        req->excpCode = WX_RS422_SLAVE_EXCP_CODE_WR_HANDLE_UNFINE;
        return WX_RS422I_SLAVE_DATA_WRITE_HANDLE_UNFINE;
    }
    /* 数据解码 */
    WxFailCode rc = handle->decAdu(&this->rxAdu, &req->data);
    if (rc != WX_SUCCESS) {
        req->excpCode = WX_RS422_SLAVE_EXCP_CODE_WR_DEC_FAIL;
        return WX_RS422I_SLAVE_DATA_WRITE_LOCAL_FAIL;
    }
    /* 数据写到本地 */
    rc = handle->writeData(&req->data, &this->txAdu);
    if (rc != WX_SUCCESS) {
        req->excpCode = WX_RS422_SLAVE_EXCP_CODE_WR_DATA_FAIL;
        return WX_RS422I_SLAVE_DATA_WRITE_DEC_FAIL;
    }

    /* 把数据原封不动的发送处理 */
    rc = WX_RS422I_Slave_SendAdu(this, &this->rxAdu);
    if (rc != WX_SUCCESS) {
        req->excpCode = WX_RS422_SLAVE_EXCP_CODE_RD_SEND_RSP_FAIL;
        wx_excp_cnt(WX_EXCP_RS422_SLAVE_WR_SEND_ADU_FAIL);
        return rc;
    }

    return rc;
}

WxFailCode WX_RS422I_Slave_ProcReadFileReq(WxRs422ISlaveTask *this, WxRs422ISlaveMsg *req)
{

}

WxFailCode WX_RS422I_Slave_ProcWriteFileReq(WxRs422ISlaveTask *this, WxRs422ISlaveMsg *req)
{

}
/* 从机处理主机的数据请求 */
WxFailCode WX_RS42I_Slave_ProcMsg(WxRs422ISlaveTask *this, WxRs422ISlaveMsg *req)
{
    WxFailCode rc = WX_SUCCESS;
    /* 如果数据请求处理失败则会填写错误消息码，直接发送错误响应即可 */
    if (WX_MODBUS_IS_EXCP_FUNC_CODE(req->funcCode)) {
        return WX_RS422I_Slave_SendExcpRsp(this, req->funcCode, req->excpCode);
    }
    /* 根据请求发送响应 */
    switch (req->funcCode) {
        case WX_MODBUS_FUNC_CODE_READ_DATA:
            rc = WX_RS422I_Slave_ProcReadDataReq(this, req);
            break;
        case WX_MODBUS_FUNC_CODE_WRITE_DATA:
            rc = WX_RS422I_Slave_ProcWriteDataReq(this, req);
            break;
        case WX_MODBUS_FUNC_CODE_READ_FILE:
            rc = WX_RS422I_Slave_ProcReadFileReq(this, req);
            break;
        case WX_MODBUS_FUNC_CODE_WRITE_FILE:
            rc = WX_RS422I_Slave_ProcWriteFileReq(this, req);
            break;
        default: {
            req->excpCode = WX_RS422_SLAVE_EXCP_CODE_UNSPT_FUNC_CODE;
            rc = WX_ERR;
            break;
        }
    }
    /* 如果发送响应失败，则发送异常响应 */
    if (rc != WX_SUCCESS) {
        return WX_RS422I_Slave_SendExcpRsp(this, req->funcCode + 0x80, req->excpCode);
    }

    return WX_SUCCESS;
}
/* 用于消息收发 */
VOID WX_RS422I_Slave_MainTask(VOID *pvParameters)
{
    WxRs422ISlaveTask *this = &g_wxRs422ISlaveTask;
    UINT32 recvCount;
    WxFailCode rc;
    while(1) {
         /* 清空并预设预期接收的报文大小（256），防止任务被抢占来不及缓存导致串口消息丢失 */
        do {
            recvCount = XUartNs550_Recv(&this->rs422Inst, this->rxAdu.adu, WX_MODBUS_ADU_MAX_SIZE);
        } while (recvCount);

        /* 这里会阻塞等待接收完成，如果长时间不完成则认为是异常 */
        if (xSemaphoreTake(this->aduRxFinishSemaphore, (TickType_t)portMAX_DELAY) == pdTRUE) {
            /* 解码接收到ADU，解码结果写到dataReq */
            rc = WX_RS422I_Slave_DecodeAdu(&this->rxAdu, &this->msg);
            if (rc != WX_SUCCESS) {
                continue;
            }

            /* 处理dataReq请求(内部会发送响应) */
            rc = WX_RS42I_Slave_ProcMsg(this, &this->msg);
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