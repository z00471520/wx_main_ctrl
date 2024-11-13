#include "wx_typedef.h"
#include "wx_debug.h"
#include "wx_failcode.h"
#include "wx_uart_ns550_intr.h"
WxFailCode WX_SetupIntrSystem(WxMainCtrlCpu0Info *this)
{
	/*
	 * inst the interrupt controller inst
	 */
	WxFailCode rc = WX_MATCHCTRL_InitIntCtrlInst(&this->intCtrlInst);
	if (rc != WX_SUCCESS) {
		wx_log(WX_CRITICAL, "Error Exit: WX_MATCHCTRL_InitIntCtrlInst fail(%u)", rc);
		return rc;
	}
	

	/*
	 * reg the device interrupt func
	 */
	rc = WX_UARTNS550_SetupDeviceListIntr(&this->intCtrlInst, &this->rs422Devices);
	if (rc != WX_SUCCESS) {
		wx_log(WX_CRITICAL, "Error Exit: WX_MATCHCTRL_InitIntCtrlInst fail(%u)", rc);
		return rc;
	}

	/*
	 * if more please add here
	 */
}