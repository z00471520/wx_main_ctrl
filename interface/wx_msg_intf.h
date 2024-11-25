
#ifndef WX_MSG_INTF_H
#define WX_MSG_INTF_H
#include "wx_typedef.h"
#define WX_EVT_MSG_DATA_SIZE 1500 /* 默认的消息长度 */

/* 核ID定义 */
typedef enum {
    WX_CORE_ID_NA = 0,
    WX_CORE_ID_0 = 0x1,
    WX_CORE_ID_1 = 0x2,
    WX_CORE_ID_2 = 0x4,
    WX_CORE_ID_3 = 0x8,
} WxCoreId;
#define WX_CORE_ID_ALL 0xF
#define WX_CORE_NUM     4
#define WX_CORE_IDX_2_ID(coreIdx) ((UINT32)1 << (coreIdx))
inline UINT32 WX_CORE_ID_2_IDX(WxCoreId coreId)
{
    switch (coreId) {
        case WX_CORE_ID_0:
            return 0;
        case WX_CORE_ID_1:
            return 1;
        case WX_CORE_ID_2:
            return 2;
        case WX_CORE_ID_3:
            return 3;
        default:
            return ;
    }
}
#define WX_IS_VALID_CORE_ID(coreId) (((coreId) != WX_CORE_ID_NA) && ((coreId) & WX_CORE_ID_ALL == (coreId)))

/* 当前内核支持的消息处理模块定义 */
typedef enum {
    /**************************************************************************
     * Task's module ID
     *************************************************************************/
    WX_MODULE_INVALID, /* 无效ID */
    WX_MODULE_CAN_SLAVE_A,
    WX_MODULE_CAN_SLAVE_B,
    WX_MODULE_RS422_I_MASTER, /* 内部外设通信使用的RS422 */
    WX_MODULE_ZJ_SPI_DRIVER,
    WX_MODULE_DRIVER_CAN_A,
    WX_MODULE_DRIVER_CAN_B,
    WX_MODULE_DRIVER_RS422_MASTER,
    WX_MODULE_DRIVER_RS422_SLAVE,
    /* if more please add here */
    WX_MODULE_BUTT,
} WxModuleId;

#define WX_IsValidModuleId(i) ((i) > WX_MODULE_INVALID && (i) < WX_MODULE_BUTT)

/* 消息大类 */
typedef enum {
    WX_MSG_TYPE_INVALID,
    WX_MSG_TYPE_REMOTE_CTRL,        /* 遥控消息， see wx_remote_ctrl_msg_def.h for detail */
    WX_MSG_TYPE_CAN_FRAME,          /* CAN FRAME消息，see wx_msg_can_frame_intf.h.h for detail */
    WX_RS422I_MASTER_MSG_READ_DATA,        /* 读数据请求, 子类型：WxRs422IReadDataType,  消息体为：NA */
    WX_RS422I_MASTER_MSG_READ_DATA_RSP,    /* 读数据响应, 子类型：WxRs422IReadDataType,  消息体为：WxRs422IReadDataRsp */
    WX_RS422I_MASTER_MSG_WRITE_DATA,       /* 写数据请求, 子类型：WxRs422IWriteDataType, 消息体为: WxRs422IWriteData */
    WX_RS422I_MASTER_MSG_WRITE_DATA_RSP,   /* 写数据响应, 子类型: WxRs422IWriteDataType, NA */
    WX_RS422I_MASTER_MSG_READ_FILE,
    WX_RS422I_MASTER_MSG_READ_FILE_RSP,
    WX_RS422I_MASTER_MSG_WRITE_FILE,
    WX_RS422I_MASTER_MSG_WRITE_FILE_RSP,
    /* if more please add here */
    WX_MSG_TYPE_BUTT,
} WxMsgType;
#define WX_IsValidMsgType(t) ((t) > WX_MSG_TYPE_INVALID && (t) < WX_MSG_TYPE_BUTT)

typedef struct {
    UINT32 transID;    /* 消息对应的事务ID */
    UINT8 sender;   /* 消息发送模块ID, 详见: WxModuleId */
    UINT8 receiver; /* 消息接收模块ID, 详见: WxModuleId */
   	UINT16 msgType;    /* 消息类型, 详见枚举 WxMsgType 定义 */
    UINT16 msgSubType; /* 消息子类型, 由大类确定 */
    UINT16 msgDataLen; /* 消息体实际长度 --- msgData的长度, 必选：不能超过，WX_MSG_DEFAULT_BODY_SIZE */
    UINT16 outEvent;   /* 消息处理的出事件， WX_SUCCESS - 表示成功 */
    UINT8  msgData[0]; /* 最大长度为：WX_EVT_MSG_DATA_SIZE */
} WxEvtMsg;

#endif