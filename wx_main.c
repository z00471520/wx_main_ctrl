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
#include "wx_typedef.h"
#include "wx_failcode.h"
#include "xil_exception.h"
#include "wx_frame.h"
int main(void)
{
	taskENTER_CRITICAL();
	boot_debug("\r\n");
	boot_debug("===========================================================");
	boot_debug("======================WX BOOT STATT========================");
	boot_debug("===========================================================");
	boot_debug("WX_SetupFrame start...!");
	UINT32 ret = WX_SetupFrame(WX_GetCurCoreId());
	if (ret != WX_SUCCESS) {
		boot_debug("WX_SetupFrame failed(%u)", ret);
		taskEXIT_CRITICAL();
	} else {
		boot_debug("Enable exception!");
		/* enable excption */
		Xil_ExceptionEnable();
		boot_debug("===========================================================");
		boot_debug("======================WX BOOT SUCCESS======================");
		boot_debug("===========================================================");
		taskEXIT_CRITICAL();
		/* Start the scheduler. */
		vTaskStartScheduler();
			
	}
	
	/* If all is well, the scheduler will now be running, and the following line
	will never be reached.  If the following line does execute, then there was
	insufficient FreeRTOS heap memory available for the idle and/or timer tasks
	to be created.  See the memory management section on the FreeRTOS web site
	for more details. */
	for( ;; );
}
