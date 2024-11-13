#ifndef WX_RS422I_ENCODE_ADU_RD_DATA_REQ_H
#define WX_RS422I_ENCODE_ADR_RD_DATA_REQ_H
#include "wx_rs422_i_task.h"
/* 解码函数 */
typedef WxFailCode (*WxRs422IReadDateDecodeFunc)(UINT8 data[], UINT8 dataLen, WxRs422IReadDataRsp *rspStruct);

/* 不同读类型对应的编码 */
typedef struct {
    WxRs422ISlaveAddrDef slaveDevice; /* 读数据的对应从机设备ID */
    UINT16 dataAddr;  /* 读数据在从机设备中的数据地址 */
    UINT16  dataLen;  /* 读数据的数据长度, 0- 无效值 */
    WxRs422IReadDateDecodeFunc decFunc; /* 读数据的解码函数，把数据流转换位消息结构体 */
} WxRs422IReadDataHandle;
WxFailCode WX_RS422I_EncodeAduReadDataReq(WxRs422IMsg *txMsg, WxRs422IAdu *txAdu);
WxRs422IReadDateDecodeFunc WX_RS422I_GetDecodeFunc(WxRs422IReadDataType dataType);

#endif