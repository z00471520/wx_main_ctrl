#ifndef __WX_UART_NS550_H__
#define __WX_UART_NS550_H__
#include "wx_include.h"

/* UART的中断配置信息 */
typedef struct {
	UINT32 intrId,			    /* 中断ID */
	XUartNs550_Handler handle; /* 中断服务函数 */ 
} WxUartNs550IntrCfg;

/* RS422配置信息 */
typedef struct {
    UINT32 rs422DevId;  /* 设备ID */
    UINT32 intrId;      /* 中断ID */
    UINT32 msgQueItemNum;	/* 如果需要消息队列可以设置 */
    XUartNs550Format rs422Format; /* 串口配置信息 */
} WxRs422DriverCfg;

/* 根据配置信息初始化UartNs550 IP core */
UINT32 WX_InitUartNs550(XUartNs550 *this, UINT32 deviceId, XUartNs550Format *format);
/* 设置URART的中断 */
UINT32 WX_SetupUartNs550Interrupt(XUartNs550 *uartInstPtr, XUartNs550_Handler handler, UINT32 intrId, VOID *callBackRef);
#endif
