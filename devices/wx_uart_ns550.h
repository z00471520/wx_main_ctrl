#ifndef WX_UART_NS550_H
#define WX_UART_NS550_H
#include "wx_include.h"

/* UART的中断配置信息 */
typedef struct {
	UINT32 intrId,			    /* 中断ID */
	XUartNs550_Handler handle; /* 中断服务函数 */ 
	VOID *callBackRef;	           /* 中断服务函数的dataRef */
} WxUartNs550IntrCfg;

/* 根据配置信息初始化UartNs550 IP core */
WxFailCode WX_InitUartNs550(XUartNs550 *this, UINT32 deviceId, XUartNs550Format *format);
/* 设置URART的中断 */
WxFailCode WX_SetupUartNs550Interrupt(XUartNs550 *uartInstPtr, WxUartNs550IntrCfg *uartIntrCfg);
#endif
