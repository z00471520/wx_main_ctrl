#include "wx_include.h"
#include "wx_uart_ns550.h"
/*
 * 初始化指定的UART NS550 IP core
 */
WxFailCode WX_InitUartNs550(XUartNs550 *this, UINT32 deviceId, XUartNs550Format *format)
{
	/*
	 * Initialize the UART Lite driver so that it's ready to use,
	 * specify the device ID that is generated in xparameters.h
	 */
	int Status = XUartNs550_Initialize(this, deviceId);
	if (Status != XST_SUCCESS) {
		wx_log(WX_CRITICAL, "Error Exit: XUartNs550_Initialize(%u) fail(%d)", deviceId, Status);
		return WX_INIT_UARTNS550_FAIL;
	}

	/*
	 * Perform a self-test to ensure that the hardware was built  correctly
	 */
	Status = XUartNs550_SelfTest(this);
	if (Status != XST_SUCCESS) {
		wx_log(WX_CRITICAL, "Error Exit: XUartNs550_SelfTest(%u) fail(%d)", Status);
		return WX_UARTNS550_SELF_TEST_FAIL;
	}
	XUartNs550_SetOptions(this, (u16)XUN_OPTION_FIFOS_ENABLE);
	/* Set uart mode Baud Rate 115200, 8bits, no parity, 1 stop bit */
	XUartNs550_SetDataFormat(this, format);
	return WX_SUCCESS;
}

/*
 * 使能UARTNS550设备的中断
 * intcInst - 中断控制器
 * uartInstPtr - uart实例指针
 * uartIntrCfg - 中断配置信息
 **/
WxFailCode WX_SetupUartNs550Interrupt(XUartNs550 *uartInstPtr, WxUartNs550IntrCfg *uartIntrCfg)
{
	INTC *intcInst = WX_GetIntrCtrlInst();
	if (intcInst == NULL) {
		return WX_UART_NS550_INTR_CTRL_UNREADY;

	}
	/* 设置优先级 */
	XScuGic_SetPriorityTriggerType(intcInst, uartIntrCfg->intrId, 0xA0, 0x3);
	/*
	 * Connect a device driver handler that will be called when an interrupt
	 * for the device occurs, the device driver handler performs the
	 * specific interrupt processing for the device.
	 */
	int status = XScuGic_Connect(intcInst, intrCfg->intrId,
				 (Xil_ExceptionHandler)XUartNs550_InterruptHandler, uartInstPtr);
	if (status != XST_SUCCESS) {
		wx_log(WX_CRITICAL, "Error Exit: XScuGic_Connect RS422(%u) fail(%u)", status);
		return WX_SCUGIC_CONNECT_RS422_FAIL;
	}

	/*
	 * Enable the interrupt for the Timer device.
	 */
	XScuGic_Enable(intcInst, uartIntrCfg->intrId);
	/*
	 * Setup the handlers for the UART that will be called from the
	 * interrupt context when data has been sent and received, specify a
	 * pointer to the UART driver instance as the callback reference so
	 * the handlers are able to access the instance data.
	 */
	XUartNs550_SetHandler(uartInstPtr, uartIntrCfg->handle, uartIntrCfg->callBackRef);

	return WX_SUCCESS;
}
