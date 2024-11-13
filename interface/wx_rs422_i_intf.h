#ifndef WX_RS422_INNER_INTF_H
#define WX_RS422_INNER_INTF_H
#include "wx_typedef.h"
#include "wx_modbus.h"
#define WX_RS422I_MSG_BODY_SIZE WX_MODBUS_ADU_MAX_SIZE             /* 消息体的长度 */

/* RS422 I 从机地址定义 */
typedef enum {
    WX_RS422I_SLAVE_ADDR_NA,
    WX_RS422I_SLAVE_ADDR_XXX,
    WX_RS422I_SLAVE_ADDR_YYY,
    WX_RS422I_SLAVE_ADDR_BUTT,
} WxRs422ISlaveAddrDef;

/* RS422消息大类 */
typedef enum {
    WX_RS422I_MSG_READ_DATA,        /* 读数据请求, 子类型：WxRs422IReadDataType,  消息体为：NA */
    WX_RS422I_MSG_READ_DATA_RSP,    /* 读数据响应, 子类型：WxRs422IReadDataType,  消息体为：WxRs422IReadDataRsp */
    WX_RS422I_MSG_WRITE_DATA,       /* 写数据请求, 子类型：WxRs422IWriteDataType, 消息体为: WxRs422IWriteData */
    WX_RS422I_MSG_WRITE_DATA_RSP,   /* 写数据响应, 子类型: WxRs422IWriteDataType, NA */
    WX_RS422I_MSG_READ_FILE,
    WX_RS422I_MSG_READ_FILE_RSP,
    WX_RS422I_MSG_WRITE_FILE,
    WX_RS422I_MSG_WRITE_FILE_RSP,
    /* if more please add here */
} WxRs422IMsgType;

/******************************************************************************
 * 读数据请求/响应的消息子类型定义
 ******************************************************************************/
typedef enum {
    WX_RS422_I_MSG_READ_DATA_XXX = 0, /* 数据结构体为： WxRs422IDataXXXInfo */
    WX_RS422_I_MSG_READ_DATA_YYY = 1, /* 数据结构体为： WxRs422IDataYYYInfo */
    WX_RS422_I_MSG_READ_DATA_BUTT,
} WxRs422IReadDataType;

/******************************************************************************
 * 读数据响应
 ******************************************************************************/
typedef struct {

} WxRs422IDataXXXInfo;

typedef struct {
    
} WxRs422IDataYYYInfo;

/* 读数据成功的响应 */
typedef struct {
    /*
     * 具体联合体选择哪一个由枚举 WxRs422IReadDataType 确定
     */
    union { 
        WxRs422IDataXXXInfo xxxInfo;
        WxRs422IDataYYYInfo yyyInfo;
        /* if more please add here */
    };
} WxRs422IReadDataRsp;

/******************************************************************************
 * 写数据请求和响应的子类型定义
******************************************************************************/
typedef enum {
    WX_RS422_I_MSG_WRITE_DATA_MMM, /* WxRs422IDataMMMInfo */
    WX_RS422_I_MSG_WRITE_DATA_NNN, /* WxRs422IDataNNNInfo */
    WX_RS422_I_MSG_WRITE_DATA_BUTT,
} WxRs422IWriteDataType;

/******************************************************************************
 * 写数据请求的结构体定义
******************************************************************************/
typedef struct {

} WxRs422IDataMMMInfo;

typedef struct {
    
} WxRs422IDataNNNInfo;

/* 写数据请求的内容，由子类型确定， 所以这里是一个枚举值 */
typedef struct {
    union {
        WxRs422IDataMMMInfo MMM;
        WxRs422IDataNNNInfo NNN; 
    };
} WxRs422IWriteData;

/******************************************************************************
 * RS422消息
 *****************************************************************************/
typedef struct {
    UINT32 transID; /* 消息对应的事务ID */
   	UINT16 msgType;    /* 消息类型, 详见枚举 WxRs422InnerMsgType 定义 */
    UINT16 msgSubType; /* 消息子类型, 由大类确定 */
    UINT16 msgBodyLen; /* 消息体实际长度 --- msgBody的长度 */
    UINT16 failCode;  /* 消息处理的失败码， WX_SUCCESS - 表示成功 */  
    UINT8 msgBody[WX_RS422I_MSG_BODY_SIZE];  /* Msg body 由msgType和msgSubType确定，由通信双方自行约束 */
} WxRs422IMsg;

#endif
