#ifndef WX_MODBUS_H
#define WX_MODBUS_H
#include "wx_modbus_crc16.h"
#include "wx_typedef.h"
#include <stdint.h>
/* modus field length define */
#define WX_MODBUS_CRC_LEN                   2       /** CRC value is 2 byte */
#define WX_MODBUS_SLAVE_ADDR_LEN            1       /** Slave address is 1 byte  */
#define WX_MODBUS_FUNC_CODE_LEN             1       /** Function-code is 1 byte see mobus function code define for detail  */
#define WX_MODBUS_EXCP_CODE_LEN             1       /** Excption-code is 1 byte */
#define WX_MODBUS_DATA_ADDR_LEN             2       /** Data Address is 2 byte */
#define WX_MODBUS_DATA_SIZE_LEN             1       /** data size is 1 byte  */

/* mobus function code define */
#define WX_MODBUS_FUNC_CODE_READ_DATA       0x41    /* 读数据 */
#define WX_MODBUS_FUNC_CODE_WRITE_DATA      0x42    /* 写数据 */
#define WX_MODBUS_FUNC_CODE_READ_FILE       0x64    /* 读文件 */
#define WX_MODBUS_FUNC_CODE_WRITE_FILE      0x65    /* 写文件 */
#define WX_MODBUS_FUNC_CODE_READ_DATA_ERR    (WX_MODBUS_FUNC_CODE_READ_DATA + 0x80)    /* 读数据 */
#define WX_MODBUS_FUNC_CODE_WRITE_DATA_ERR   (WX_MODBUS_FUNC_CODE_WRITE_DATA + 0x80)    /* 写数据 */
#define WX_MODBUS_FUNC_CODE_READ_FILE_ERR    (WX_MODBUS_FUNC_CODE_READ_FILE + 0x80)    /* 读文件 */
#define WX_MODBUS_FUNC_CODE_WRITE_FILE_ERR   (WX_MODBUS_FUNC_CODE_WRITE_FILE + 0x80)    /* 写文件 */

/* Read data response format */
#define WX_MODBUS_RD_RSP_MIN_LEN            (6)
#define WX_MODBUS_RD_RSP_SLAVE_ADDR_IDX     0   /* oct0 is slave address */
#define WX_MODBUS_RD_RSP_FUNC_CODE_IDX      1   /* oct1 is the function code */
#define WX_MODBUS_RD_RSP_DATA_LEN           2   /* oct2 is the data length */
#define WX_MODBUS_RD_RSP_DATA               3   /* oct3 is the data start position */

/* Error ADU format */
#define WX_MODBUS_ERR_ADU_LEN               (5) // Error ADU is 5Byte len
#define WX_MODBUS_ERR_ADU_SLAVE_ADDR_IDX    0   // oct0 is slave address
#define WX_MODBUS_ERR_ADU_FUNC_CODE_IDX     1   // oct1 is the function code
#define WX_MODBUS_ERR_ADU_EXPC_CODE_IDX     2   // oct2 is the excption code
#define WX_MODBUS_ERR_ADU_CRC_HI_IDX        3   // oct3 is the CRC hi
#define WX_MODBUS_ERR_ADU_CRC_LO_IDX        4   // oct4 is the CRC lo

#define WX_MODBUS_ADU_MAX_SIZE              256
#define WX_MODBUS_MAX_EXCP_CODE_NUM         256
#define WX_MODBUS_CRC_VALUE(hi, lo)         (((UINT16)(hi) << 8) + (lo)) /* 用于拼接CRC U8的(HI, LO)到一个U16 */
uint16_t WX_Modbus_Crc16(uint8_t *pFrame, uint16_t len);
#endif