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

INTC g_wxIntrCtrlInst = {0};

/* 获取中断控制实例 */
INTC *WX_GetIntrCtrlInst(VOID)
{
	return &g_wxIntrCtrlInst;
} 

/* 初始化中断控制实例 */
UINT32 WX_InitIntrCtrlInst(INTC *intcInst)
{
	/* 创建中断控制器实例 */
	XScuGic_Config *intcConfig = XScuGic_LookupConfig(INTC_DEVICE_ID);
	if (NULL == intcConfig) {
		wx_log(WX_CRITICAL, "Error Exit: XScuGic_LookupConfig(%u) fail(%u)", INTC_DEVICE_ID, rc);
		return WX_SCUGIC_LOOKUP_CFG_FAIL;
	}

	int Status = XScuGic_CfgInitialize(intcInst, intcConfig, intcConfig->CpuBaseAddress);
	if (Status != XST_SUCCESS) {
		return WX_SCUGIC_CFG_INIT_FAIL;
	}
	/*
	 * Initialize the exception table.
	 */
	Xil_ExceptionInit();
	/*
	 * Register the interrupt controller handler with the exception table.
	 */
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT, (Xil_ExceptionHandler)XScuGic_InterruptHandler, intcInst);

	/*
	 * Enable exceptions.
	 */
	// Xil_ExceptionEnable();
	return WX_SUCCESS;
}

int main(void)
{
	/*
	 * inst the interrupt controller inst
	 */
	UINT32 rc = WX_InitIntrCtrlInst(&g_wxIntrCtrlInst);
	if (rc != WX_SUCCESS) {
		wx_log(WX_CRITICAL, "Error Exit: WX_InitIntrCtrlInst fail(%u)", rc);
		return rc;
	}

	/* If all is well, the scheduler will now be running, and the following line
	will never be reached.  If the following line does execute, then there was
	insufficient FreeRTOS heap memory available for the idle and/or timer tasks
	to be created.  See the memory management section on the FreeRTOS web site
	for more details. */
	for( ;; );
}
