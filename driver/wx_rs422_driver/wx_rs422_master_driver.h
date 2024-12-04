#ifndef __WX_RS422MasterDriver_H__
#define __WX_RS422MasterDriver_H__
#include "wx_modbus.h"
#include "wx_include.h "
#include "wx_rs422_master_driver_intf.h"
typedef enum {
    WX_RS422_MASTER_STATUS_IDLE = 0,
    WX_RS422_MASTER_STATUS_TX_ADU,
    WX_RS422_MASTER_STATUS_RX_ADU,
} WxRs422Status;

typedef struct {
    UINT32 rs422DevId;
    UINT32 intrId;
    UINT32 gpioDevId;
    UINT8 upperModuleId;
    UINT8 resv33;
    UINT16 resv;
    UINT32 msgQueItemNum;
    XUartNs550Format rs422Format;
} WxRs422MasterDriverCfg;

typedef struct {
    UINT8 status; /*  master's txrx status see WxRs422Status for detail  */
    UINT8 moduleId; /* module id  */
    UINT8 upperModuleId; /* upper module id */
    UINT8 resv1;
    WxRs422MasterDriverTxDataReq txAduInfo; /* The latest ADU message sent by rs422 master  */
    WxModbusAdu rxAdu;  /* the adu recieve from rs422 */
    QueueHandle_t msgQueHandle; /* adu msg to be sent struct is WxRs422MasterDriverTxDataReq */
    XUartNs550 rs422Inst; /* RS422 inst  */
    XGpio gpipInst; /* GPIO inst to control rs422's direction */
} WxRs422DriverMaster;

UINT32 WX_RS422MasterDriver_Construct(VOID *module);
UINT32 WX_RS422MasterDriver_Entry(VOID *module, WxMsg *evtMsg);
UINT32 WX_RS422MasterDriver_Destruct(VOID *module);
#endif //__WX_RS422MasterDriver_H__
