#ifndef WX_RS422_I_SLAVE_TASK_H
#define WX_RS422_I_SLAVE_TASK_H
#include "wx_uart_ns550.h"
#include "wx_modbus.h"
#include "wx_rs422_i_master_intf.h"
#include "wx_rs422_i_slave_intf.h"

/******************************************************************************
 * 读数据请求
******************************************************************************/
typedef UINT32 (*WxRs422ISlaveGetDataHandle)(WxRs422ISlaveData *data);
typedef UINT32 (*WxRs422ISlaveEncDataHandle)(WxRs422ISlaveData *data, WxRs422ISlaveMsg *req, WxModbusAdu *adu);
/* 读数据的hanlde - g_wxRs422ISlaveDataReadHandles for detail */
typedef struct {
    WxRs422ISlaveGetDataHandle readData; /* 数据获取的handle */
    WxRs422ISlaveEncDataHandle encAdu; /* 数据编码的Hanle */
} WxRs422ISlaveDataReadHandle;

/******************************************************************************
 * 写数据请求
******************************************************************************/
/* 码流解码到数据结构体 */
typedef UINT32 (*WxRs422ISlaveDecAduHandle)(WxModbusAdu *adu, WxRs422ISlaveData *data);
/* 数据写到本地 */
typedef UINT32 (*WxRs422ISlaveWriteDataHandle)(WxRs422ISlaveData *data);

/* 读数据的hanlde - g_wxRs422ISlaveDataReadHandles for detail */
typedef struct {
    WxRs422ISlaveDecAduHandle decAdu;       /* 解码ADU */
    WxRs422ISlaveWriteDataHandle writeData; /* 数据写到本地 */
} WxRs422ISlaveDataWriteHandle;

/******************************************************************************
 * RS422配置信息
******************************************************************************/
typedef struct {
    UINT8 salveAddr; /* 本从机的地址 */
    UINT32 taskPri; /* 任务优先级 */
    UINT32 rs422DevId; /* 设别ID */
    XUartNs550Format rs422Format; /* 串口配置信息 */
    WxUartNs550IntrCfg rs422IntrCfg; /* 串口中断配置 */
} WxRs422ISlaveTaskCfgInfo;

/******************************************************************************
 * 任务信息
******************************************************************************/
typedef struct {
    WxModbusAdu txAdu; /* the adu to be send by rs422 */
    WxModbusAdu rxAdu;  /* the adu recieve from rs422 */
    WxRs422ISlaveMsg msg; /* 从机收到的消息 */
    XUartNs550 rs422Inst; /* RS422实例 */
    SemaphoreHandle_t aduTxFinishSemaphore; /* 二进制信号，用于表征ADU是否发送完毕 */
    SemaphoreHandle_t aduRxFinishSemaphore; /* 二进制信号，用于表征ADU是否接收完毕 */
    UINT64 excpCnt[WX_RS422_MASTER_MB_OPR_BUTT][WX_MODBUS_MAX_EXCP_CODE_NUM]; /* 用于记录不同操作不同从机返回的异常码 */
} WxRs422ISlaveTask;

/******************************************************************************
 * 函数声明
******************************************************************************/
WxRs422ISlaveDataReadHandle *WX_RS422I_Slave_GetDataReadHandle(WxRs422ISlaveDataAddr dataAddr);
WxRs422ISlaveDataWriteHandle *WX_RS422I_Slave_GetDataWriteHandle(WxRs422ISlaveDataAddr dataAddr);
#endif