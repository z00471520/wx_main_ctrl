#include "wx_uart_ns550_intr.h"
#include "wx_rs422_uart_ns550.h"
#include "wx_rs422_i_task.h"
#include "xil_exception.h"
/* 中断处理函数 */
void WX_RS422_IntrHandler(void *CallBackRef, u32 Event, unsigned int EventData)
{
	
}

/* 中断注册信息 */
WxUartNs550IntrCfgInfo g_wxUartNs550IntrCfgInfos[WX_RS422_UART_NS_550_TYPE_BUTT] = {
	[WX_RS422_UART_NS_550_TYPE_2_EXT_ELE_SEC_PULSE] =  {
        WX_DISABLE_INTR, /* 中断开关，如果开启则必须提供下述参数 */
		WX_INVALID_INTR_ID, /* 中断ID */
		WX_RS422_IntrHandler, /* 中断处理函数 */
        NULL, /* 中断函数的数据引用获取钩子 */ 
	},
	[WX_RS422_UART_NS_550_TYPE_2_EXT_ELE_TELECTRL] =  {
        WX_DISABLE_INTR, /* 中断开关，如果开启则必须提供下述参数 */
		WX_INVALID_INTR_ID, /* 中断ID */
		WX_RS422_IntrHandler, /* 中断处理函数 */
	},
	[WX_RS422_UART_NS_550_TYPE_2_EXT_SELF_MNG] = {
        WX_DISABLE_INTR, /* 中断开关，如果开启则必须提供下述参数 */
		WX_INVALID_INTR_ID, /* 中断ID */
		WX_RS422_IntrHandler, /* 中断处理函数 */
        NULL, /* 中断函数的数据引用获取钩子 */ 
	},
	[WX_RS422_UART_NS_550_TYPE_2_EXT_GPS] = {
        WX_DISABLE_INTR, /* 中断开关，如果开启则必须提供下述参数 */
		WX_INVALID_INTR_ID, /* 中断ID */
		WX_RS422_IntrHandler, /* 中断处理函数 */
        NULL,
	},
	[WX_RS422_UART_NS_550_TYPE_2_INNER_DEVICE] = {
        WX_DISABLE_INTR, /* 中断开关，如果开启则必须提供下述参数 */
		WX_INVALID_INTR_ID, /* 中断ID */
		WX_RS422I_IntrHandler, /* RS422I的中断处理函数 */
        WX_GetRs422IIntrDataRef, /* 中断函数的数据引用获取钩子 */ 
	},
};

/*
 * 使能UARTNS550设备的中断
 **/
WxFailCode WX_UARTNS550_SetupDeviceIntr(INTC *intcInst, WxRs422UartNs550 *device, WxUartNs550IntrCfgInfo *intrCfg)
{
	/* 设置优先级 */
	XScuGic_SetPriorityTriggerType(intcInst, intrCfg->intrId, 0xA0, 0x3);
	/*
	 * Connect a device driver handler that will be called when an interrupt
	 * for the device occurs, the device driver handler performs the
	 * specific interrupt processing for the device.
	 */
	int status = XScuGic_Connect(intcInst, intrCfg->intrId,
				 (Xil_ExceptionHandler)XUartNs550_InterruptHandler, &device->inst);
	if (status != XST_SUCCESS) {
		wx_log(WX_CRITICAL, "Error Exit: XScuGic_Connect RS422(%u) fail(%u)", Status);
		return WX_SCUGIC_CONNECT_RS422_FAIL;
	}

	/*
	 * Enable the interrupt for the Timer device.
	 */
	XScuGic_Enable(intcInst, intrCfg->intrId);
	/*
	 * Setup the handlers for the UART that will be called from the
	 * interrupt context when data has been sent and received, specify a
	 * pointer to the UART driver instance as the callback reference so
	 * the handlers are able to access the instance data.
	 */
	XUartNs550_SetHandler(&this->inst, intrCfg->intrHandler, device);

	return WX_SUCCESS;
}

/* Set the interrupt of all device
 * intcInst - 中断控制器
  */
WxFailCode WX_UARTNS550_SetupDeviceListIntr(INTC *intcInst, WxRs422Devices *deviceList)
{
	WxFailCode rc = WX_SUCCESS;
	WxUartNs550IntrCfgInfo curIntrCfg = NULL; 
	WxRs422UartNs550 *curDevice = NULL;
	for (UINT32 i = 0; i < WX_RS422_UART_NS_550_TYPE_BUTT; i++) {
		
		curIntrCfg = g_wxUartNs550IntrCfgInfos[i];
        /* 如果中断不使能则不处理 */
        if (!curIntrCfg->isEnable) {
            continue;
        }
        curDevice = &deviceList->uartNs550[i];
		/* 使能的了中断的设备必选实例化OK */
		if (!curDevice->isValid){
            return WX_UART_NS550_INTR_DEVICE_UNREADY;
        }
        if (curIntrCfg->intrHandler == NULL) {
			return WX_UART_NS550_INTR_HANDLE_NULL;
		}

        if (curIntrCfg->intrDatRef == NULL) {
            return WX_UART_NS550_INTR_DATAREF_NULL;
        }
        
		rc = WX_UARTNS550_SetupDeviceIntr(intcInst, curDevice, curIntrCfg);
		if (rc != WX_SUCCESS) {
			wx_log(WX_CRITICAL, "Error Exit: WX_RS422_SetupIntr(%u) fail(%u)", i, rc);
			return rc;
		}
	}

	return WX_SUCCESS;
}


WxFailCode WX_MATCHCTRL_InitIntCtrlInst(INTC *intcInst)
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