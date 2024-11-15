#include "wx_include.h"
#include "wx_can.h"
#include "xcanps.h"
#include "xparameters.h"
#include "xil_printf.h"

/* CAN设备初始化 */
UINT32 WX_CAN_InitialDevice(XCanPs *canInstPtr, WxCanDeviceCfgInfo *cfg)
{
	/*
	 * Initialize the Can device.
	 */
	XCanPs_Config *configPtr = XCanPs_LookupConfig(cfg->deviceId);
	if (canInstPtr == NULL) {
		return WX_CAN_LOOKUP_CONFIG_FAIL;
	}
	int status = XCanPs_CfgInitialize(canInstPtr, configPtr, configPtr->BaseAddr);
	if (status != XST_SUCCESS) {
		return WX_CAN_CONFIG_INIT_FAIL;
	}

	/*
	 * Run self-test on the device, which verifies basic sanity of the
	 * device and the driver.
	 */
	status = XCanPs_SelfTest(canInstPtr);
	if (status != XST_SUCCESS) {
		return WX_CAN_CONFIG_SELF_TEST_FAIL;
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