#include "wx_include.h"
#include "wx_can_driver.h"
#include "xcanps.h"
#include "xparameters.h"
#include "xil_printf.h"

/* 设置CAN中断 */
UINT32 WX_CAN_DRIVER_SetupCanInterrupt(XCanPs *canInstPtr, WxCanIntrCfgInfo *cfg)
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

/* CAN发送数据帧, 该函数发送失败就会返回对应的原因值 */
UINT32 WX_CAN_DRIVER_SendFrame(XCanPs *canInstPtr, WxCanFrame *frame)
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

/* CAN设备初始化 */
UINT32 WX_CAN_DRIVER_InitialDevice(XCanPs *canInstPtr, WxCanDeviceCfgInfo *cfg)
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