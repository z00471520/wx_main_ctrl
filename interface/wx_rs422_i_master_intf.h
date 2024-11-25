#ifndef WX_RS422_I_MASTER_INTF_H
#define WX_RS422_I_MASTER_INTF_H
#include "wx_typedef.h"
#include "wx_modbus.h"
#include "wx_msg_intf.h"
#define WX_RS422I_MASTER_MSG_BODY_SIZE WX_MODBUS_ADU_MAX_SIZE             /* 消息体的长度 */

/* RS422 I Master的从机地址定义 */
typedef enum {
    WX_RS422I_MASTER_SLAVE_ADDR_NA,
    WX_RS422I_MASTER_SLAVE_ADDR_XXX,
    WX_RS422I_MASTER_SLAVE_ADDR_YYY,
    WX_RS422I_MASTER_SLAVE_ADDR_BUTT,
} WxRs422IMasterSlaveAddrDef;

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
    WxEvtMsg header; /* 消息头 */
    UINT8 msgData[WX_RS422I_MASTER_MSG_BODY_SIZE];  /* Msg body 由msgType和msgSubType确定，由通信双方自行约束 */
} WxRs422IMasterMsg;

#endif
