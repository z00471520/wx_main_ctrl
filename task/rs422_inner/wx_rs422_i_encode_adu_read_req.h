#ifndef WX_RS422I_ENCODE_H
#define WX_RS422I_ENCODE_H
#include "wx_rs422_i_task.h"

/* 不同读类型对应的编码 */
typedef struct {
    WxRs422ISlaveAddrDef slaveDevice; /* 读数据的对应从机设备ID */
    UINT16 dataAddr;  /* 读数据在从机设备中的数据地址 */
    UINT16  dataLen;  /* 读数据的数据长度, 0- 无效值 */
} WxRs422IReadDataEncodeInfo;
WxFailCode WX_RS422I_EncodeAduReadDataReq(WxRs422IMsg *txMsg, WxRs422IAdu *txAdu);
#endif