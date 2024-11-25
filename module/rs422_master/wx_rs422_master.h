
#ifndef WT_RS422_I_MASTER_TASK_H
#define WT_RS422_I_MASTER_TASK_H
#include "wx_typedef.h"
#include "wx_rs422_i_master_intf.h"

#define WX_RS422_MASTER_MSG_ITERM_MAX_NUM     10  /* 发送消息队列支持的最大支持缓存的消息数 */
#define WX_RS422_ADU_MSX_SIZE           256 /* ADU的最大长度 */

/* 设备异常描述 */
typedef struct
{
    WxRs422MasterSlaveAddrDef deviceAddr;   /* 设备地址 */
    UINT8 funcCode;     /* 设备的异常码 */
    CHAR *desc;         /* 设备的异常码的描述 */
} WxRs422IExcpDesc;

/* Modbus操作类型 */
typedef enum {
    WX_RS422_MASTER_MB_OPR_RD_DATA,
    WX_RS422_MASTER_MB_OPR_WR_DATA,
    WX_RS422_MASTER_MB_OPR_RD_FILE,
    WX_RS422_MASTER_MB_OPR_WR_FILE,
    WX_RS422_MASTER_MB_OPR_BUTT,
} WxRs422IModbusOprType;

/* 返回编码的长度 */
typedef UINT8 (*WxRs422MasterEncStructFunc)(UINT8 buf[], UINT8 bufSize, VOID *data);

/* 解码函数 */
typedef UINT32 (*WxRs422IReadDateDecodeFunc)(UINT8 data[], UINT8 dataLen, WxRs422MasterReadData *rspStruct);

/* 不同读类型对应的编码 */
typedef struct {
    WxRs422MasterSlaveAddrDef slaveDevice; /* 读数据的对应从机设备ID */
    UINT16 dataAddr;  /* 读数据在从机设备中的数据地址 */
    UINT16  dataLen;  /* 读数据的数据长度, 0- 无效值 */
    WxRs422IReadDateDecodeFunc decFunc; /* 读数据的解码函数，把数据流转换位消息结构体 */
} WxRs422IReadDataHandle;
UINT32 WX_RS422_MASTER_EncodeAduReadDataReq(WxRs422IMasterMsg *txMsg, WxModbusAdu *txAdu);
WxRs422IReadDateDecodeFunc WX_RS422_MASTER_GetDecodeFunc(WxRs422MasterRdDataType dataType);

/* 不同写请求对应的编码信息 */
typedef struct {
    WxRs422MasterSlaveAddrDef slaveDevice; /* 写操作对应从机设备ID */
    UINT32 dataAddr; /* 写操作的数据地址 */
    WxRs422MasterEncStructFunc encStruct; /* 数据结构体编码函数 */
} WxRs422MasterWrDataEnchHandle;
UINT32 WX_RS422_MASTER_EncodeWrDataMsg2Adu(WxMsg *req, WxModbusAdu *adu);

/* 不同读类型对应的编码 */
typedef struct {
    WxRs422MasterSlaveAddrDef slaveDevice; /* 读数据的对应从机设备ID */
    UINT16 dataAddr;  /* 读数据在从机设备中的数据地址 */
    UINT16  dataLen;  /* 读数据的数据长度, 0- 无效值 */
} WxRs422IReadDataHandle;


/* 不同写请求对应的编码信息 */
typedef struct {
    WxRs422MasterSlaveAddrDef slaveDevice; /* 写操作对应从机设备ID */
    WxRs422MasterEncStructFunc encStruct; /* 数据结构体编码函数 */
} WxRs422MasterWrDataEnchHandle;

/* RS422配置信息 */
typedef struct {
    UINT32 taskPri; /* 任务优先级 */
    UINT32 rs422DevId;
    XUartNs550Format rs422Format; /* 串口配置信息 */
    WxUartNs550IntrCfg rs422IntrCfg; /* 串口中断配置 */
} WxRs422MasterCfgInfo;

/* 任务信息 */
typedef struct {
    WxModbusAdu txAdu;  /* the adu to be send by rs422 */
    WxModbusAdu rxAdu;  /* the adu recieve from rs422 */
    WxRs422IMasterMsg rs422Msg; /* A buffer used to store the tx/rx msg from the other module */
    QueueHandle_t msgQueHandle; /* 用于缓存外部模块发送给RS422的待发送的消息队列 */
    XUartNs550 rs422Inst; /* RS422实例 */
    SemaphoreHandle_t aduTxFinishSemaphore; /* 二进制信号，用于表征ADU是否发送完毕 */
    SemaphoreHandle_t aduRxFinishSemaphore; /* 二进制信号，用于表征ADU是否接收完毕 */
    UINT64 slaveExcpCnt[WX_RS422_MASTER_MB_OPR_BUTT][WX_RS422_MASTER_SLAVE_ADDR_BUTT][WX_MODBUS_MAX_EXCP_CODE_NUM]; /* 用于记录不同操作不同从机返回的异常码 */
} WxRs422Master;

UINT32 WX_RS422_MASTER_DecodeAdu(WxModbusAdu *txAdu, WxModbusAdu *rxAdu, WxRs422IMasterMsg *rxMsgBuf);;
UINT32 WX_RS422_MASTER_EncodeAdu(WxRs422IMasterMsg *txMsg, WxModbusAdu *txAdu);
UINT32 WX_RS422_MASTER_DecodeAduReadDataResponce(WxRs422Master *this, WxModbusAdu *txAdu, WxModbusAdu *rxAdu, WxRs422IMasterMsg *rspMsp);
UINT32 WX_RS422_MASTER_DecodeAduExcpResponce(WxRs422Master *this, WxModbusAdu *txAdu, WxModbusAdu *rxAdu, WxRs422IMasterMsg *rxMsg);
#endif
