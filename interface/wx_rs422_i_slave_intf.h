#ifndef WX_RS422_I_SLAVE_INTF_H
#define WX_RS422_I_SLAVE_INTF_H
/******************************************************************************
 * 异常码定义
 *****************************************************************************/
typedef enum {
    WX_RS422_SLAVE_EXCP_CODE_UNKNOWN, /* 未知错误 */
    WX_RS422_SLAVE_EXCP_CODE_UNSPT_WR,
    WX_RS422_SLAVE_EXCP_CODE_UNSPT_RD,
    WX_RS422_SLAVE_EXCP_CODE_UNSPT_FUNC_CODE,   /* 不支持的功能码 */
    WX_RS422_SLAVE_EXCP_CODE_UNSPT_DATA_ADDR,   /* 不支持的数据地址 */
    WX_RS422_SLAVE_EXCP_CODE_RD_HANDLE_UNFINE,  /* 读handle未定义 */
    WX_RS422_SLAVE_EXCP_CODE_RD_DATA_GET_FAIL,  /* 读数据获取失败 */
    WX_RS422_SLAVE_EXCP_CODE_RD_DATA_ENC_FAIL,  /* 读数据编码失败 */
    WX_RS422_SLAVE_EXCP_CODE_RD_SEND_RSP_FAIL,  /* 读操作发送响应消息失败 */
    WX_RS422_SLAVE_EXCP_CODE_WR_HANDLE_UNFINE,
    WX_RS422_SLAVE_EXCP_CODE_WR_DEC_FAIL,
    WX_RS422_SLAVE_EXCP_CODE_WR_DATA_FAIL,
    WX_RS422_SLAVE_EXCP_CODE_WR_FILE_UNSPT,
    WX_RS422_SLAVE_EXCP_CODE_RD_FILE_UNSPT,
    WX_RS422_SLAVE_EXCP_CODE_CRC_ERR,
    /* if more please add here */
} WxRs422SlaveExcpCodeDef;

/******************************************************************************
 * 数据地址请求
 *****************************************************************************/
typedef enum {
    WX_RS422_I_SLAVE_DATA_ADDR_X = 1,
    /* if more please add here */
    WX_RS422_I_SLAVE_DATA_ADDR_BUTT,
} WxRs422ISlaveDataAddr;

/******************************************************************************
 * 数据格式定义
 *****************************************************************************/
typedef struct {

} WxRs422ISlaveDataX;

typedef struct {
    /* 联合体由数据地址确定 */
    union {
        WxRs422ISlaveDataX dataX;
        /* if more please add here */
    };
} WxRs422ISlaveData;

/* 从机数据请求 */
typedef struct {
    UINT8 salveAddr;            /* 从机地址 */
    UINT8 funcCode;             /* 功能码 */ 
    UINT8 excpCode;             /* 异常码 - 当功能码为异常码时有效, 详见： WxRs422SlaveExcpCodeDef */
    UINT16 dataAddr;            /* 数据地址, 当为读写数据时有效，由枚举WxRs422ISlaveDataAddr定义 */
    WxRs422ISlaveData data;     /* 读写数据的结构体 */
} WxRs422ISlaveMsg;

#endif