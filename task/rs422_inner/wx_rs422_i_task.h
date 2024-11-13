
#ifndef WT_RS422_INNER_TASK_H
#define WT_RS422_INNER_TASK_H
#include "wx_typedef.h"
#include "wx_rs422_i_intf.h"
#define WX_RS422I_MSG_ITERM_MAX_NUM 10 /* 发送消息队列支持的最大支持缓存的消息数 */
#define WX_RS422_ADU_MSX_SIZE 256 /* */
#define WX_RS422I_WAIT_TX_FINISH_TIME  portMAX_DELAY /* 等待发送完成的定时器 portMAX_DELAY: 死等 */
#define WX_RS422I_WAIT_RX_FINISH_TIME  portMAX_DELAY/* 等待接收完成的定时器 portMAX_DELAY: 死等， 实际不需要设置有效的等地时间防止任务挂死 TODO */

/* Modbus操作类型 */
typedef enum {
    WX_RS422I_MB_OPR_RD_DATA,
    WX_RS422I_MB_OPR_WR_DATA,
    WX_RS422I_MB_OPR_RD_FILE,
    WX_RS422I_MB_OPR_WR_FILE,
    WX_RS422I_MB_OPR_BUTT,
} WxRs422IModbusOprType;

/* the application data uint */
typedef struct {
    UINT32 aduLen;
    volatile UINT32 expectRspLen; /* 预期从机响应的报文长度, 当报文为TX是有效 */
    UINT8 adu[WX_RS422_ADU_MSX_SIZE]; /* 数组 */
} WxRs422ITxAdu;

typedef struct {
    UINT32 aduLen;
    BOOL isTimeout; /* 是否超时 */
    UINT8 adu[WX_RS422_ADU_MSX_SIZE]; /* 数组 */
} WxRs422IRxAdu;

typedef struct {
    WxRs422ITxAdu txAdu; /* the adu to be send by rs422 */
    WxRs422IRxAdu rxAdu;  /* the adu recieve from rs422 */
    WxRs422IMsg msgTempBuf; /* A buffer used to store the tx msg from the other module */
    WxRs422IMsg rxMsgTempBuf; /* A buffer uesed to store the rx msg from rs422-i */
    QueueHandle_t msgQueHandle; /* 用于缓存外部模块发送给RS422的待发送的消息队列 */
    XUartNs550 *rs422InstPtr; /* 实例指针 */
    SemaphoreHandle_t aduTxFinishSemaphore; /* 二进制信号，用于表征ADU是否发送完毕 */
    SemaphoreHandle_t aduRxFinishSemaphore; /* 二进制信号，用于表征ADU是否接收完毕 */
    UINT64 slaveExcpCnt[WX_RS422I_MB_OPR_BUTT][WX_RS422I_SLAVE_ADDR_BUTT][WX_MODBUS_MAX_EXCP_CODE_NUM]; /* 用于记录不同操作不同从机返回的异常码 */
} WxRs422ITask;
/* 中断处理函数 */
void WX_RS422I_IntrHandler(void *CallBackRef, u32 Event, unsigned int EventData);
/* 创建422收发任务 */
WxFailCode WX_RS422I_CreateTask(VOID);

#endif
