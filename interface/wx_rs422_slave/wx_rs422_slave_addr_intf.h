#ifndef __WX_RS422_SLAVE_ADDR_INTF_H__
#define __WX_RS422_SLAVE_ADDR_INTF_H__

#define WX_RS422_SLAVE_ADDR 10 /* 从机地址 */
/******************************************************************************
 * 从机地址定义（先假设读写地址是一套）
 *****************************************************************************/
typedef enum {
    WX_RS422_SLAVE_DATA_ADDR_X = 1,
    /* if more please add here */
    WX_RS422_SLAVE_DATA_ADDR_BUTT,
} WxRs422SlaveDataAddr;

/******************************************************************************
 * 数据格式定义
 *****************************************************************************/
typedef struct {

} WxRs422SlaveDataX;

/* 读写的数据 */
typedef struct {
    /* 联合体由数据地址确定 */
    union {
        WxRs422SlaveDataX dataX;
        /* if more please add here */
    };
} WxRs422SlaveData;

#endif
