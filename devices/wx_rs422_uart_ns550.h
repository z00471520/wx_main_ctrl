#ifndef WX_COMM_RS422_H
#define WX_COMM_RS422_H
#include "wx_typedef.h"
#define WX_INVALID_BORT_RATE 0
/* UART 对应的实际接口类型 */
typedef enum {
	WX_RS422_UART_NS_550_TYPE_2_EXT_ELE_SEC_PULSE,  /* to extern Integrated Electronics for sec pulse  */
	WX_RS422_UART_NS_550_TYPE_2_EXT_ELE_TELECTRL,  /* to extern integrated electronics for telecontrl */
	WX_RS422_UART_NS_550_TYPE_2_EXT_SELF_MNG, /* to self manager component for reconstruct */
	WX_RS422_UART_NS_550_TYPE_2_EXT_GPS,      /* to gps for sec pulse */
	WX_RS422_UART_NS_550_TYPE_2_INNER_DEVICE, /* to inner device based modbus protocal */
	WX_RS422_UART_NS_550_TYPE_BUTT,
} WxRs422UartNs550Type;

typedef struct {
    BOOL isValid;    /* 实例是否使能 */
	UINT32 bortRate; /* In bps, ie 1200 */
	UINT32 dataBits; /* Number of data bits eg: XUN_FORMAT_8_BITS*/
	UINT32 parity;   /* 奇偶校验位设置，eg: XUN_FORMAT_NO_PARITY */
	UINT8  stopBits; /* Number of stop bits, eg: XUN_FORMAT_1_STOP_BIT */
	UINT32 deviceId; /* 设备ID，WX_INVALID_DEVICE_ID is a invalid value mean the uart not exist eg: XPAR_AXI_UART16550_0_DEVICE_ID */
    XUartNs550 inst; /* 实例 */
} WxRs422UartNs550;

/* the ip core uart ns550 reg info */
typedef struct {
	UINT32 bortRate; /**< In bps, ie 1200 */
	UINT32 dataBits; /**< Number of data bits eg: XUN_FORMAT_8_BITS*/
	UINT32 parity;  /* eg: XUN_FORMAT_NO_PARITY */
	UINT8  stopBits; /**< Number of stop bits, eg: XUN_FORMAT_1_STOP_BIT */
	UINT32 deviceId; // WX_INVALID_DEVICE_ID is a invalid value mean the uart not exist eg: XPAR_AXI_UART16550_0_DEVICE_ID
} WxRs422CfgInfo;

typedef struct {
	WxRs422UartNs550 uartNs550[WX_RS422_UART_NS_550_TYPE_BUTT]; // the inst of uart of ip core
} WxRs422Devices;

WxFailCode WX_RS422_InitUartNs555(WxRs422UartNs550 *this, WxRs422CfgInfo *init);

#endif
