
#ifndef WT_RS422_I_MASTER_TASK_H
#define WT_RS422_I_MASTER_TASK_H
#include "wx_typedef.h"
#include "wx_rs422_master_rd_data_req_intf.h"
#include "wx_rs422_master_rd_data_rsp_intf.h"
#include "wx_rs422_master_wr_data_req_intf.h"
#include "wx_rs422_master_wr_data_rsp_intf.h"
#include "wx_rs422_master.h"
/* RS422 Master的从机地址定义 */
typedef enum {
    WX_RS422_MASTER_SLAVE_ADDR_NA,
    WX_RS422_MASTER_SLAVE_ADDR_XXX,
    WX_RS422_MASTER_SLAVE_ADDR_YYY,
    WX_RS422_MASTER_SLAVE_ADDR_BUTT,
} WxRs422MasterSlaveAddrDef;

/******************************************************************************
 * 读数据相关
 **/
/* buff = [data0][data1] */
typedef UINT32 (*WxRs422MasterRdDateDecFunc)(UINT8 *buff, UINT8 buffSize, WxRs422MasterReadData *rspStruct);
/* 不同读类型对应的编码 */
typedef struct tagWxRs422MasterRdDataHandle{
    WxRs422MasterSlaveAddrDef slaveDevice; /* 读数据的对应从机设备ID */
    UINT16 dataAddr;  /* 读数据在从机设备中的数据地址 */
    UINT16  dataLen;  /* 读数据的数据长度, 0- 无效值 */
    WxRs422MasterRdDateDecFunc decFunc; /* 读数据的解码函数，把数据流转换位消息结构体 */
} WxRs422MasterRdDataHandle;

UINT32 WX_RS422_MASTER_DecRdDataAdu(WxModbusAdu *rxAdu, WxRs422MasterReadData *readData);
/******************************************************************************
 * 写数据相关
 **/
/* 返回编码的长度 */
typedef UINT8 (*WxRs422MasterEncStructFunc)(UINT8 buf[], UINT8 bufSize, VOID *data);
/* 不同写请求对应的编码信息 */
typedef struct {
    WxRs422MasterSlaveAddrDef slaveDevice; /* 写操作对应从机设备ID */
    UINT32 dataAddr; /* 写操作的数据地址 */
    WxRs422MasterEncStructFunc encStruct; /* 数据结构体编码函数 */
} WxRs422MasterWrDataEncHandle;
WxRs422MasterWrDataEncHandle* WX_RS422_MASTER_GetWrDataHandle(UINT16 subMsgType);
WxRs422MasterRdDataHandle g_wxRs422MasterReadDataHandles[WX_RS422_MASTER_MSG_READ_DATA_BUTT];
/* 任务信息 */
typedef struct {
	UINT8 moduleId;
	UINT8 resv;
	UINT16 resv1;
    WxModbusAdu txAdu;  /* the adu to be send by rs422 */
    UINT8 rdDataModule[WX_RS422_MASTER_MSG_READ_DATA_BUTT]; /* 这个用于记录读是那个模块发起的，用于响应填写接收者 */
    UINT8 wrDataModule[WX_RS422_MASTER_MSG_WRITE_BUTT]; /* 这个用于记录写是那个模块发起的，用于响应填写接收者 */
} WxRs422Master;

UINT32 WX_RS422_MASTER_Construct(VOID *module);
UINT32 WX_RS422_MASTER_Destruct(VOID *module);
UINT32 WX_RS422_MASTER_Entry(VOID *module, WxMsg *msg);
#endif
