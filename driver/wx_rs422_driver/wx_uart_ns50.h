#ifndef __WX_UART_NS550_H__
#define __WX_UART_NS550_H__
#include "wx_include.h"

UINT32 WX_InitUartNs550(XUartNs550 *this, UINT32 deviceId, XUartNs550Format *format);
UINT32 WX_SetupUartNs550Interrupt(XUartNs550 *uartInstPtr, XUartNs550_Handler handler, UINT32 intrId, VOID *callBackRef);
#endif
