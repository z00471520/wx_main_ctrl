#ifndef __WX_RS422MasterDriver_H__
#define __WX_RS422MasterDriver_H__
#include "wx_modbus.h"
#include "wx_include.h "
typedef enum {
    WX_RS422_MASTER_STATUS_IDLE = 0,
    WX_RS422_MASTER_STATUS_TX_ADU,
    WX_RS422_MASTER_STATUS_RX_ADU,
} WxRs422Status;

typedef struct {
    UINT32 rs422DevId;
    UINT32 intrId;
    UINT32 gpioDevId;
    UINT32 resv;
    UINT32 msgQueItemNum;
    XUartNs550Format rs422Format;
} WxRs422MasterDriverCfg;

typedef struct {
    UINT8 status; /*  master's txrx status see WxRs422Status for detail  */
    UINT8 moduleId; /* module id  */
    UINT8 resv;
    UINT8 resv1;
    WxModbusAdu txAdu; /* the adu to be send by rs422 */
    WxModbusAdu rxAdu;  /* the adu recieve from rs422 */
    QueueHandle_t msgQueHandle; /* adu to be sent */
    XUartNs550 rs422Inst; /* RS422 inst  */
    XGpio gpipInst; /* GPIO inst to control rs422's direction */
} WxRs422DriverMaster;

UINT32 WX_RS422MasterDriver_Construct(VOID *module);
UINT32 WX_RS422MasterDriver_Entry(VOID *module, WxMsg *evtMsg);
UINT32 WX_RS422MasterDriver_Destruct(VOID *module);
#endif //__WX_RS422MasterDriver_H__
