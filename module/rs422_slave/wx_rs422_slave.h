#ifndef __WX_RS422_SLAVE_H__
#define __WX_RS422_SLAVE_H__
#include "wx_modbus.h"
#include "wx_id_def.h"
/******************************************************************************
 * 读数据请求
******************************************************************************/
typedef UINT32 (*WxRs422SlaveGetDataHandle)(WxRs422SlaveData *data);
typedef UINT32 (*WxRs422SlaveEncDataHandle)(WxRs422SlaveData *data, WxRs422SlaveMsg *req, WxModbusAdu *adu);
/* 读数据的hanlde - g_wxRs422SlaveRdDataHandles for detail */
typedef struct {
    WxRs422SlaveGetDataHandle readData; /* 读取要获取的数据，数据获取的handle */
    WxRs422SlaveEncDataHandle encAdu;   /* 对读取的数据进行编码 */
} WxRs422SlaveRdDataHandle;

/******************************************************************************
 * 写数据请求
******************************************************************************/
/* 码流解码到数据结构体 */
typedef UINT32 (*WxRs422SlaveDecAduHandle)(WxModbusAdu *adu, WxRs422SlaveData *data);
/* 数据写到本地 */
typedef UINT32 (*WxRs422SlaveWriteDataHandle)(WxRs422SlaveData *data);

/* 读数据的hanlde - g_wxRs422SlaveRdDataHandles for detail */
typedef struct {
    WxRs422SlaveDecAduHandle decAdu;       /* 解码ADU */
    WxRs422SlaveWriteDataHandle writeData; /* 数据写到本地 */
} WxRs422SlaveWrDataHandle;

/******************************************************************************
 * RS422从机的顶层配置
******************************************************************************/
typedef WxRs422SlaveWrDataHandle (*WxRs422SlaveGetWrDataHandles)(UINT8 moduleId, UINT16 *handleNum); /* 获取写数据的hanlde */
typedef WxRs422SlaveRdDataHandle (*WxRs422SlaveGetRdDataHandles)(UINT8 moduleId, UINT16 *handleNum); /* 获取读数据的hanlde */
typedef struct {
    WxModuleId moduleId; /* 模块ID的配置 */
    UINT8 slaveAddr; /* 从机地址 */
    WxRs422SlaveGetWrDataHandles getWrHandles; /* 获取写数据的hanldes */
    WxRs422SlaveGetRdDataHandles getRdHandles; /* 获取读数据的hanldes */
} WxRs422SlaveCfg;

/******************************************************************************
 * RS422从机的顶层，基于Modbus协议
******************************************************************************/
typedef struct {
    UINT8 moduleId; /* 模块ID */
    UINT8 slaveAddr; /* 从机地址 */
    UINT64 excpCnt[WX_MODBUS_EXCP_MAX_EXCP_CODE_NUM]; /* 用于记录不同操作不同从机返回的异常码 */
    WxModbusAdu txAdu;  /* 待发送的Modbus协议ADU */
    UINT16 rdHandleNum; /* 读数据的hanldes的数量 */
    UINT16 wrHandleNum; /* 写数据的hanldes的数量 */
    WxRs422SlaveRdDataHandle *rdDataHandles; /* 读数据的hanlde */
    WxRs422SlaveWrDataHandle *wrDataHandles; /* 写数据的hanlde */
    WxRs422SlaveCfg *cfg; /* 指向配置 */
} WxRs422Slave;

/* get the write data handle */
WxRs422SlaveWrDataHandle *WX_RS422Slave_GetWrDataHandles(UINT8 moduleId, UINT16 *handleNum);
WxRs422SlaveRdDataHandle *WX_RS422Slave_GetRdDataHandles(UINT8 moduleId, UINT16 *handleNum);
/******************************************************************************
 * 函数声明
******************************************************************************/
UINT32 WX_RS422Slave_Construct(VOID *module);
UINT32 WX_RS422Slave_Entry(VOID *module, WxMsg *evtMsg);
UINT32 WX_RS422Slave_Destruct(VOID *module);

#endif
