
#include "wx_include.h"
#include "wx_can_driver.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "wx_frame.h"
#include "xparameters.h"
#include "xcanps.h"
#include "xil_exception.h"
#include "wx_msg_can_frame_intf.h"
WxCanDriverCfg g_wxCanDriverCfg[] = {
    {
		.moduleId = WX_MODULE_DRIVER_CAN_A,
		.upperModuleId = WX_MODULE_CAN_SLAVE_A,
		.txQueueDepth = 1024,
		.messageId = 0x123,
		.devCfg.deviceId = XPAR_XCANPS_0_DEVICE_ID,
		.devCfg.baudPrescalar = 29, 
		.devCfg.syncJumpWidth = 3,
		.devCfg.timeSegment2 = 2,
		.devCfg.timeSegment1 = 15,
    	.intrCfg.intrId = XPAR_XCANPS_0_INTR,
		.intrCfg.sendHandle = WX_CAN_DRIVER_IntrSendHandler,
		.intrCfg.recvHandle = WX_CAN_DRIVER_IntrRecvHandler,
		.intrCfg.errHandle = WX_CAN_DRIVER_IntrErrorHandler,
		.intrCfg.eventHandle = WX_CAN_DRIVER_IntrEventHandler,
	},
    {
		.moduleId = WX_MODULE_DRIVER_CAN_B,
		.upperModuleId = WX_MODULE_CAN_SLAVE_B,
		.txQueueDepth = 1024,
		.messageId = 0x124,
		.devCfg.deviceId = XPAR_XCANPS_1_DEVICE_ID,
		.devCfg.baudPrescalar = 29,
		.devCfg.syncJumpWidth = 3,
		.devCfg.timeSegment2 = 2,
		.devCfg.timeSegment1 = 15,
    	.intrCfg.intrId = XPAR_XCANPS_1_INTR,
		.intrCfg.sendHandle = WX_CAN_DRIVER_IntrSendHandler,
		.intrCfg.recvHandle = WX_CAN_DRIVER_IntrRecvHandler,
		.intrCfg.errHandle = WX_CAN_DRIVER_IntrErrorHandler,
		.intrCfg.eventHandle = WX_CAN_DRIVER_IntrEventHandler,
	},
};

/* Get can driver configuration by module id */
WxCanDriverCfg *WX_CAN_DRIVER_GetCfg(UINT32 moduleId)
{
    for (int i = 0; i < sizeof(g_wxCanDriverCfg) / sizeof(WxCanDriverCfg); i++) {
        if (g_wxCanDriverCfg[i].moduleId == moduleId) {
            return &g_wxCanDriverCfg[i];
        }
    }
    return NULL;
}

UINT32 WX_CAN_DRIVER_SendFrameDirectly(XCanPs *canInstPtr, WxCanFrame *frame)
{
	if (canInstPtr == NULL) {
		return WX_CAN_DRIVER_CONFIG_INVALID_CAN_INST_PRT;
	}
	/*
	* Buffers to hold frames to send and receive. These are declared as global so
	* that they are not on the stack.
	* These buffers need to be 32-bit aligned
	*/
	UINT32 txFrame[WX_CAN_DRIVER_FRAME_U32_LEN] = {0};
	/*
	 * Create correct values for Identifier and Data Length Code Register.
	 */
	txFrame[0] = (u32)XCanPs_CreateIdValue(frame->standardMessID, frame->subRemoteTransReq,
		frame->idExtension, frame->extendedId, frame->remoteTransReq);
	txFrame[1] = (u32)XCanPs_CreateDlcValue(frame->dataLengCode);

	/*
	 * Now fill in the data field with known values so we can verify them
	 * on receive.
	 */
	UINT8 *dataPtr = (UINT8 *)(&txFrame[2]);
	for (UINT32 i = 0; i < frame->dataLengCode; i++) {
		*dataPtr++ = (u8)frame->data[i];
	}

	/*
	 * if TX FIFO is full send will fail.
	 */
	if (XCanPs_IsTxFifoFull(canInstPtr) == TRUE) {
		return WX_CAN_DRIVER_CONFIG_TX_BUFF_FULL;
	}

	/*
	 * Now send the frame.
	 *
	 * Another way to send a frame is keep calling XCanPs_Send() until it
	 * returns XST_SUCCESS. No check on if TX FIFO is full is needed anymore
	 * in that case.
	 */
	UINT32 status = XCanPs_Send(canInstPtr, txFrame);
	if (status != XST_SUCCESS) {
		return WX_CAN_DRIVER_CONFIG_TX_FAIL_FULL;
	}

	return WX_SUCCESS;
}

VOID WX_CAN_DRIVER_SendRxFrame2UpperLayer(WxCanDriver *this, UINT32 *rxFrame)
{
	WxCanFrame frame;
	frame.standardMessID = (rxFrame[0] & XCANPS_IDR_ID1_MASK) >> XCANPS_IDR_ID1_SHIFT;
	frame.subRemoteTransReq = (rxFrame[0] & XCANPS_IDR_SRR_MASK) >> XCANPS_IDR_SRR_SHIFT;
	frame.idExtension = (rxFrame[0] & XCANPS_IDR_IDE_MASK) >> XCANPS_IDR_IDE_SHIFT;
	frame.extendedId = (rxFrame[0] & XCANPS_IDR_ID2_MASK) >> XCANPS_IDR_ID2_SHIFT;
	frame.remoteTransReq = (rxFrame[0] & XCANPS_IDR_RTR_MASK);
	frame.dataLengCode = (rxFrame[1] >> XCANPS_DLCR_DLC_SHIFT) & XCANPS_DLCR_DLC_MASK;
	UINT8 *dataPtr = (UINT8 *)(&rxFrame[2]);
	for (UINT32 i = 0; i < frame.dataLengCode; i++) {
		frame.data[i] = dataPtr[i];
	}
	/* 鍒涘缓娑堟伅 */
	WxCanFrameMsg *msg = WX_ApplyEvtMsgFromISR(WX_MSG_TYPE_CAN_FRAME);
	if (msg == NULL) {
		wx_excp_cnt(WX_EXCP_CAN_SLAVE_MSG_APPLY_FAIL);
		return;
	}
	msg->sender = this->moduleId;
	msg->receiver = this->upperModuleId;
	msg->canFrame = frame;
	UINT32 ret = WX_MsgShedule(msg->sender, msg->receiver, (WxMsg*)msg);
	if (ret != WX_SUCCESS) {
		wx_excp_cnt(WX_EXCP_CAN_SLAVE_MSG_SCHEDULE_FAIL);
	}

	return;
}
/*****************************************************************************/
/**
*
* Callback function (called from interrupt handler) to handle confirmation of
* transmit events when in interrupt mode.
*
* @param	callBackRef is the callback reference passed from the interrupt
*		handler, which in our case is a pointer to the driver instance.
*
* @return	None.
*
* @note		This function is called by the driver within interrupt context.
*
******************************************************************************/
VOID WX_CAN_DRIVER_IntrSendHandler(VOID *callBackRef)
{
	WxCanDriver *this = (WxCanDriver *)callBackRef;
	WxCanFrame frame;
	/*
	 * The frame was sent successfully. Notify the task context.
	 */
	if (xQueueReceiveFromISR(this->sendQueue, (VOID *)&frame, NULL) == pdTRUE) {
		UINT32 ret = WX_CAN_DRIVER_SendFrameDirectly(&this->canInst, &frame);
		if (ret != WX_SUCCESS) {
			wx_excp_cnt(WX_EXCP_CAN_SLAVE_TX_FAIL);
			wx_fail_code_cnt(ret);
		}
	}
}


/*****************************************************************************/
/**
*
* Callback function (called from interrupt handler) to handle frames received in
* interrupt mode.  This function is called once per frame received.
* The driver's receive function is called to read the frame from RX FIFO.
*
* @param	callBackRef is the callback reference passed from the interrupt
*		handler, which in our case is a pointer to the device instance.
*
* @return	None.
*
* @note		This function is called by the driver within interrupt context.
*
******************************************************************************/
VOID WX_CAN_DRIVER_IntrRecvHandler(VOID *callBackRef)
{
	
	WxCanDriver *this = (WxCanDriver *)callBackRef;
	XCanPs *canPtr = (XCanPs *)&this->canInst;

	int Status = XCanPs_Recv(canPtr, this->rxFrame);
	if (Status != XST_SUCCESS) {
		wx_excp_cnt(WX_EXCP_CAN_SLAVE_RECV_ERR);
		return;
	}

	WX_CAN_DRIVER_SendRxFrame2UpperLayer(this, this->rxFrame);
}


/*****************************************************************************/
/**
*
* Callback function (called from interrupt handler) to handle error interrupt.
* Error code read from Error Status register is passed into this function.
*
* @param	callBackRef is the callback reference passed from the interrupt
*		handler, which in our case is a pointer to the driver instance.
* @param	errorMask is a bit mask indicating the cause of the error.
*		Its value equals 'OR'ing one or more XCANPS_ESR_* defined in
*		xcanps_hw.h.
*
* @return	None.
*
* @note		This function is called by the driver within interrupt context.
*
******************************************************************************/
VOID WX_CAN_DRIVER_IntrErrorHandler(VOID *callBackRef, u32 errorMask)
{

	if (errorMask & XCANPS_ESR_ACKER_MASK) {
		/*
		 * ACK Error handling code should be put here.
		 */
		wx_excp_cnt(WX_EXCP_CAN_SLAVE_ACK_ERR);
	}

	if (errorMask & XCANPS_ESR_BERR_MASK) {
		/*
		 * Bit Error handling code should be put here.
		 */
		wx_excp_cnt(WX_EXCP_CAN_SLAVE_BIT_ERR);
	}

	if (errorMask & XCANPS_ESR_STER_MASK) {
		/*
		 * Stuff Error handling code should be put here.
		 */
		wx_excp_cnt(WX_EXCP_CAN_SLAVE_STUFF_ERR);
	}

	if (errorMask & XCANPS_ESR_FMER_MASK) {
		/*
		 * Form Error handling code should be put here.
		 */
		wx_excp_cnt(WX_EXCP_CAN_SLAVE_FORM_ERR);
	}

	if (errorMask & XCANPS_ESR_CRCER_MASK) {
		/*
		 * CRC Error handling code should be put here.
		 */
		wx_excp_cnt(WX_EXCP_CAN_SLAVE_CRC_ERR);
	}
}

/*****************************************************************************/
/**
*
* Callback function (called from interrupt handler) to handle the following
* interrupts:
*   - XCANPS_IXR_BSOFF_MASK:	Bus Off Interrupt
*   - XCANPS_IXR_RXOFLW_MASK:	RX FIFO Overflow Interrupt
*   - XCANPS_IXR_RXUFLW_MASK:	RX FIFO Underflow Interrupt
*   - XCANPS_IXR_TXBFLL_MASK:	TX High Priority Buffer Full Interrupt
*   - XCANPS_IXR_TXFLL_MASK:	TX FIFO Full Interrupt
*   - XCANPS_IXR_WKUP_MASK:	Wake up Interrupt
*   - XCANPS_IXR_SLP_MASK:	Sleep Interrupt
*   - XCANPS_IXR_ARBLST_MASK:	Arbitration Lost Interrupt
*
*
* @param	callBackRef is the callback reference passed from the
*		interrupt Handler, which in our case is a pointer to the
*		driver instance.
* @param	intrMask is a bit mask indicating pending interrupts.
*		Its value equals 'OR'ing one or more of the XCANPS_IXR_*_MASK
*		value(s) mentioned above.
*
* @return	None.
*
* @note		This function is called by the driver within interrupt context.
* 		This function should be changed to meet specific application
*		needs.
*
******************************************************************************/
VOID WX_CAN_DRIVER_IntrEventHandler(VOID *callBackRef, u32 intrMask)
{
	if (intrMask & XCANPS_IXR_BSOFF_MASK) {
		/*
		 * Entering Bus off status interrupt requires
		 * the CAN device be reset and reconfigured.
		 */
		// XCanPs_Reset(canPtr);
		wx_excp_cnt(WX_EXCP_CAN_SLAVE_BUS_OFF);
		return;
	}

	if (intrMask & XCANPS_IXR_RXOFLW_MASK) {
		/*
		 * Code to handle RX FIFO Overflow Interrupt should be put here.
		 */
		wx_excp_cnt(WX_EXCP_CAN_SLAVE_RX_FIFO_OVERFLOW);
	}

	if (intrMask & XCANPS_IXR_RXUFLW_MASK) {
		/*
		 * Code to handle RX FIFO Underflow Interrupt
		 * should be put here.
		 */
		wx_excp_cnt(WX_EXCP_CAN_SLAVE_RX_FIFO_UNDERFLOW);
	}

	if (intrMask & XCANPS_IXR_TXBFLL_MASK) {
		/*
		 * Code to handle TX High Priority Buffer Full
		 * Interrupt should be put here.
		 */
		wx_excp_cnt(WX_EXCP_CAN_SLAVE_TX_HP_BUF_FULL);
	}

	if (intrMask & XCANPS_IXR_TXFLL_MASK) {
		/*
		 * Code to handle TX FIFO Full Interrupt should be put here.
		 */
		wx_excp_cnt(WX_EXCP_CAN_SLAVE_TX_FIFO_FULL);
	}

	if (intrMask & XCANPS_IXR_WKUP_MASK) {
		/*
		 * Code to handle Wake up from sleep mode Interrupt
		 * should be put here.
		 */
		wx_excp_cnt(WX_EXCP_CAN_SLAVE_WAKE_UP);
	}

	if (intrMask & XCANPS_IXR_SLP_MASK) {
		/*
		 * Code to handle Enter sleep mode Interrupt should be put here.
		 */
		wx_excp_cnt(WX_EXCP_CAN_SLAVE_SLEEP);
	}

	if (intrMask & XCANPS_IXR_ARBLST_MASK) {
		/*
		 * Code to handle Lost bus arbitration Interrupt
		 * should be put here.
		 */
		wx_excp_cnt(WX_EXCP_CAN_SLAVE_LOST_ARB);
	}
}

UINT32 WX_CAN_DRIVER_InitInterrupt(WxCanDriver *this, WxCanDriverIntrCfg *cfg)
{
	XCanPs *canInstPtr = &this->canInst;
	XScuGic *intcInst = WX_GetScuGicInstance();
	if (intcInst == NULL) {
		return WX_UART_NS550_INTR_CTRL_UNREADY;
	}

	/*
	 * Connect the device driver handler that will be called when an
	 * interrupt for the device occurs, the handler defined above performs
	 * the specific interrupt processing for the device.
	 */
	int status = XScuGic_Connect(intcInst, cfg->intrId,
				 (Xil_ExceptionHandler)XCanPs_IntrHandler, canInstPtr);
	if (status != XST_SUCCESS) {
		wx_log(WX_CRITICAL, "Error Exit: XScuGic_Connect RS422(%u) fail(%u)", status);
		return WX_SCUGIC_CONNECT_RS422_FAIL;
	}


	/*
	 * Set interrupt handlers.
	 */
	XCanPs_SetHandler(canInstPtr, XCANPS_HANDLER_SEND, (VOID *)cfg->sendHandle, (VOID *)this);
	XCanPs_SetHandler(canInstPtr, XCANPS_HANDLER_RECV, (VOID *)cfg->recvHandle, (VOID *)this);
	XCanPs_SetHandler(canInstPtr, XCANPS_HANDLER_ERROR, (VOID *)cfg->errHandle, (VOID *)this);
	XCanPs_SetHandler(canInstPtr, XCANPS_HANDLER_EVENT, (VOID *)cfg->eventHandle, (VOID *)this);

	/*
	 * Enable the interrupt for the Timer device.
	 */
	XScuGic_Enable(intcInst, cfg->intrId);
	
	/*
	 * Enable all interrupts in CAN device.
	 */
	XCanPs_IntrEnable(canInstPtr, XCANPS_IXR_ALL);

	return WX_SUCCESS;
}


/* Initialize the Can device. */
UINT32 WX_CAN_DRIVER_InitDevice(XCanPs *canInstPtr, WxCanDriverDevCfg *cfg)
{
	if (canInstPtr == NULL) {
		return WX_CAN_DRIVER_CONFIG_INVALID_CAN_INST_PRT;
	}
	/*
	 * Initialize the Can device.
	 */
	XCanPs_Config *configPtr = XCanPs_LookupConfig(cfg->deviceId);
	if (canInstPtr == NULL) {
		return WX_CAN_DRIVER_LOOKUP_CONFIG_FAIL;
	}
	int status = XCanPs_CfgInitialize(canInstPtr, configPtr, configPtr->BaseAddr);
	if (status != XST_SUCCESS) {
		return WX_CAN_DRIVER_CONFIG_INIT_FAIL;
	}

	/*
	 * Run self-test on the device, which verifies basic sanity of the
	 * device and the driver.
	 */
	status = XCanPs_SelfTest(canInstPtr);
	if (status != XST_SUCCESS) {
		return WX_CAN_DRIVER_CONFIG_SELF_TEST_FAIL;
	}

	/*
	 * Enter Configuration Mode so we can setup Baud Rate Prescaler
	 * Register (BRPR) and Bit Timing Register (BTR).
	 */
	XCanPs_EnterMode(canInstPtr, XCANPS_MODE_CONFIG);
	while(XCanPs_GetMode(canInstPtr) != XCANPS_MODE_CONFIG);

	/*
	 * Setup Baud Rate Prescaler Register (BRPR) and
	 * Bit Timing Register (BTR).
	 */
	XCanPs_SetBaudRatePrescaler(canInstPtr, cfg->baudPrescalar);
	XCanPs_SetBitTiming(canInstPtr, cfg->syncJumpWidth, cfg->timeSegment2, cfg->timeSegment1);

	return status;
}


UINT32 WX_CAN_DRIVER_SendCanFrameToSendQue(QueueHandle_t sendQueue, WxCanFrame *frame)
{
	BaseType_t ret = xQueueSend(sendQueue, (const VOID * )frame, (TickType_t)0);
	if (ret != pdPASS) {
		return WX_CAN_DRIVER_CONFIG_TX_QUEUE_SEND_FAIL;
	}
	return WX_SUCCESS;
}

UINT32 WX_CAN_DRIVER_ProcCanFrameMsg(WxCanDriver *this, WxMsg *evtMsg)
{
	WxCanFrame *frame = (WxCanFrame *)evtMsg->msgData;
	UINT32 ret = WX_CAN_DRIVER_SendFrameDirectly(&this->canInst, frame);
	if (ret == WX_SUCCESS) {
		return WX_SUCCESS;
	}
	if (ret == WX_CAN_DRIVER_CONFIG_TX_FAIL_FULL) {
		return WX_CAN_DRIVER_SendCanFrameToSendQue(this->sendQueue, frame);
	}

	return ret;
}

UINT32 WX_CAN_DRIVER_Construct(VOID *module)
{
    UINT32 ret;
    WxCanDriver *this = WX_Mem_Alloc(WX_GetModuleName(module), 1, sizeof(WxCanDriver));
    if (this == NULL) {
        return WX_CAN_DRIVER_MEM_ALLOC_FAIL;
    }
    WX_CLEAR_OBJ(this);
    this->moduleId = WX_GetModuleId(module);
    WxCanDriverCfg *cfg = WX_CAN_DRIVER_GetCfg(this->moduleId);
	if (cfg == NULL) {
		return WX_CAN_DRIVER_CONFIG_NOT_FOUND;
	}
	this->upperModuleId = cfg->upperModuleId;
    this->cfg = cfg;
	ret = WX_CAN_DRIVER_InitDevice(&this->canInst, &cfg->devCfg);
	if (ret != WX_SUCCESS) {
		return ret;
	}

	this->sendQueue = xQueueCreate(cfg->txQueueDepth, sizeof(WxCanFrame));
	if (this->sendQueue == NULL) {
		return WX_CAN_DRIVER_CONFIG_TX_QUEUE_CREATE_FAIL;
	}
	ret = WX_CAN_DRIVER_InitInterrupt(this, &cfg->intrCfg);
	if (ret != WX_SUCCESS) {
		return ret;
	}
	/*
	 * Enter Normal Mode.
	 */
	XCanPs_EnterMode(&this->canInst, XCANPS_MODE_NORMAL);
	while(XCanPs_GetMode(&this->canInst) != XCANPS_MODE_NORMAL);
    WX_SetModuleInfo(module, this);
    return WX_SUCCESS;
}

UINT32 WX_CAN_DRIVER_Destruct(VOID *module)
{
    WxCanDriver *this = WX_GetModuleInfo(module);
    if (this == NULL) {
        return WX_SUCCESS;
    }
    WX_Mem_Free(this);
    WX_SetModuleInfo(module, NULL);
    return WX_SUCCESS;
}

UINT32 WX_CAN_DRIVER_Entry(VOID *module, WxMsg *evtMsg)
{
    WxCanDriver *this = WX_GetModuleInfo(module);
    switch (evtMsg->msgType) {
		case WX_MSG_TYPE_CAN_FRAME: {
			return WX_CAN_DRIVER_ProcCanFrameMsg(this, evtMsg);
		}
		default: {
			return WX_CAN_DRIVER_MSG_TYPE_INVALID;	
		}
	}
}
