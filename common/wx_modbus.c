#include <stdint.h>
#include "wx_include.h"
#include "wx_modbus.h"

static const uint8_t aucCRCHi[] = {
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40
};

static const uint8_t aucCRCLo[] = {
    0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7,
    0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E,
    0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09, 0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9,
    0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC,
    0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
    0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32,
    0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D,
    0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A, 0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 
    0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF,
    0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
    0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1,
    0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4,
    0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F, 0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 
    0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA,
    0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
    0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0,
    0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97,
    0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C, 0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E,
    0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89,
    0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
    0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83,
    0x41, 0x81, 0x80, 0x40
};

uint16_t WX_Modbus_Crc16( uint8_t *pFrame, uint16_t len )
{
    uint8_t           ucCRCHi = 0xFF;
    uint8_t           ucCRCLo = 0xFF;
    int             iIndex;

    while( len-- )
    {
        iIndex = ucCRCLo ^ *( pFrame++ );
        ucCRCLo = ( uint8_t )( ucCRCHi ^ aucCRCHi[iIndex] );
        ucCRCHi = aucCRCLo[iIndex];
    }
    return ( uint16_t )( ucCRCHi << 8 | ucCRCLo );
}


/* 获取ADU的报文CRC校验结果 */
UINT16 WX_Modbug_GetAduCrcValue(WxModbusAdu *adu)
{
    if (adu->valueLen < WX_MODBUS_ADU_MIN_SIZE) {
        return 0;
    }
    UINT16 lo = (UINT16)adu->value[adu->valueLen - 1];
    UINT16 hi = (UINT16)adu->value[adu->valueLen - 2];
    return ((hi << 8) + lo);
}

/* WX_SUCCESS-校验成功; 其他-校验失败 */
UINT32 WX_Modbus_AduCrcCheck(WxModbusAdu *adu)
{
    if (adu->valueLen < WX_MODBUS_ADU_MIN_SIZE) {
        return WX_ERR;
    }
    UINT16 expectCrc16 = WX_Modbus_Crc16(adu->value, adu->valueLen - WX_MODBUS_CRC_LEN);
    UINT16 realCrc16 = WX_Modbug_GetAduCrcValue(adu);
    return (realCrc16 == expectCrc16) ? WX_SUCCESS : WX_MODBUS_ADU_CRC_CHECK_FAIL;
}

/* 
 * 函数功能：编码基础类型的数据数据到ADU
 * 参数说明：
 * adu       - 待编码的ADU
 * value     - 要编码的数据
 * valueSize - 数据的长度 取值1,2,4,8
 * 返回值说明：
 * WX_SUCCESS - 编码成功
 * 其他        - 编码失败
 **/
UINT32 WX_Modbus_AduEncodeBasic(WxModbusAdu *adu, intptr_t value, UINT32 valueSize)
{
    /* 编码空间不足 */
    if (adu->valueLen + valueSize > sizeof(adu->value)) {
        return WX_MODBUS_ADU_ENCODE_INSUFFICIENT_SPACE;
    }

    switch (valueSize)
    {
        case 1: {
            UINT8 value8 = (UINT8)value;
            adu->value[adu->valueLen++] = value8;
            break;
        }
        case 2: {
            UINT16 value16 = (UINT16)value;
            adu->value[adu->valueLen++] = (UINT8)((value16 >> 8) & 0xFF);
            adu->value[adu->valueLen++] = (UINT8)(value16 & 0xFF);
            break;
        }
        case 4: {
            UINT32 value32 = (UINT32)value;
            adu->value[adu->valueLen++] = (UINT8)((value32 >> 24) & 0xFF);
            adu->value[adu->valueLen++] = (UINT8)((value32 >> 16) & 0xFF);
            adu->value[adu->valueLen++] = (UINT8)((value32 >> 8)  & 0xFF);
            adu->value[adu->valueLen++] = (UINT8)(value32 & 0xFF);
            break;
        }
        case 8: {
            UINT64 value64 =(UINT64)value;
            adu->value[adu->valueLen++] = (UINT8)((value64 >> 56) & 0xFF);
            adu->value[adu->valueLen++] = (UINT8)((value64 >> 48) & 0xFF);
            adu->value[adu->valueLen++] = (UINT8)((value64 >> 40) & 0xFF);
            adu->value[adu->valueLen++] = (UINT8)((value64 >> 32) & 0xFF);
            adu->value[adu->valueLen++] = (UINT8)((value64 >> 24) & 0xFF);
            adu->value[adu->valueLen++] = (UINT8)((value64 >> 16) & 0xFF);
            adu->value[adu->valueLen++] = (UINT8)((value64 >> 8)  & 0xFF);
            adu->value[adu->valueLen++] = (UINT8)(value64 & 0xFF);
            break;
        }
        default: {
            return WX_MODBUS_ADU_ENCODE_INVALID_VALUE_SIZE;
        }
    }

    return WX_SUCCESS;
}

/* 
 * 函数功能：编码数据块到ADU
 * 参数说明：
 * adu       - 待编码的ADU
 * block  - 数据块的地址
 * blockSize - 数据块的长度
 * 返回值说明：
 * WX_SUCCESS - 编码成功
 * 其他        - 编码失败
 **/
UINT32 WX_Modbus_AduEncodeBlock(WxModbusAdu *adu, UINT8 block, UINT32 blockSize)
{
    /* 编码空间不足 */
    if (adu->valueLen + blockSize > sizeof(adu->value)) {
        return WX_MODBUS_ADU_ENCODE_INSUFFICIENT_SPACE;
    }

    for (UINT32 i = 0; i < blockSize; i++) {
         adu->value[adu->valueLen++] = block[i];
    }

    return WX_SUCCESS;
}

/*
 * 函数功能: 解码基础类型
 **/
UINT32 WX_Modbus_AduDecodeBasic(WxModbusAdu *adu, UINT32 *startPtr, UINT32 len, VOID *buff)
{
    /* 入参检查 */
    UINT32 start = *startPtr;
    if (start + len > adu->valueLen) {
        return WX_MODBUS_ADU_DECODE_INVALID_START_LEN;
    }

    switch (len)
    {
        case 1: {
            *((UINT8 *)buff) = adu->value[start];
            break;
        }
        case 2: {
            UINT16 value16 = 0;
            value16 |= adu->value[start];
            value16 <<= 8;
            value16 |= adu->value[start + 1];
            value16 <<= 8;
            *((UINT16 *)buff) = value16;
            break;
        }
        case 4: {
            UINT32 value32 = 0;
            value32 |= adu->value[start];
            value32 <<= 8;
            value32 |= adu->value[start + 1];
            value32 <<= 8;
            value32 |= adu->value[start + 2];
            value32 <<= 8;
            value32 |= adu->value[start + 3];
            value32 <<= 8;
            *((UINT32 *)buff) = value32;
            break;
        }
        case 8: {
            UINT64 value64 = 0;
            value64 |= adu->value[start];
            value64 <<= 8;
            value64 |= adu->value[start + 1];
            value64 <<= 8;
            value64 |= adu->value[start + 2];
            value64 <<= 8;
            value64 |= adu->value[start + 3];
            value64 <<= 8;
            value64 |= adu->value[start + 4];
            value64 <<= 8;
            value64 |= adu->value[start + 5];
            value64 <<= 8;
            value64 |= adu->value[start + 6];
            value64 <<= 8;
            value64 |= adu->value[start + 7];
            value64 <<= 8;
            *((UINT64 *)buff) = value64;
            break;
        }
        default: {
            return WX_MODBUS_ADU_DECODE_INVALID_LEN;
        }
    }
    *startPtr += len; /* 解码完毕后进行偏移 */
    return WX_SUCCESS;
}



/*
 * 函数功能：解码ADU的指定位置和长度的数据块到buf, 解码完毕后数据的startPrt取值为下一个待解码位置
 **/
UINT32 WX_Modbus_AduDecodeBlock(WxModbusAdu *adu, UINT32 *startPtr, UINT32 len, UINT8 *buff, UINT32 buffSize)
{
    /* 入参检查 */
    UINT32 start = *startPtr;
    if (start + len > adu->valueLen) {
        return WX_MODBUS_ADU_DECODE_INVALID_START_LEN;
    }

    if (len > buffSize) {
        return WX_MODBUS_ADU_DECODE_BUF_SIZE_ERR;
    }
    memcpy(buff, adu->value[start], len);
    *startPtr += len;

    return WX_SUCCESS;
}
