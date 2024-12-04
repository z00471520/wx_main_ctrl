#ifndef WX_MODBUS_H
#define WX_MODBUS_H
#include "wx_typedef.h"
#include <stdint.h>
/* modus field length define */
#define WX_MODBUS_CRC_LEN                   2       /* CRC value is 2 byte */
#define WX_MODBUS_SLAVE_ADDR_LEN            1       /* Slave address is 1 byte  */
#define WX_MODBUS_FUNC_CODE_LEN             1       /* Function-code is 1 byte see mobus function code define for detail  */
#define WX_MODBUS_EXCP_CODE_LEN             1       /* Excption-code is 1 byte */
#define WX_MODBUS_DATA_ADDR_LEN             2       /* Data Address is 2 byte */
#define WX_MODBUS_DATA_SIZE_LEN             1       /* data size is 1 byte  */
#define WX_MODBUS_INVALID_SLAVE_ADDR        0xff    /* 0xFF涓嶈兘浣滀负浠庢満鍦板潃浣跨敤 */

#define WX_MODBUS_SLAVE_ADDR_IDX            0
#define WX_MODBUS_FUNC_CODE_IDX       1
/* mobus function code define */
#define WX_MODBUS_FUNC_CODE_READ_DATA       0x41    /* 璇绘暟鎹� */
#define WX_MODBUS_FUNC_CODE_WRITE_DATA      0x42    /* 鍐欐暟鎹� */
#define WX_MODBUS_FUNC_CODE_READ_FILE       0x64    /* 璇绘枃浠� */
#define WX_MODBUS_FUNC_CODE_WRITE_FILE      0x65    /* 鍐欐枃浠� */
#define WX_MODBUS_FUNC_CODE_READ_DATA_ERR    (WX_MODBUS_FUNC_CODE_READ_DATA + 0x80)    /* 璇绘暟鎹� */
#define WX_MODBUS_FUNC_CODE_WRITE_DATA_ERR   (WX_MODBUS_FUNC_CODE_WRITE_DATA + 0x80)    /* 鍐欐暟鎹� */
#define WX_MODBUS_FUNC_CODE_READ_FILE_ERR    (WX_MODBUS_FUNC_CODE_READ_FILE + 0x80)    /* 璇绘枃浠� */
#define WX_MODBUS_FUNC_CODE_WRITE_FILE_ERR   (WX_MODBUS_FUNC_CODE_WRITE_FILE + 0x80)    /* 鍐欐枃浠� */
#define WX_MODBUS_IS_EXCP_FUNC_CODE(funcCode) (((funcCode) & 0x80) != 0)

/* Modus ADU */
#define WX_MODBUS_ADU_MAX_SIZE              256     /* ADU鐨勬渶澶ч暱搴� */
#define WX_MODBUS_ADU_MIN_SIZE              (4)     /* 浠庢満鍦板潃(1) + function code(1) + crc(2) */
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
#define WX_MODBUS_ADU_WR_REQ_DATA_ADDR_HI_IDX   2   /* oct2 is the data address hi */
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


#define WX_MODBUS_MAX_EXCP_CODE_NUM         256 /* 寮傚父鐮乁8鏈�澶氭敮鎸�256涓暟 */

/* 瀹氫箟浠庢満寮傚父鐮佺殑鏋氫妇鍊� */
typedef enum {
    WX_MODBUS_EXCP_NONE = 0,
    WX_MODBUS_EXCP_ILLEGAL_FUNCTION = 1,
    WX_MODBUS_EXCP_ILLEGAL_DATA_ADDRESS = 2,
    WX_MODBUS_EXCP_ILLEGAL_DATA_VALUE = 3,
    WX_MODBUS_EXCP_RD_ADDR_NOT_DEFINED = 4,
    WX_MODBUS_EXCP_RD_DATA_FAIL = 5,
    WX_MODBUS_EXCP_ENC_RD_DATA_FAIL = 6,
    WX_MODBUS_EXCP_WR_HANDLE_UNFINE = 7,
    WX_MODBUS_EXCP_WR_DATA_DEC_FAIL = 8,
    WX_MODBUS_EXCP_WR_DATA_FAIL = 9,
    WX_MODBUS_EXCP_MAX_EXCP_CODE_NUM = 256
} WxModbusExcpCode;

typedef struct {
    UINT16 valueLen;      /* adu value length  */
    UINT16 expectRspLen;  /* expect response length when master send request */
    UINT16 dataAddr;      /* data address */
    UINT8 funcCode;       /* function code */
    UINT8 slaveAddr;      /* slave address */
    UINT8 excpCode;       /* exception code */
    UINT8 sender;         /* origin sender */
    UINT8 value[WX_MODBUS_ADU_MAX_SIZE]; /* the adu value array */
} WxModbusAdu;

/* 璁＄畻CRC鐨勫彇鍊� */
uint16_t WX_Modbus_Crc16(uint8_t *pFrame, uint16_t len);
UINT32 WX_Modbus_AduCrcCheck(WxModbusAdu *adu);
VOID WX_Modbus_AduGenerateExceptionRsp(UINT8 slaveAddr, UINT8 functionCode, UINT8 exceptionCode, WxModbusAdu *adu);
UINT16 WX_Modbug_GetAduCrcValue(WxModbusAdu *adu);
UINT32 WX_Modbus_AduEncodeBasic(WxModbusAdu *adu, intptr_t value, UINT32 valueSize);
UINT32 WX_Modbus_AduEncodeBlock(WxModbusAdu *adu, UINT8 *block, UINT32 blockSize);
UINT32 WX_Modbus_AduDecodeBasic(WxModbusAdu *adu, UINT32 *startPtr, UINT32 len, VOID *buff);
UINT32 WX_Modbus_AduDecodeBlock(WxModbusAdu *adu, UINT32 *startPtr, UINT32 len, UINT8 *buff, UINT32 buffSize);
#define WX_MODBUS_ADU_ENCODE_BASIC(adu, v) WX_Modbus_AduEncodeBasic(adu, (intptr_t)v, sizeof(v))
#define WX_MODBUS_ADU_DECODE_BASIC(adu, sp, vp) WX_Modbus_AduDecodeBasic(adu, sp, sizeof(*vp), (VOID *)vp)
#endif
