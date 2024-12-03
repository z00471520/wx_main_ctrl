#ifndef __WX_RS422_SLAVE_DRIVER_H__
#define __WX_RS422_SLAVE_DRIVER_H__
#include "wx_modbus.h"
#include "wx_include.h"
#include "xuartns550.h"
#include "FreeRTOS.h"
#include "wx_msg_common.h"
#include "queue.h"
/* RS422閰嶇疆淇℃伅 */
typedef struct {
    UINT32 rs422DevId;  /* 璁惧ID */
    UINT32 intrId;      /* 涓柇ID */
    UINT8 slaveAddr;    /* 浠庢満鍦板潃 */
    UINT8 moduleId;     /* moduleId */
    UINT8 upperModuleId; /* upperModuleId */
    UINT8 resv8;      /* resv16 */
    XUartNs550Format rs422Format; /* 涓插彛閰嶇疆淇℃伅 */
} WxRs422SlaveDriverCfg;

/* RS422 Slave Driver */
typedef struct {
    UINT8 resv; 			/* resv for you no thks */
    UINT8 moduleId;			/* the module ID of module */
    UINT8 slaveAddr; 		/* the slave addr of he device */
    UINT8 upperModuleId; 	/* upperModuleId */
    UINT32 resv3222;
    WxModbusAdu txAdu; 		/* the adu to be send by rs422 */
    WxModbusAdu rxAdu;  	/* the adu recieve from rs422 */
    XUartNs550 rs422Inst; 	/* RS422 instance */
} WxRs422SlaverDriver;

UINT32 WX_RS422SlaveDriver_Construct(VOID *module);
UINT32 WX_RS422SlaveDriver_Entry(VOID *module, WxMsg *evtMsg);
UINT32 WX_RS422SlaveDriver_Destruct(VOID *module);
#endif //__WX_RS422MasterDriver_H__
