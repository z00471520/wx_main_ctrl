#include "wx_include.h"
#include "wx_rs422_master_driver.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "wx_msg_common.h"
#include "wx_rs422_master_driver_intf.h"
#include "xuartns550.h"
#include "wx_uart_ns550.h"
#include "wx_msg_res_pool.h"
#include "wx_frame.h"
#include "xuartns550.h"
#include "wx_rs422_master_adu_rsp_intf.h"
WxRs422MasterDriverCfg g_rs422DriverMasterCfg = {
    .rs422DevId = XPAR_UARTNS550_0_DEVICE_ID,
    .upperModuleId = WX_MODULE_RS422_MASTER,
    .gpioDevId = XPAR_GPIO_0_DEVICE_ID,
    .rs422Format = {
        .DataBits = XUN_FORMAT_8_BITS,
        .StopBits = XUN_FORMAT_1_STOP_BIT,
        .Parity = XUN_FORMAT_NO_PARITY,
        .BaudRate = 115200
    },
   .intrId = XPAR_FABRIC_UARTNS550_0_VEC_ID,
   .msgQueItemNum = 128,
};

VOID WX_RS422MasterDriver_SentRspAdu2Rs422Master(WxRs422DriverMaster *this, WxModbusAdu *rxAdu)
{
    WxRs422MasterDriverRspMsg *rsp = (WxRs422MasterDriverRspMsg *)WX_ApplyEvtMsg(WX_MSG_TYPE_RS422_MASTER_ADU_RSP);
    if (rsp == NULL) {
        return;
    }

    /* init the data */
    rsp->data.rspAdu = *rxAdu;
    rsp->data.rspAdu.expectRspLen = this->txAduInfo.adu.expectRspLen;
    rsp->data.reqMsgType = this->txAduInfo.msgType;
    rsp->data.reqSubMsgType = this->txAduInfo.subMsgType;
    rsp->data.reqSender = this->txAduInfo.sender;
    rsp->sender = this->moduleId;
    rsp->receiver = this->upperModuleId; /* back the receiver to upper layer */
    
    /* send the msg */
    UINT32 ret = WX_MsgShedule(this->moduleId, this->upperModuleId, rsp);
    if (ret != WX_SUCCESS) {
        wx_fail_code_cnt(ret);
        WX_FreeEvtMsg((WxMsg **)&rsp);
    }
    return;
}

/* master proc rsp pdu */
VOID WX_RS422MasterDriver_ProcRecvAduFromISR(WxRs422DriverMaster *this)
{
    this->status = WX_RS422_MASTER_STATUS_IDLE;
    /* send the response adu to upper layer */
    WX_RS422MasterDriver_SentRspAdu2Rs422Master(this, &this->rxAdu);

    /* get the next tx adu from queue */
    WxRs422MasterDriverTxDataReq *txAduInfo = &this->txAduInfo;
    if (xQueueReceiveFromISR(this->msgQueHandle, txAduInfo, 0) == pdPASS) {
        /* before send clear the receive buff */
        UINT32 recvCount; 
        do {
            recvCount = XUartNs550_Recv(&this->rs422Inst, this->rxAdu.value,
            	(unsigned int)txAduInfo->adu.expectRspLen);
        } while (recvCount);
        
        /* send the tx adu by uart */
        UINT32 sendCount = XUartNs550_Send(&this->rs422Inst, txAduInfo->adu.value, (UINT32)txAduInfo->adu.valueLen);
        if (sendCount == 0) {
            /* this is a error */
            wx_excp_cnt(WX_EXCP_RS422_MASTER_DRIVER_ISR_SEND_BY_UART_FAIL);
            return;
        }
        this->status = WX_RS422_MASTER_STATUS_TX_ADU;
    }

    return;
}


/* RS422 master娑擃厽鏌囨径鍕倞閸戣姤鏆� */
VOID WX_RS422I_DRIVER_MASTER_IntrHandler(VOID *callBackRef, UINT32 event, UINT32 eventData)
{
    WxRs422DriverMaster *this = callBackRef;
    /*
	 * All of the data has been sent.
	 */
	if (event == XUN_EVENT_SENT_DATA) {
        /* 0 - rx, 1 - tx */
        XGpio_DiscreteWrite(&this->gpipInst, 1, 0);
        this->status = WX_RS422_MASTER_STATUS_RX_ADU;
	}

    /*
	 * All of the data has been received.
	 */
	if (event == XUN_EVENT_RECV_DATA) {
        
        this->rxAdu.valueLen = eventData;
        WX_RS422MasterDriver_ProcRecvAduFromISR(this);
        /* 0 - rx, 1 - tx */
        XGpio_DiscreteWrite(&this->gpipInst, 1, 1);
	}

	/*
	 * Data was received, but not the expected number of bytes, a
	 * timeout just indicates the data stopped for 4 character times.
	 */
	if (event == XUN_EVENT_RECV_TIMEOUT) {
        this->rxAdu.valueLen = eventData;
        WX_RS422MasterDriver_ProcRecvAduFromISR(this);
        /* 0 - rx, 1 - tx */
        XGpio_DiscreteWrite(&this->gpipInst, 1, 1);
	}
}

/* proccess the upper layer tx adu request msg */
UINT32 WX_RS422MasterDriver_ProcTxAduMsg(WxRs422DriverMaster *this, WxRs422MasterDriverMsg *msg)
{
    /* buffer the tx adu to queue for further process */
    if (this->status != WX_RS422_MASTER_STATUS_IDLE) {
        /* if not IDLE status we sent it buff */
        if (xQueueSend(this->msgQueHandle, &msg->reqTxData, 0) != pdPASS) {
            return WX_RS422_MASTER_CACHE_MSG_FAIL;
        }
        return WX_SUCCESS;
    }
    /* buff the msg to local send it by uartns555 */
    this->txAduInfo = msg->reqTxData;

    /* clear the receive buff before send */
    WxModbusAdu *rxAdu = &this->rxAdu;
    UINT32 recvCount; 
    do {
        recvCount = XUartNs550_Recv(&this->rs422Inst, rxAdu->value, this->txAduInfo.adu.expectRspLen);
    } while (recvCount);
    
    /* set to tx mode, 0 - rx, 1 - tx */
    XGpio_DiscreteWrite(&this->gpipInst, 1, 1);


    UINT32 sendCount = XUartNs550_Send(&this->rs422Inst, this->txAduInfo.adu.value, (UINT32)this->txAduInfo.adu.valueLen);
    if (sendCount == 0) {
        return WX_RS422_MASTER_DRIVER_SEND_BY_UART_FAIL;
    }
    this->status = WX_RS422_MASTER_STATUS_TX_ADU;
    return WX_SUCCESS;
}

/* init the gpio */
UINT32 WX_RS422MasterDriver_InitGpio(XGpio *gpipInstPtr, UINT32 gpioId)
{
	/* initial gpio */
	int status = XGpio_Initialize(gpipInstPtr, gpioId);
	if (status != XST_SUCCESS) {
        return WX_RS422_SLAVE_DRIVER_INIT_GPIO_FAIL;
    }

	/* set gpio as output */
	XGpio_SetDataDirection(gpipInstPtr, 1, 0x0);
    /* 0 - rx, 1 - tx */
    XGpio_DiscreteWrite(gpipInstPtr, 1, 1);

    return WX_SUCCESS;
}

UINT32 WX_RS422MasterDriver_Construct(VOID *module)
{
    UINT32 ret;
    WxRs422DriverMaster *this = WX_Mem_Alloc(WX_GetModuleName(module), 1, sizeof(WxRs422DriverMaster));
    if (this == NULL) {
        return WX_RS422_MASTER_MEM_ALLOC_FAIL;
    }
    WX_CLEAR_OBJ(this);
    WxRs422MasterDriverCfg *cfg = &g_rs422DriverMasterCfg;
    this->moduleId = WX_GetModuleId(module);
    this->status = WX_RS422_MASTER_STATUS_IDLE;
    this->upperModuleId = cfg->upperModuleId;
    /* create a queue to buff the tx adu */
    this->msgQueHandle = xQueueCreate(cfg->msgQueItemNum, sizeof(WxRs422MasterDriverTxDataReq));
    if (this->msgQueHandle == NULL) {
        boot_debug("Error Exit: xQueueCreate fail");
        return WX_RS422_MASTER_CREATE_MSG_QUE_FAIL;
    }
    /* init the rs422 master driver 's gpio */
    ret = WX_RS422MasterDriver_InitGpio(&this->gpipInst, cfg->gpioDevId);
    if (ret != WX_SUCCESS) {
        boot_debug("Error Exit: WX_RS422MasterDriver_InitGpio fail(%u)", ret);
        return ret;
    }

    /* the inst or rs422 used for uart data tx/rx */
    ret = WX_InitUartNs550(&this->rs422Inst, cfg->rs422DevId, &cfg->rs422Format);
    if (ret != WX_SUCCESS) {
        return ret;
    }

    /* setup the uart interrupt */
    ret = WX_SetupUartNs550Interrupt(&this->rs422Inst,
    	WX_RS422I_DRIVER_MASTER_IntrHandler, cfg->intrId, this);
    if (ret != WX_SUCCESS) {
        return ret;
    }

    WX_SetModuleInfo(module, this);
    return WX_SUCCESS;
}

UINT32 WX_RS422MasterDriver_Entry(VOID *module, WxMsg *evtMsg)
{
    WxRs422DriverMaster *this = WX_GetModuleInfo(module);
    if (evtMsg->msgType != WX_MSG_TYPE_RS422_MASTER_DRIVER_REQ) {
        return WX_RS422_MASTER_DRIVER_UNSPT_MSGTYPE;
    }
    /* 鐎涙劖绉烽幁顖滆閸拷 */
    switch (evtMsg->subMsgType) {
        case WX_SUB_MSG_REQ_RS422_MASTER_DRIVER_TX_ADU: {
            return WX_RS422MasterDriver_ProcTxAduMsg(this, (WxRs422MasterDriverMsg *)evtMsg);
        }
        default: {
            return WX_RS422_MASTER_DRIVER_UNSPT_SUBTYPE;
        }
    }
}

UINT32 WX_RS422MasterDriver_Destruct(VOID *module)
{
    WxRs422DriverMaster *this = WX_GetModuleInfo(module);
    if (this == NULL) {
        return WX_SUCCESS;
    }
    WX_Mem_Free(this);
    WX_SetModuleInfo(module, NULL);
    return WX_SUCCESS;
}
