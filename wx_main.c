/*
    Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
    Copyright (c) 2012 - 2020 Xilinx, Inc. All Rights Reserved.
	SPDX-License-Identifier: MIT


    http://www.FreeRTOS.org
    http://aws.amazon.com/freertos


    1 tab == 4 spaces!
*/

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
/* Xilinx includes. */
#include "xil_printf.h"
#include "xparameters.h"
#include "wx_main_ctrl_cpu0.h"
#include "wx_typedef.h"
#include "wx_rs422_uart_ns550.h"
#include "wx_failcode.h"
#include "xil_exception.h"
#define INTC_DEVICE_ID		XPAR_SCUGIC_SINGLE_DEVICE_ID
WxMainCtrlCpu0Info g_wxMainCtrlCpu0Info = {0};



/* 获取RS422I的实例指针 */
XUartNs550 *WX_GetRs422IInstPtr(VOID)
{
	return &g_wxMainCtrlCpu0Info.rs422Devices.uartNs550[WX_RS422_UART_NS_550_TYPE_2_INNER_DEVICE].inst;
}
 

/* 创建不同设备的各个实例 */
WxFailCode WX_InitDeviceInsts(WxMainCtrlCpu0Info *this)
{
	WxFailCode rc = WX_RS422_InitDevices(this->rs422Devices);
	if (rc != WX_SUCCESS) {
		wx_log(WX_CRITICAL, "Error Exit: WX_RS422_InitDevices fail(%u)", rc);
		return rc;
	}
	/* if more device please add here */
	return WX_OK;
}

/* 初始化 */
WxFailCode WX_Init(WxMainCtrlCpu0Info *this)
{
	WxFailCode rc;
	rc = WX_InitDeviceInsts(this);
	if (rc != WX_OK) {
		wx_log(WX_CRITICAL, "Error Exit: WX_InitDeviceInst fail(%u)", rc);
		return rc;
	}

	rc = WX_SetupIntrSystem(this);
	if (rc != WX_SUCCESS) {
		wx_log(WX_CRITICAL, "Error Exit: WX_SetupIntrSystem fail(%u)", rc);
		return rc;
	}

	return WX_SUCCESS;
}

int main(void)
{
	WxMainCtrlCpu0Info *this = &g_wxMainCtrlCpu0Info;
	WxFailCode rc = WX_Init();
	if (rc != WX_SUCCESS) {
		xil_printf("Error Exit: WX_Init fail(%u)", rc);
		return rc;
	}

	/* If all is well, the scheduler will now be running, and the following line
	will never be reached.  If the following line does execute, then there was
	insufficient FreeRTOS heap memory available for the idle and/or timer tasks
	to be created.  See the memory management section on the FreeRTOS web site
	for more details. */
	for( ;; );
}
