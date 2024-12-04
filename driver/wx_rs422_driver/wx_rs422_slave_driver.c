#include "wx_include.h"
#include "wx_rs422_slave_driver.h"
#include "wx_id_def.h"
#include "wx_rs422_slave_driver_tx_adu_req_intf.h"
#include "wx_rs422_slave_rx_adu_req_intf.h"
#include "wx_uart_ns550.h"
#include "wx_msg_res_pool.h"
 #include "wx_frame.h"
WxRs422SlaveDriverCfg g_rs422SlaverDriverCfg = {
    .moduleId = WX_MODULE_RS422_SLAVE_DRIVER,
    .upperModuleId = WX_MODULE_RS422_SLAVE,
    .gpioDevId = XPAR_GPIO_1_DEVICE_ID,
    .intrId = XPAR_FABRIC_UARTNS550_1_VEC_ID,     /* interrupt ID */
    .slaveAddr = 0x01,   /* slave address of the device */
    .rs422DevId = XPAR_UARTNS550_1_DEVICE_ID,
    .rs422Format = {
        .DataBits = XUN_FORMAT_8_BITS,
        .StopBits = XUN_FORMAT_1_STOP_BIT,
        .Parity = XUN_FORMAT_NO_PARITY,
        .BaudRate = 115200
    },
};

VOID WX_RS422SlaveDriver_SentRxAdu2Upper(WxRs422SlaverDriver *this, WxModbusAdu *rxAdu)
{
    /* 鐢宠娑堟伅 */
	WxRs422SlaveRxReqAdu *msg = WX_ApplyEvtMsg(WX_MSG_TYPE_RS422_SLAVE_RX_ADU_REQ);
    if (msg == NULL) {
        wx_excp_cnt(WX_EXCP_RS422_SLAVE_MALLOC_MSG_FAIL);
        return;
    }

    /* set the message content */
    msg->sender = this->moduleId;
    msg->receiver = this->upperModuleId;
    for (UINT8 i = 0; i < rxAdu->valueLen; i++) {
        msg->rxAdu.value[i] = rxAdu->value[i];
    }
    msg->rxAdu.valueLen = rxAdu->valueLen;

    /* send the message to the upper layer */
    UINT32 ret = WX_MsgShedule(this->moduleId, msg->receiver, msg);
    if (ret != WX_SUCCESS) {
        wx_fail_code_cnt(ret);
        WX_FreeEvtMsg((WxMsg **)&msg);
    }
}

/* is support function code */
BOOL WX_RS422SlaveDriver_IsSupportFuncCode(WxRs422SlaverDriver *this, UINT8 funcCode)
{
    return (funcCode == WX_MODBUS_FUNC_CODE_READ_DATA ||
            funcCode == WX_MODBUS_FUNC_CODE_WRITE_DATA ||
            funcCode == WX_MODBUS_FUNC_CODE_READ_FILE ||
            funcCode == WX_MODBUS_FUNC_CODE_WRITE_FILE);

}
/* proc recv adu from isr */
VOID WX_RS422SlaveDriver_ProcRecvAduFromISR(WxRs422SlaverDriver *this)
{
    /* invalid data length */
    if (this->rxAdu.valueLen < WX_MODBUS_ADU_MIN_SIZE) {
        wx_excp_cnt(WX_EXCP_RS422_SLAVE_RECV_DATA_LEN_ERR);
        return;
    }
    /* get the slave address */
    UINT8 slaveAddr = this->rxAdu.value[WX_MODBUS_SLAVE_ADDR_IDX];
    if (slaveAddr != this->slaveAddr) {
        wx_excp_cnt(WX_EXCP_RS422_SLAVE_RECV_DATA_ADDR_ERR);
        return;
    }
    /* CRC check OF the received data */
    if (WX_Modbus_AduCrcCheck(&this->rxAdu) != WX_SUCCESS) {
        wx_excp_cnt(WX_EXCP_RS422_SLAVE_RECV_DATA_CRC_ERR);
        return;
    }
    UINT8 funcCode = this->rxAdu.value[WX_MODBUS_FUNC_CODE_IDX];
    if (!WX_RS422SlaveDriver_IsSupportFuncCode(this, this->rxAdu.funcCode)) {
        wx_excp_cnt(WX_EXCP_RS422_SLAVE_RECV_DATA_FUNC_CODE_ERR);
        /* Exception response */
        WX_Modbus_AduGenerateExceptionRsp(slaveAddr, funcCode,
        	WX_MODBUS_EXCP_ILLEGAL_FUNCTION, &this->txAdu);
        /* send the exception response */
        UINT32 sendCount = XUartNs550_Send(&this->rs422Inst, this->txAdu.value,
        	(unsigned int)this->txAdu.valueLen);
        if (sendCount == 0) {
            wx_excp_cnt(WX_EXCP_RS422_SLAVE_SEND_DATA_FAIL);
        }
        return;
    }

    /* check ok send the recevie adu to upper layer */
    WX_RS422SlaveDriver_SentRxAdu2Upper(this, &this->rxAdu);
    return;
}


/* interrupt handle function */
VOID WX_RS422SlaveDriver_IntrHandle(VOID *callBackRef, UINT32 event, UINT32 eventData)
{
    WxRs422SlaverDriver *this = callBackRef;
    /* data send finish switch to recv new adu from master */
	if (event == XUN_EVENT_SENT_DATA) {
        UINT32 recvCount = XUartNs550_Recv(&this->rs422Inst, this->rxAdu.value, WX_MODBUS_ADU_MAX_SIZE);
        if (recvCount == 0) {
            wx_excp_cnt(WX_EXCP_RS422_SLAVE_RECV_DATA_TX_FINISH);
        }
        /* 0 - rx, 1 - tx */
        XGpio_DiscreteWrite(&this->gpipInst, 1, 0);
	}

    /* All of the data has been received. or Data was received, but not the expected number of bytes */
	if (event == XUN_EVENT_RECV_DATA || event == XUN_EVENT_RECV_TIMEOUT) {
        this->rxAdu.valueLen = eventData;
        WX_RS422SlaveDriver_ProcRecvAduFromISR(this);
        /* 0 - rx, 1 - tx */
        XGpio_DiscreteWrite(&this->gpipInst, 1, 1);
	}
}

/* init the gpio */
UINT32 WX_RS422SlaveDriver_InitGpio(XGpio *gpipInstPtr, UINT32 gpioId)
{
	/* initial gpio */
	int status = XGpio_Initialize(gpipInstPtr, gpioId);
	if (status != XST_SUCCESS) {
        return WX_RS422_SLAVE_DRIVER_INIT_GPIO_FAIL;
    }

	/* set gpio as output */
	XGpio_SetDataDirection(gpipInstPtr, 1, 0x0);
    /* 0 - rx, 1 - tx */
    XGpio_DiscreteWrite(gpipInstPtr, 1, 0);

    return WX_SUCCESS;
}

UINT32 WX_RS422SlaveDriver_Construct(VOID *module)
{
    UINT32 ret;
    /* allocate memory for module */
    WxRs422SlaverDriver *this = WX_Mem_Alloc(WX_GetModuleName(module), 1, sizeof(WxRs422SlaverDriver));
    if (this == NULL) {
        return WX_RS422_SLAVE_DRIVER_MEM_ALLOC_FAIL;
    }
    WxRs422SlaveDriverCfg *cfg = &g_rs422SlaverDriverCfg;
    this->moduleId = cfg->moduleId;
    this->upperModuleId = cfg->upperModuleId;
    this->slaveAddr = cfg->slaveAddr;

    /* this is only for test */
    ret = WX_RS422SlaveDriver_InitGpio(&this->gpipInst, cfg->gpioDevId);
    if (ret != WX_SUCCESS) {
        boot_debug("Error: RS422 slave driver init gpio fail(%u)", ret);
        return ret;
    }

    /* the inst or rs422 used for uart data tx/rx */
    ret = WX_InitUartNs550(&this->rs422Inst, cfg->rs422DevId, &cfg->rs422Format);
    if (ret != WX_SUCCESS) {
        return ret;
    }

    /* set up the interrupt */
    ret = WX_SetupUartNs550Interrupt(&this->rs422Inst, WX_RS422SlaveDriver_IntrHandle, cfg->intrId, this);
    if (ret != WX_SUCCESS) {
        return ret;
    }

    /* prepare to receive data */
    UINT32 recvCount; 
    do {
        recvCount = XUartNs550_Recv(&this->rs422Inst, this->rxAdu.value, WX_MODBUS_ADU_MAX_SIZE);
    } while (recvCount);

    /* set the module information */
    WX_SetModuleInfo(module, this);
    return WX_SUCCESS;
}

/* proc tx adu request from upper layer and send to master */
UINT32 WX_RS422SlaveDriver_ProcTxAduReq(WxRs422SlaverDriver *this, WxRs422SlaverDriverTxAduReq *msg)
{
    /* send the data to master */
    UINT32 sendCount = XUartNs550_Send(&this->rs422Inst, this->txAdu.value, this->txAdu.valueLen);
    if (sendCount == 0) {
        wx_excp_cnt(WX_EXCP_RS422_SLAVE_SEND_DATA_FAIL_0);
    }

    return WX_SUCCESS;
}

/* entry function */
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

/* destruct function */
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
