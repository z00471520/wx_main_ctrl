#ifndef WX_RS422I_ENCODE_ADU_H
#define WX_RS422I_ENCODE_ADU_H
#include "wx_rs422_i_task.h"
#include "wx_rs422_i_encode_adu.h"
#include "wx_rs422_i_encode_adu_write_req.h"
#include "wx_rs422_i_encode_adu_read_req.h"

/* 不同读类型对应的编码 */
typedef struct {
    WxRs422ISlaveAddrDef slaveDevice; /* 读数据的对应从机设备ID */
    UINT16 dataAddr;  /* 读数据在从机设备中的数据地址 */
    UINT16  dataLen;  /* 读数据的数据长度, 0- 无效值 */
} WxRs422IReadDataHandle;

typedef WxFailCode (*WxRs422IEncodeDataStructFunc)(UINT8 buf[], UINT8 bufSize, VOID *data);

/* 不同写请求对应的编码信息 */
typedef struct {
    WxRs422ISlaveAddrDef slaveDevice; /* 写操作对应从机设备ID */
    WxRs422IEncodeDataStructFunc dataStrucEncodeFunc; /* 数据结构体编码函数 */
} WxRs422IWriteDataEncodeInfo;

WxFailCode WX_RS422I_EncodeAdu(WxRs422IMsg *txMsg, WxRs422IAdu *txAdu);
#endif