#ifndef __WX_RS422MasterDriver_H__
#define __WX_RS422MasterDriver_H__
#include "wx_modbus.h"
typedef enum {
    WX_RS422_MASTER_STATUS_IDLE = 0,
    WX_RS422_MASTER_STATUS_TX_ADU,
    WX_RS422_MASTER_STATUS_RX_ADU,
} WxRs422Status;

typedef struct {
    UINT8 status; /* 鐘舵�� */
    WxModbusAdu txAdu; /* the adu to be send by rs422 */
    WxModbusAdu rxAdu;  /* the adu recieve from rs422 */
    QueueHandle_t msgQueHandle; /* 鐢ㄤ簬缂撳瓨澶栭儴妯″潡鍙戦�佺粰RS422鐨勫緟鍙戦�佺殑娑堟伅闃熷垪 */
    XUartNs550 rs422Inst; /* RS422瀹炰緥 */
} WxRs422DriverMaster;

UINT32 WX_RS422MasterDriver_Construct(VOID *module);
UINT32 WX_RS422MasterDriver_Entry(VOID *module, WxMsg *evtMsg);
UINT32 WX_RS422MasterDriver_Destruct(VOID *module);
#endif //__WX_RS422MasterDriver_H__
