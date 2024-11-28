#ifndef __WX_RS422_DRIVER_MASTER_H__
#define __WX_RS422_DRIVER_MASTER_H__

typedef enum {
    WX_RS422_MASTER_STATUS_IDLE = 0,
    WX_RS422_MASTER_STATUS_TX_ADU,
    WX_RS422_MASTER_STATUS_RX_ADU,
} WxRs422Status;

typedef struct {
    UINT8 status; /* 状态 */
    WxModbusAdu txAdu; /* the adu to be send by rs422 */
    WxModbusAdu rxAdu;  /* the adu recieve from rs422 */
    QueueHandle_t msgQueHandle; /* 用于缓存外部模块发送给RS422的待发送的消息队列 */
    XUartNs550 rs422Inst; /* RS422实例 */
    UINT64 slaveExcpCnt[WX_RS422_MASTER_MB_OPR_BUTT][WX_RS422_MASTER_SLAVE_ADDR_BUTT][WX_MODBUS_MAX_EXCP_CODE_NUM]; /* 用于记录不同操作不同从机返回的异常码 */
} WxRs422DriverMaster;

UINT32 WX_RS422_DRIVER_MASTER_Construct(VOID *module);
UINT32 WX_RS422_DRIVER_MASTER_Entry(VOID *module, WxMsg *evtMsg);
UINT32 WX_RS422_DRIVER_MASTER_Destruct(VOID *module);
#endif //__WX_RS422_DRIVER_MASTER_H__