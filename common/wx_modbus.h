#ifndef WX_MODBUS_H
#define WX_MODBUS_H
#include "wx_modbus_crc16.h"
#include "wx_typedef.h"
#include <stdint.h>
/* modus field length define */
#define WX_MODBUS_CRC_LEN                   2       /* CRC value is 2 byte */
#define WX_MODBUS_SLAVE_ADDR_LEN            1       /* Slave address is 1 byte  */
#define WX_MODBUS_FUNC_CODE_LEN             1       /* Function-code is 1 byte see mobus function code define for detail  */
#define WX_MODBUS_EXCP_CODE_LEN             1       /* Excption-code is 1 byte */
#define WX_MODBUS_DATA_ADDR_LEN             2       /* Data Address is 2 byte */
#define WX_MODBUS_DATA_SIZE_LEN             1       /* data size is 1 byte  */
#define WX_MODBUS_INVALID_SLAVE_ADDR        0xff    /* 0xFF不能作为从机地址使用 */

#define WX_MODBUS_SLAVE_ADDR_IDX            0
#define WX_MODBUS_SLAVE_FUNC_CODE_IDX       1
/* mobus function code define */
#define WX_MODBUS_FUNC_CODE_READ_DATA       0x41    /* 读数据 */
#define WX_MODBUS_FUNC_CODE_WRITE_DATA      0x42    /* 写数据 */
#define WX_MODBUS_FUNC_CODE_READ_FILE       0x64    /* 读文件 */
#define WX_MODBUS_FUNC_CODE_WRITE_FILE      0x65    /* 写文件 */
#define WX_MODBUS_FUNC_CODE_READ_DATA_ERR    (WX_MODBUS_FUNC_CODE_READ_DATA + 0x80)    /* 读数据 */
#define WX_MODBUS_FUNC_CODE_WRITE_DATA_ERR   (WX_MODBUS_FUNC_CODE_WRITE_DATA + 0x80)    /* 写数据 */
#define WX_MODBUS_FUNC_CODE_READ_FILE_ERR    (WX_MODBUS_FUNC_CODE_READ_FILE + 0x80)    /* 读文件 */
#define WX_MODBUS_FUNC_CODE_WRITE_FILE_ERR   (WX_MODBUS_FUNC_CODE_WRITE_FILE + 0x80)    /* 写文件 */
#define WX_MODBUS_IS_EXCP_FUNC_CODE(funcCode) (((funcCode) & 0x80) != 0)

/* Modus ADU */
#define WX_MODBUS_ADU_MAX_SIZE              256     /* ADU的最大长度 */
#define WX_MODBUS_ADU_MIN_SIZE              (4)     /* 从机地址(1) + function code(1) + crc(2) */ 
#define WX_MODBUS_ADU_SLAVE_ADDR_IDX        0
#define WX_MODBUS_ADU_FUNC_CODE_IDX         1         

/* Read data request format */
#define WX_MODBUS_ADU_RD_REQ_LEN                (7)
#define WX_MODBUS_ADU_RD_REQ_SLAVE_ADDR_IDX     0   /* oct0 is slave address */
#define WX_MODBUS_ADU_RD_REQ_FUNC_CODE_IDX      1   /* oct1 is the function code */
#define WX_MODBUS_ADU_RD_REQ_DATA_ADDR_HI_IDX   2   /* oct2 is the data address hi */
#define WX_MODBUS_ADU_RD_REQ_DATA_ADDR_LO_IDX   3   /* oct3 is the data address lo */
#define WX_MODBUS_ADU_RD_REQ_DATA_LEN_IDX       4   /* oct4 is the data expect len */
#define WX_MODBUS_ADU_RD_REQ_CRC_HI_IDX         5   /* oct5 is the CRC hi */
#define WX_MODBUS_ADU_RD_REQ_CRC_LO_IDX         6   /* oct6 is the CRC hi */

/* Read data response format */
#define WX_MODBUS_ADU_RD_RSP_MIN_LEN            (6) /* the minimum length */
#define WX_MODBUS_ADU_RD_RSP_SLAVE_ADDR_IDX     0   /* oct0 is slave address */
#define WX_MODBUS_ADU_RD_RSP_FUNC_CODE_IDX      1   /* oct1 is the function code */
#define WX_MODBUS_ADU_RD_RSP_DATA_LEN_IDX       2   /* oct2 is the data length */
#define WX_MODBUS_ADU_RD_RSP_DATA_START_IDX     3   /* oct3 is the data start position */

/* Write data request format*/
#define WX_MODBUS_ADU_WR_REQ_MIN_LEN            (8) /* the minimum length */
#define WX_MODBUS_ADU_WR_REQ_SLAVE_ADDR_IDX     0   /* oct0 is the slave address */
#define WX_MODBUS_ADU_WR_REQ_FUNC_CODE_IDX      1   /* oct1 is the function code */
#define WX_MODBUS_ADU_WR_REQ_DATA_ADDR_HI_IDX   1   /* oct2 is the data address hi */
#define WX_MODBUS_ADU_WR_REQ_DATA_ADDR_LO_IDX   3   /* oct2 is the data address lo */
#define WX_MODBUS_ADU_WR_REQ_DATA_LEN_IDX       4   /* oct2 is the data length */
#define WX_MODBUG_ADU_WR_REQ_DATA_START_IDX     5   /* oct5 is data start position */

/* Error ADU format */
#define WX_MODBUS_ADU_ERR_RSP_LEN               (5) // Error ADU is 5Byte len
#define WX_MODBUS_ADU_ERR_RSP_SLAVE_ADDR_IDX    0   // oct0 is slave address
#define WX_MODBUS_ADU_ERR_RSP_FUNC_CODE_IDX     1   // oct1 is the function code
#define WX_MODBUS_ADU_ERR_RSP_EXPC_CODE_IDX     2   // oct2 is the excption code
#define WX_MODBUS_ADU_ERR_RSP_CRC_HI_IDX        3   // oct3 is the CRC hi
#define WX_MODBUS_ADU_ERR_RSP_CRC_LO_IDX        4   // oct4 is the CRC lo


#define WX_MODBUS_MAX_EXCP_CODE_NUM         256 /* 异常码U8最多支持256个数 */

typedef struct {
    UINT16 valueLen;
    UINT8 value[WX_MODBUS_ADU_MAX_SIZE]; /* 数组 */
} WxModbusAdu;

/* 计算CRC的取值 */
uint16_t WX_Modbus_Crc16(uint8_t *pFrame, uint16_t len);
WxFailCode WX_Modbus_AduCrcCheck(WxModbusAdu *adu);
UINT16 WX_Modbug_GetAduCrcValue(WxModbusAdu *adu);
WxFailCode WX_Modbus_AduEncodeBasic(WxModbusAdu *adu, const VOID* const valuePtr, UINT32 valueSize);
WxFailCode WX_Modbus_AduEncodeBlock(WxModbusAdu *adu, UINT8 block, UINT32 blockSize);
WxFailCode WX_Modbus_AduDecodeBasic(WxModbusAdu *adu, UINT32 *startPtr, UINT32 len, VOID *buff);
WxFailCode WX_Modbus_AduDecodeBlock(WxModbusAdu *adu, UINT32 *startPtr, UINT32 len, UINT8 *buff, UINT32 buffSize);
#define WX_MODBUS_ADU_ENCODE_BASIC(adu, valuePtr) WX_Modbus_AduEncodeBasic(adu, (const VOID* const)valuePtr, sizeof(*valuePtr))
#define WX_MODBUS_ADU_DECODE_BASIC(adu, startPtr, valuePtr) WX_Modbus_AduDecodeBasic(adu, startPtr, sizeof(*valuePtr), (VOID *)valuePtr)
#endif