#ifndef __WX_PS_UART_TX_DATA_REQ_H__
#define __WX_PS_UART_TX_DATA_REQ_H__
#include "wx_msg_common.h"
#define WX_UART_PRINT_MAX_CHAR_LEN 256
typedef struct {
    CHAR chs[WX_UART_PRINT_MAX_CHAR_LEN];
    UINT32 chNum;
} WxUartPrintStr;

typedef struct _wx_ps_uart_tx_data_req
{
    WX_INHERIT_MSG_HEADER
    union {
        WxUartPrintStr printStr;  // print string
    };
    
} WxPsUartTxDataReq;


#endif //__WX_PS_UART_TX_DATA_REQ_H__
