#include "wx_include.h"
#include "wx_can_driver.h"
#include "xcanps.h"
#include "xparameters.h"
#include "xil_printf.h"
#include "wx_msg_intf.h"

/* 设置CAN中断 */
UINT32 WX_CAN_DRIVER_InitialInterrupt(XCanPs *canInstPtr, WxCanDriverIntrCfg *cfg)
{
	/* 中断控制器 */
	INTC *intcInst = WX_GetIntrCtrlInst();
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
	 * Enable the interrupt for the Timer device.
	 */
	XScuGic_Enable(intcInst, cfg->intrId);
	
	/*
	 * Enable all interrupts in CAN device.
	 */
	XCanPs_IntrEnable(canInstPtr, XCANPS_IXR_ALL);
	/*
	 * Set interrupt handlers.
	 */
	XCanPs_SetHandler(canInstPtr, XCANPS_HANDLER_SEND, (void *)SendHandler, (void *)CanInstPtr);
	XCanPs_SetHandler(canInstPtr, XCANPS_HANDLER_RECV, (void *)RecvHandler, (void *)CanInstPtr);
	XCanPs_SetHandler(canInstPtr, XCANPS_HANDLER_ERROR, (void *)ErrorHandler, (void *)CanInstPtr);
	XCanPs_SetHandler(canInstPtr, XCANPS_HANDLER_EVENT, (void *)EventHandler, (void *)CanInstPtr);
	return WX_SUCCESS;
}


/* CAN设备初始化 */
UINT32 WX_CAN_DRIVER_InitialInstance(XCanPs *canInstPtr, WxCanDriverCfg *cfg)
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

	/*
	 * Enter Normal Mode.
	 */
	XCanPs_EnterMode(canInstPtr, XCANPS_MODE_NORMAL);
	while(XCanPs_GetMode(canInstPtr) != XCANPS_MODE_NORMAL);

	return status;
}


/* CAN发送数据帧, 该函数发送失败就会返回对应的原因值 */
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

/* CAN发送数据帧到缓存队列, 该函数发送失败就会返回对应的原因值 */
UINT32 WX_CAN_DRIVER_SendCanFrameToSendQue(QueueHandle_t sendQueue, WxCanFrame *frame)
{
	BaseType_t ret = xQueueSend(sendQueue, (const void * )frame, (TickType_t)0);
	if (ret != pdPASS) {	/* 发送失败 */
		return WX_CAN_DRIVER_CONFIG_TX_QUEUE_SEND_FAIL;
	}
	return WX_SUCCESS;
}

/* CAN发送数据帧, 该函数发送失败就会返回对应的原因值 */
UINT32 WX_CAN_DRIVER_ProcCanFrameMsg(WxCanDriver *this, WxMsg *evtMsg)
{
	WxCanFrame *frame = (WxCanFrame *)evtMsg->msgData;
	/* 发送CAN帧 */
	UINT32 ret = WX_CAN_DRIVER_SendFrameDirectly(&this->canInst, frame);
	if (ret == WX_SUCCESS) {
		return WX_SUCCESS;
	}
	/* 如果发送的CAN缓存满了则发送到缓存队列 */
	if (ret == WX_CAN_DRIVER_CONFIG_TX_FAIL_FULL) {
		return WX_CAN_DRIVER_SendCanFrameToSendQue(this, frame);
	}

	return ret;
}

UINT32 WX_CAN_DRIVER_Constuct(WxCanDriver *this, WxCanDriverCfg *cfg, WxCanDriverIntrCfg *intrCfg)
{
	UINT32 ret;
	/* 初始化实例 */
	ret = WX_CAN_DRIVER_InitialInstance(&this->canInst, cfg);
	if (ret != WX_SUCCESS) {
		return ret;
	}

	/* 初始化发送缓存队列 */
	QueueHandle_t this->sendQueue = xQueueCreate((UBaseType_t)cfg->bufferQueLen, 
		(WXUBaseType_t)sizeof(WxCanFrame));
	if (this->sendQueue == NULL) {
		return WX_CAN_DRIVER_CONFIG_TX_QUEUE_CREATE_FAIL;
	}

	/* 初始化中断 */
	ret = WX_CAN_DRIVER_InitialInterrupt(&this->canInst, intrCfg);
	if (ret != WX_SUCCESS) {
		return ret;
	}

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


/* 消息处理入口 */
UINT32 WX_CAN_DRIVER_Entry(WxCanDriver *this, WxMsg *evtMsg)
{
	switch (evtMsg->msgType) {
		case WX_MSG_TYPE_CAN_FRAME: {
			return WX_CAN_DRIVER_ProcCanFrameMsg(&this->canInst, evtMsg);
		}
		default: {
			return WX_CAN_DRIVER_MSG_TYPE_INVALID;	
		}
	}
}