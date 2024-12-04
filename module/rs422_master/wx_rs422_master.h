
#ifndef WT_RS422_I_MASTER_TASK_H
#define WT_RS422_I_MASTER_TASK_H
#include "wx_typedef.h"
#include "wx_rs422_master_rd_data_req_intf.h"
#include "wx_rs422_master_rd_data_rsp_intf.h"
#include "wx_rs422_master_wr_data_req_intf.h"
#include "wx_rs422_master_wr_data_rsp_intf.h"
#include "wx_rs422_master_adu_rsp_intf.h"
#include "wx_rs422_master_driver_intf.h"
#include "wx_rs422_master.h"
/* RS422 Master鐨勪粠鏈哄湴鍧�瀹氫箟 */
typedef enum {
    WX_RS422_MASTER_SLAVE_ADDR_NA,
    WX_RS422_MASTER_SLAVE_ADDR_XXX,
    WX_RS422_MASTER_SLAVE_ADDR_YYY,
    WX_RS422_MASTER_SLAVE_ADDR_BUTT,
} WxRs422MasterSlaveAddrDef;

/******************************************************************************
 * 璇绘暟鎹浉鍏�
 **/
/* buff = [data0][data1] */
typedef UINT32 (*WxRs422MasterRdDateDecFunc)(UINT8 *buff, UINT8 buffSize, WxRs422MasterReadData *rspStruct);
/* 涓嶅悓璇荤被鍨嬪搴旂殑缂栫爜 */
typedef struct tagWxRs422MasterRdDataHandle{
    WxRs422MasterSlaveAddrDef slaveDevice; /* 璇绘暟鎹殑瀵瑰簲浠庢満璁惧ID */
    UINT16 dataAddr;  /* 璇绘暟鎹湪浠庢満璁惧涓殑鏁版嵁鍦板潃 */
    UINT16  dataLen;  /* 璇绘暟鎹殑鏁版嵁闀垮害, 0- 鏃犳晥鍊� */
    WxRs422MasterRdDateDecFunc decFunc; /* 璇绘暟鎹殑瑙ｇ爜鍑芥暟锛屾妸鏁版嵁娴佽浆鎹綅娑堟伅缁撴瀯浣� */
} WxRs422MasterRdDataHandle;

UINT32 WX_RS422_MASTER_DecRdDataRsp(WxRs422MasterDriverRspData *rspData, WxRs422MasterReadData *readData);
/******************************************************************************
 * 鍐欐暟鎹浉鍏�
 **/
/* 杩斿洖缂栫爜鐨勯暱搴� data is the struct to be encoded */
typedef UINT8 (*WxRs422MasterEncStructFunc)(WxModbusAdu *adu, WxRs422MasterWrData *data);
/* 涓嶅悓鍐欒姹傚搴旂殑缂栫爜淇℃伅 */
typedef struct {
    WxRs422MasterSlaveAddrDef slaveDevice; /* 鍐欐搷浣滃搴斾粠鏈鸿澶嘔D */
    UINT32 dataAddr; /* 鍐欐搷浣滅殑鏁版嵁鍦板潃 */
    WxRs422MasterEncStructFunc encStruct; /* write data struct to data stream, 鏁版嵁缁撴瀯浣撶紪鐮佸嚱鏁� */
} WxRs422MasterWrDataEncHandle;
WxRs422MasterWrDataEncHandle* WX_RS422_MASTER_GetWrDataHandle(UINT16 subMsgType);
WxRs422MasterRdDataHandle g_wxRs422MasterReadDataHandles[WX_RS422_MASTER_MSG_READ_DATA_BUTT];
/* 浠诲姟淇℃伅 */
typedef struct {
	UINT8 moduleId;
	UINT8 resv;
	UINT16 resv1;
    WxRs422MasterDriverTxDataReq txDataReq;  /* the data info to be send by rs422 */
} WxRs422Master;

UINT32 WX_RS422_MASTER_Construct(VOID *module);
UINT32 WX_RS422_MASTER_Destruct(VOID *module);
UINT32 WX_RS422_MASTER_Entry(VOID *module, WxMsg *msg);
#endif
