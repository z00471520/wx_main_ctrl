#include "wx_include.h"
#include "wx_rs422_slave_driver.h"
#include "wx_id_def.h"
#include "wx_rs422_slave_driver_tx_adu_req_intf.h"
#include "wx_rs422_slave_rx_adu_req_intf.h"
#include "wx_uart_ns50.h"
#include "wx_msg_res_pool.h"
 #include "wx_frame.h"
WxRs422SlaveDriverCfg g_rs422SlaverDriverCfg = {0};

/* 发送接收到的报文给上层处理 */
VOID WX_RS422SlaveDriver_SentRxAdu2Upper(WxRs422SlaverDriver *this, WxModbusAdu *rxAdu)
{
    /* 申请消息 */
	WxRs422SlaveRxAduReq *msg = WX_ApplyEvtMsg(WX_MSG_TYPE_RS422_SLAVE_RX_ADU_REQ);
    if (msg == NULL) {
        return;
    }

    /* 初始化消息头 */
    WX_CLEAR_OBJ((WxMsg *)msg);

    /* 填写消息信息 */
    msg->sender = this->moduleId;
    msg->receiver = WX_MODULE_RS422_SLAVE;
    msg->msgType = WX_MSG_TYPE_RS422_SLAVE_RX_ADU_REQ;
    msg->rxAdu.failCode = WX_SUCCESS;
    for (UINT8 i = 0; i < rxAdu->valueLen; i++) {
        msg->rxAdu.value[i] = rxAdu->value[i];
    }
    msg->rxAdu.valueLen = rxAdu->valueLen;

    /* 发送消息 */
    UINT32 ret = WX_MsgShedule(this->moduleId, msg->receiver, msg);
    if (ret != WX_SUCCESS) {
        WX_FreeEvtMsg(&msg);
    }
}

/* 是否是支持的功能码 */
BOOL WX_RS422SlaveDriver_IsSupportFuncCode(WxRs422SlaverDriver *this, UINT8 funcCode)
{
    return (funcCode == WX_MODBUS_FUNC_CODE_READ_DATA ||
            funcCode == WX_MODBUS_FUNC_CODE_WRITE_DATA ||
            funcCode == WX_MODBUS_FUNC_CODE_READ_FILE ||
            funcCode == WX_MODBUS_FUNC_CODE_WRITE_FILE);

}
/* 处理接收到的PDU */
VOID WX_RS422SlaveDriver_ProcRecvAduFromISR(WxRs422SlaverDriver *this)
{
    /* 检查接收到的报文 */
    if (this->rxAdu.valueLen < WX_MODBUS_ADU_MIN_SIZE) {
        wx_excp_cnt(WX_EXCP_RS422_SLAVE_RECV_DATA_LEN_ERR);
        return;
    }
    /* 检查地址 */
    UINT8 slaveAddr = this->rxAdu.value[WX_MODBUS_SLAVE_ADDR_IDX];
    if (slaveAddr != this->slaveAddr) {
        wx_excp_cnt(WX_EXCP_RS422_SLAVE_RECV_DATA_ADDR_ERR);
        return;
    }
    /* 检查CRC */
    if (WX_Modbus_AduCrcCheck(&this->rxAdu) != WX_SUCCESS) {
        wx_excp_cnt(WX_EXCP_RS422_SLAVE_RECV_DATA_CRC_ERR);
        return;
    }
    UINT8 funcCode = this->rxAdu.value[WX_MODBUS_FUNC_CODE_IDX];
    if (!WX_RS422SlaveDriver_IsSupportFuncCode(this, this->rxAdu.funcCode)) {
        wx_excp_cnt(WX_EXCP_RS422_SLAVE_RECV_DATA_FUNC_CODE_ERR);
        /* 发送异常响应报文 */
        WX_Modbus_AduGenerateExceptionRsp(&this->txAdu, slaveAddr, funcCode,
        	WX_MODBUS_EXCP_ILLEGAL_FUNCTION);
        /* 首次发送消息会被缓存到实例，返回缓存了多少报文 */
        UINT32 sendCount = XUartNs550_Send(&this->rs422Inst, this->txAdu.value,
        	(unsigned int)this->txAdu.valueLen);
        if (sendCount == 0) {
            wx_excp_cnt(WX_EXCP_RS422_SLAVE_SEND_DATA_FAIL);
        }
        return;
    }

    /* 报文发送给上层处理 */
    WX_RS422SlaveDriver_SentRxAdu2Upper(this, &this->rxAdu);
    return;
}


/* RS422I中断处理函数 */
VOID WX_RS422SlaveDriver_IntrHandle(VOID *callBackRef, UINT32 event, UINT32 eventData)
{
    WxRs422SlaverDriver *this = callBackRef;
    /* 从机数据发送完毕转换为接收 */
	if (event == XUN_EVENT_SENT_DATA) {
        UINT32 recvCount = XUartNs550_Recv(&this->rs422Inst, this->rxAdu.value, WX_MODBUS_ADU_MAX_SIZE);
        if (recvCount == 0) {
            wx_excp_cnt(WX_EXCP_RS422_SLAVE_RECV_DATA_TX_FINISH);
        }
	}

    /* All of the data has been received. or Data was received, but not the expected number of bytes */
	if (event == XUN_EVENT_RECV_DATA || event ==XUN_EVENT_RECV_TIMEOUT) {
        this->rxAdu.valueLen = eventData;
        WX_RS422SlaveDriver_ProcRecvAduFromISR(this);
	}
}

UINT32 WX_RS422SlaveDriver_Construct(VOID *module)
{
    UINT32 ret;
    /* allocate memory for module */
    WxRs422SlaverDriver *this = WX_Mem_Alloc(WX_GetModuleName(module), 1, sizeof(WxRs422SlaverDriver));
    if (this == NULL) {
        return WX_ERR;
    }
    WxRs422SlaveDriverCfg *cfg = &g_rs422SlaverDriverCfg;
    /* the inst or rs422 used for uart data tx/rx */
    ret = WX_InitUartNs550(&this->rs422Inst, cfg->rs422DevId, &cfg->rs422Format);
    if (ret != WX_SUCCESS) {
        return ret;
    }

    /* 设置中断 */
    ret = WX_SetupUartNs550Interrupt(&this->rs422Inst, WX_RS422SlaveDriver_IntrHandle, cfg->intrId, this);
    if (ret != WX_SUCCESS) {
        return ret;
    }

    /* 接收准备 */
    UINT32 recvCount; 
    do {
        recvCount = XUartNs550_Recv(&this->rs422Inst, this->rxAdu.value, WX_MODBUS_ADU_MAX_SIZE);
    } while (recvCount);

    /* 设置上Module */
    WX_SetModuleInfo(module, this);
    return WX_SUCCESS;
}

/* 处理发送报文请求 */
UINT32 WX_RS422SlaveDriver_ProcTxAduReq(WxRs422SlaverDriver *this, WxRs422SlaverDriverTxAduReq *msg)
{
    /* 首次发送消息会被缓存到实例，返回缓存了多少报文 */
    UINT32 sendCount = XUartNs550_Send(&this->rs422Inst, this->txAdu.value, this->txAdu.valueLen);
    if (sendCount == 0) {
        wx_excp_cnt(WX_EXCP_RS422_SLAVE_SEND_DATA_FAIL_0);
    }

    return WX_SUCCESS;
}

/* 消息处理入口函数 */
UINT32 WX_RS422SlaveDriver_Entry(VOID *module, WxMsg *evtMsg)
{
    WxRs422SlaverDriver *this = WX_GetModuleInfo(module);
    switch (evtMsg->msgType) {
        case WX_MSG_TYPE_RS422_SLAVE_TX_ADU_REQ: {
            return WX_RS422SlaveDriver_ProcTxAduReq(this, (WxRs422SlaverDriverTxAduReq *)evtMsg);
        }
        default: {
            wx_excp_cnt(WX_EXCP_RS422_SLAVE_UNSPT_MSG_TYPE);
            return WX_RS422_SLAVE_DRIVER_UNSPT_MSG_TYPE;
        } 
    }
}

UINT32 WX_RS422SlaveDriver_Destruct(VOID *module)
{
    WxRs422SlaverDriver *this = WX_GetModuleInfo(module);
    if (this == NULL) {
        return WX_SUCCESS;
    }
    WX_Mem_Free(this);
    WX_SetModuleInfo(module, NULL);
    return WX_SUCCESS;
}
