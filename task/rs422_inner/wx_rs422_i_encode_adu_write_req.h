#ifndef WX_RS422I_ENCODE_ADU_WRITE_REQ_H
#define WX_RS422I_ENCODE_ADU_WRITE_REQ_H
#include "wx_rs422_i_task.h"
#include "wx_rs422_i_encode_adu.h"
typedef WxFailCode (*WxRs422IEncodeDataStructFunc)(UINT8 buf[], UINT8 bufSize);

/* 不同写请求对应的编码信息 */
typedef struct {
    WxRs422ISlaveAddrDef slaveDevice; /* 写操作对应从机设备ID */
    UINT16 dataAddr; /* 写操作的数据地址 */
    WxRs422IEncodeDataStructFunc dataStrucEncodeFunc; /* 数据结构体编码函数 */
} WxRs422IWriteDataEncodeInfo;
WxFailCode WX_RS422I_EncodeAduWriteDataReq(WxRs422IMsg *txMsg, WxRs422IAdu *txAdu);
#endif