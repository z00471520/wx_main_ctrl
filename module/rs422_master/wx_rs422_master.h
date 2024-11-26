
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

/* 解码函数buff = [data0][data1] */
typedef UINT32 (*WxRs422MasterRdDateDecFunc)(UINT8 buff[], UINT8 buffSize, WxRs422MasterReadData *rspStruct);

/* 不同读类型对应的编码 */
typedef struct {
    WxRs422MasterSlaveAddrDef slaveDevice; /* 读数据的对应从机设备ID */
    UINT16 dataAddr;  /* 读数据在从机设备中的数据地址 */
    UINT16  dataLen;  /* 读数据的数据长度, 0- 无效值 */
    WxRs422MasterRdDateDecFunc decFunc; /* 读数据的解码函数，把数据流转换位消息结构体 */
} WxRs422MasterRdDataHandle;

UINT32 WX_RS422_MASTER_DecRdDataAdu(WxModbusAdu *rxAdu, WxRs422MasterReadData *readData);
/* 返回编码的长度 */
typedef UINT8 (*WxRs422MasterEncStructFunc)(UINT8 buf[], UINT8 bufSize, VOID *data);
/* 不同写请求对应的编码信息 */
typedef struct {
    WxRs422MasterSlaveAddrDef slaveDevice; /* 写操作对应从机设备ID */
    UINT32 dataAddr; /* 写操作的数据地址 */
    WxRs422MasterEncStructFunc encStruct; /* 数据结构体编码函数 */
} WxRs422MasterWrDataEncHandle;



/* 任务信息 */
typedef struct {
    WxModbusAdu txAdu;  /* the adu to be send by rs422 */
    UINT64 slaveExcpCnt[WX_RS422_MASTER_MB_OPR_BUTT][WX_RS422_MASTER_SLAVE_ADDR_BUTT][WX_MODBUS_MAX_EXCP_CODE_NUM]; /* 用于记录不同操作不同从机返回的异常码 */
    UINT8 rdDataModule[WX_RS422_MASTER_MSG_READ_DATA_BUTT]; /* 不同消息类型对应的读模块 */
    UINT8 wrDataModule[WX_RS422_MASTER_MSG_WRITE_BUTT]; /* 不同消息类型对应的写模块 */
} WxRs422Master;
WxRs422MasterWrDataEncHandle* WX_RS422_MASTER_GetWrDataHandle(UINT16 subMsgType);
UINT32 WX_RS422_MASTER_DecodeAdu(WxModbusAdu *txAdu, WxModbusAdu *rxAdu, WxRs422IMasterMsg *rxMsgBuf);;
UINT32 WX_RS422_MASTER_EncRdDataReqMsg2AduodeAdu(WxRs422IMasterMsg *txMsg, WxModbusAdu *txAdu);
UINT32 WX_RS422_MASTER_DecodeAduReadDataResponce(WxRs422Master *this, WxModbusAdu *txAdu, WxModbusAdu *rxAdu, WxRs422IMasterMsg *rspMsp);
UINT32 WX_RS422_MASTER_DecodeAduExcpResponce(WxRs422Master *this, WxModbusAdu *txAdu, WxModbusAdu *rxAdu, WxRs422IMasterMsg *rxMsg);
#endif
