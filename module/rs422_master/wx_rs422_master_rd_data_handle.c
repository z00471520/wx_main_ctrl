#include "wx_rs422_master.h"
#include "wx_include.h"
#include "wx_rs422_master_adu_rsp_intf.h"
UINT32 WX_RS422_MASTER_DecRdDataX(UINT8 *buff, UINT8 buffSize, WxRs422MasterReadData *rspStruct)
{
    return WX_NOT_SUPPORT;
}


UINT32 WX_RS422_MASTER_DecRdDataY(UINT8 *buff, UINT8 buffSize, WxRs422MasterReadData *rspStruct)
{
    return WX_NOT_SUPPORT;
}

/* 涓嶅悓璇绘搷浣滅殑缂栫爜淇℃伅, 鏂逛究娣诲姞鎴戞妸鎵�鏈夌殑閮芥斁鍒拌繖閲屼簡 */
WxRs422MasterRdDataHandle g_wxRs422MasterReadDataHandles[WX_RS422_MASTER_MSG_READ_DATA_BUTT] = {
                                     /* 鎻愪緵鏁版嵁鐨勪粠鏈哄湴鍧�, 璇绘暟鎹湴鍧�U16锛� 璇绘暟鎹殑闀垮害锛圲8锛夛紝璇诲埌鏁版嵁鐨勮В鐮佸嚱鏁� */
    [WX_RS422_MASTER_MSG_READ_DATA_XXX] = {WX_RS422_MASTER_SLAVE_ADDR_XXX, 0, sizeof(WxRs422MasterRdDataX), WX_RS422_MASTER_DecRdDataX},
    [WX_RS422_MASTER_MSG_READ_DATA_YYY] = {WX_RS422_MASTER_SLAVE_ADDR_YYY, 0, sizeof(WxRs422MasterRdDataY), WX_RS422_MASTER_DecRdDataY},
};

UINT32 WX_RS422_MASTER_DecRdDataRsp(WxRs422MasterDriverRspData *rspData, WxRs422MasterReadData *readData)
{
    WxModbusAdu *rspAdu = &rspData->rspAdu;
    if (rspAdu->valueLen < WX_MODBUS_ADU_RD_RSP_MIN_LEN) {
        return WX_RS422_MASTER_RECV_RSP_RD_VALUE_LEN_ERR;
    }
    /* 鏁版嵁鐨勯暱搴︽槸鍚︽�婚暱搴﹂噸璇� 3锛堜粠鏈哄湴鍧�锛屽姛鑳界爜锛� 鏁版嵁闀垮害锛� 2锛圕RC鏍￠獙锛� */
    UINT8 dataLen = rspAdu->value[WX_MODBUS_ADU_RD_RSP_DATA_LEN_IDX];
    if (dataLen + 3 + 2 != rspAdu->valueLen) {
        return WX_RS422_MASTER_RECV_RSP_RD_VALUE_LEN_ERR;
    }

    /* 鎸囧悜鏁版嵁鍖� */
    UINT8 *data = &rspAdu->value[WX_MODBUS_ADU_RD_RSP_DATA_START_IDX];
    /* 鑾峰彇璇绘暟鎹被鍨嬪搴旂殑瑙ｇ爜鍑芥暟 */
    WxRs422MasterRdDateDecFunc decFunc = g_wxRs422MasterReadDataHandles[rspData->reqSubMsgType].decFunc;
    if (decFunc == NULL) {
        return WX_RS422_MASTER_RECV_RSP_RD_GET_DEC_FUNC_FAIL;
    }
    
    /* 璋冪敤瑙ｇ爜鍑芥暟瑙ｇ爜锛屾妸鏁版嵁娴佽В鐮佸埌缁撴瀯浣�  */
    return decFunc(data, dataLen, readData);
}
