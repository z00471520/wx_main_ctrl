#include "wx_typedef.h"
#include "wx_rs422_uart_ns550.h"
#include "wx_failcode.h"

WxRs422CfgInfo g_wxRs422Ns550CfgInfoTbl[WX_RS422_UART_NS_550_TYPE_BUTT] = {
	[WX_RS422_UART_NS_550_TYPE_2_EXT_ELE_SEC_PULSE] =  {
		WX_INVALID_BORT_RATE,
		XUN_FORMAT_8_BITS,
		XUN_FORMAT_NO_PARITY,
		XUN_FORMAT_1_STOP_BIT,
		WX_INVALID_DEVICE_ID,
	},
	[WX_RS422_UART_NS_550_TYPE_2_EXT_ELE_TELECTRL] =  {
		115200,
		XUN_FORMAT_8_BITS,
		XUN_FORMAT_NO_PARITY,
		XUN_FORMAT_1_STOP_BIT,
		WX_INVALID_DEVICE_ID,
	},
	[WX_RS422_UART_NS_550_TYPE_2_EXT_SELF_MNG] = {
		961600000,
		XUN_FORMAT_8_BITS,
		XUN_FORMAT_NO_PARITY,
		XUN_FORMAT_1_STOP_BIT,
		WX_INVALID_DEVICE_ID,
	},
	[WX_RS422_UART_NS_550_TYPE_2_EXT_GPS] =      {
		WX_INVALID_BORT_RATE,
		XUN_FORMAT_8_BITS,
		XUN_FORMAT_NO_PARITY,
		XUN_FORMAT_1_STOP_BIT,
		WX_INVALID_DEVICE_ID,
	},
	[WX_RS422_UART_NS_550_TYPE_2_INNER_DEVICE] = {
		WX_INVALID_BORT_RATE,
		XUN_FORMAT_8_BITS,
		XUN_FORMAT_NO_PARITY,
		XUN_FORMAT_1_STOP_BIT,
		WX_INVALID_DEVICE_ID,
	},
};

WxFailCode WX_RS422_InitOneDevice(WxRs422UartNs550 *this, WxRs422CfgInfo *init)
{
	this->isValid = FALSE;
	/* some check */
	if (init->bortRate == WX_INVALID_BORT_RATE) {
		wx_log(WX_CRITICAL, "Error Exit: Invalid device(%u) bortRate(%u)", init->deviceId, init->bortRate);
	}
	
	/* Save the init info to local */
	this->bortRate = init->bortRate;
	this->dataBits = init->dataBits;
	this->deviceId = init->deviceId;
	this->intrId = init->intrId;
	this->parity = init->parity;

	/*
	 * Initialize the UART Lite driver so that it's ready to use,
	 * specify the device ID that is generated in xparameters.h
	 */
	int Status = XUartNs550_Initialize(&this->uartNs5520, init->deviceId);
	if (Status != XST_SUCCESS) {
		wx_log(WX_CRITICAL, "Error Exit: XUartNs550_Initialize(%u) fail(%d)", init->deviceId, Status);
		return WX_INIT_UARTNS550_FAIL;
	}

	/*
	 * Perform a self-test to ensure that the hardware was built  correctly
	 */
	Status = XUartNs550_SelfTest(&this->uartNs5520);
	if (Status != XST_SUCCESS) {
		wx_log(WX_CRITICAL, "Error Exit: XUartNs550_SelfTest(%u) fail(%d)", Status);
		return WX_UARTNS550_SELF_TEST_FAIL;
	}

	XUartNs550_SetOptions(&this->uartNs5520, (u16)XUN_OPTION_FIFOS_ENABLE);
	XUartNs550Format uartDataFormat = {
		.BaudRate = this->bortRate,	/**< In bps, ie 1200 */
		.DataBits = this->dataBits,	/**< Number of data bits */
		.Parity = this->parity,	/**< Parity */
		.StopBits = this->stopBits,	/**< Number of stop bits */
	};
	/* Set uart mode Baud Rate 115200, 8bits, no parity, 1 stop bit */
	XUartNs550_SetDataFormat(&this->uartNs5520, &uartDataFormat);

	this->isValid = TRUE;
	wx_log(WX_DBG_LOG, "The RS422 UART NS550(%u) init success");
	return WX_SUCCESS;
}

WxFailCode WX_RS422_InitDevices(WxRs422Devices *rs422Devices)
{
	WxFailCode rc = WX_SUCCESS;
	WxRs422CfgInfo *initPara = NULL;

	for (UINT32 i = 0; i < WX_RS422_UART_NS_550_TYPE_BUTT; i++) {
		initPara = &g_wxRs422Ns550CfgInfoTbl[i];
		if (initPara->deviceId == WX_INVALID_DEVICE_ID) {
			wx_log(WX_WARN, "Warning: the rs422 device(%u) not init", i);
			continue;
		}
		rc = WX_RS422_InitOneDevice(&rs422Devices->uartNs550[i], initPara);
		if (rc != WX_SUCCESS) {
			wx_log(WX_CRITICAL, "Error Exit: WX_RS422_InitOneDevice(%u) fail(%u)", i, rc);
			return rc;
		}
	}

	return rc;
}

/*
 * RS422消息收发任务
 */

WX_RS422_Inner_MsgTxRxTaks()
{

}

