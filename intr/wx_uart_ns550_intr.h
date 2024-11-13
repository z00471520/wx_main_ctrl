#ifndef WX_UART_NS550_INTR_H
#define WX_UART_NS550_INTR_H
#include "wx_failcode.h"
#include "wx_uart_ns550_intr.h"
#define WX_ENABLE_INTR      (TRUE)      /** 使能中断 */
#define WX_DISABLE_INTR     (FALSE)     /** 不使能中断 */
/* 原型函数：用于获取 */
typedef VOID *(WxGetIntrHandleDatRef)(VOID);

/* interrupt cfg info */
typedef struct {
    BOOL isEnable; /* 中断是能开关；取值说明：WX_ENABLE_INTR - 使能中断；WX_DISABLE_INTR - 不使能中断，如果中断使能下述参数必须提供 */
	UINT32 intrId; /* the interrupt id, */ 
	XUartNs550_Handler intrHandler;   /* 中断处理函数 */
    WxGetIntrHandleDatRef intrDatRef; /* 中断函数的数据引用 */
} WxUartNs550IntrCfgInfo;

VOID *WX_GetRs422IIntrDataRef(VOID);
WxFailCode WX_UARTNS550_SetupDeviceListIntr(INTC *intcInst, WxRs422Devices *deviceList);
#endif