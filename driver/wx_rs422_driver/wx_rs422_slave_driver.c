#include "wx_include.h"
#include "wx_rs422_slave_driver.h"
#include "wx_id_def.h"
#include "wx_rs422_slave_driver_tx_adu_req_intf.h"
#include "wx_rs422_slave_rx_adu_req_intf.h"
#include "wx_uart_ns550.h"
#include "wx_msg_res_pool.h"
 #include "wx_frame.h"
WxRs422SlaveDriverCfg g_rs422SlaverDriverCfg = {0};

VOID WX_RS422SlaveDriver_SentRxAdu2Upper(WxRs422SlaverDriver *this, WxModbusAdu *rxAdu)
{
    /* 鐢宠娑堟伅 */
	WxRs422SlaveRxAduReq *msg = WX_ApplyEvtMsg(WX_MSG_TYPE_RS422_SLAVE_RX_ADU_REQ);
    if (msg == NULL) {
        return;
    }

    /* 鍒濆鍖栨秷鎭ご */
    WX_CLEAR_OBJ((WxMsg *)msg);

    /* 濉啓娑堟伅淇℃伅 */
    msg->sender = this->moduleId;
    msg->receiver = WX_MODULE_RS422_SLAVE;
    msg->msgType = WX_MSG_TYPE_RS422_SLAVE_RX_ADU_REQ;
    msg->rxAdu.failCode = WX_SUCCESS;
    for (UINT8 i = 0; i < rxAdu->valueLen; i++) {
        msg->rxAdu.value[i] = rxAdu->value[i];
    }
    msg->rxAdu.valueLen = rxAdu->valueLen;

    /* 鍙戦�佹秷鎭� */
    UINT32 ret = WX_MsgShedule(this->moduleId, msg->receiver, msg);
    if (ret != WX_SUCCESS) {
        WX_FreeEvtMsg((WxMsg **)&msg);
    }
}

/* 鏄惁鏄敮鎸佺殑鍔熻兘鐮� */
BOOL WX_RS422SlaveDriver_IsSupportFuncCode(WxRs422SlaverDriver *this, UINT8 funcCode)
{
    return (funcCode == WX_MODBUS_FUNC_CODE_READ_DATA ||
            funcCode == WX_MODBUS_FUNC_CODE_WRITE_DATA ||
            funcCode == WX_MODBUS_FUNC_CODE_READ_FILE ||
            funcCode == WX_MODBUS_FUNC_CODE_WRITE_FILE);

}
/* 澶勭悊鎺ユ敹鍒扮殑PDU */
VOID WX_RS422SlaveDriver_ProcRecvAduFromISR(WxRs422SlaverDriver *this)
{
    /* 妫�鏌ユ帴鏀跺埌鐨勬姤鏂� */
    if (this->rxAdu.valueLen < WX_MODBUS_ADU_MIN_SIZE) {
        wx_excp_cnt(WX_EXCP_RS422_SLAVE_RECV_DATA_LEN_ERR);
        return;
    }
    /* 妫�鏌ュ湴鍧� */
    UINT8 slaveAddr = this->rxAdu.value[WX_MODBUS_SLAVE_ADDR_IDX];
    if (slaveAddr != this->slaveAddr) {
        wx_excp_cnt(WX_EXCP_RS422_SLAVE_RECV_DATA_ADDR_ERR);
        return;
    }
    /* 妫�鏌RC */
    if (WX_Modbus_AduCrcCheck(&this->rxAdu) != WX_SUCCESS) {
        wx_excp_cnt(WX_EXCP_RS422_SLAVE_RECV_DATA_CRC_ERR);
        return;
    }
    UINT8 funcCode = this->rxAdu.value[WX_MODBUS_FUNC_CODE_IDX];
    if (!WX_RS422SlaveDriver_IsSupportFuncCode(this, this->rxAdu.funcCode)) {
        wx_excp_cnt(WX_EXCP_RS422_SLAVE_RECV_DATA_FUNC_CODE_ERR);
        /* 鍙戦�佸紓甯稿搷搴旀姤鏂� */
        WX_Modbus_AduGenerateExceptionRsp(slaveAddr, funcCode,
        	WX_MODBUS_EXCP_ILLEGAL_FUNCTION, &this->txAdu);
        /* 棣栨鍙戦�佹秷鎭細琚紦瀛樺埌瀹炰緥锛岃繑鍥炵紦瀛樹簡澶氬皯鎶ユ枃 */
        UINT32 sendCount = XUartNs550_Send(&this->rs422Inst, this->txAdu.value,
        	(unsigned int)this->txAdu.valueLen);
        if (sendCount == 0) {
            wx_excp_cnt(WX_EXCP_RS422_SLAVE_SEND_DATA_FAIL);
        }
        return;
    }

    /* 鎶ユ枃鍙戦�佺粰涓婂眰澶勭悊 */
    WX_RS422SlaveDriver_SentRxAdu2Upper(this, &this->rxAdu);
    return;
}


/* RS422I涓柇澶勭悊鍑芥暟 */
VOID WX_RS422SlaveDriver_IntrHandle(VOID *callBackRef, UINT32 event, UINT32 eventData)
{
    WxRs422SlaverDriver *this = callBackRef;
    /* 浠庢満鏁版嵁鍙戦�佸畬姣曡浆鎹负鎺ユ敹 */
	if (event == XUN_EVENT_SENT_DATA) {
        UINT32 recvCount = XUartNs550_Recv(&this->rs422Inst, this->rxAdu.value, WX_MODBUS_ADU_MAX_SIZE);
        if (recvCount == 0) {
            wx_excp_cnt(WX_EXCP_RS422_SLAVE_RECV_DATA_TX_FINISH);
        }
	}

    /* All of the data has been received. or Data was received, but not the expected number of bytes */
	if (event == XUN_EVENT_RECV_DATA || event ==XUN_EVENT_RECV_TIMEOUT) {
        this->rxAdu.valueLen = eventData;
        WX_RS422SlaveDriver_ProcRecvAduFromISR(this);
	}
}

UINT32 WX_RS422SlaveDriver_Construct(VOID *module)
{
    UINT32 ret;
    /* allocate memory for module */
    WxRs422SlaverDriver *this = WX_Mem_Alloc(WX_GetModuleName(module), 1, sizeof(WxRs422SlaverDriver));
    if (this == NULL) {
        return WX_RS422_SLAVE_DRIVER_MEM_ALLOC_FAIL;
    }
    WxRs422SlaveDriverCfg *cfg = &g_rs422SlaverDriverCfg;
    /* the inst or rs422 used for uart data tx/rx */
    ret = WX_InitUartNs550(&this->rs422Inst, cfg->rs422DevId, &cfg->rs422Format);
    if (ret != WX_SUCCESS) {
        return ret;
    }

    /* 璁剧疆涓柇 */
    ret = WX_SetupUartNs550Interrupt(&this->rs422Inst, WX_RS422SlaveDriver_IntrHandle, cfg->intrId, this);
    if (ret != WX_SUCCESS) {
        return ret;
    }

    /* 鎺ユ敹鍑嗗 */
    UINT32 recvCount; 
    do {
        recvCount = XUartNs550_Recv(&this->rs422Inst, this->rxAdu.value, WX_MODBUS_ADU_MAX_SIZE);
    } while (recvCount);

    /* 璁剧疆涓奙odule */
    WX_SetModuleInfo(module, this);
    return WX_SUCCESS;
}

/* 澶勭悊鍙戦�佹姤鏂囪姹� */
UINT32 WX_RS422SlaveDriver_ProcTxAduReq(WxRs422SlaverDriver *this, WxRs422SlaverDriverTxAduReq *msg)
{
    /* 棣栨鍙戦�佹秷鎭細琚紦瀛樺埌瀹炰緥锛岃繑鍥炵紦瀛樹簡澶氬皯鎶ユ枃 */
    UINT32 sendCount = XUartNs550_Send(&this->rs422Inst, this->txAdu.value, this->txAdu.valueLen);
    if (sendCount == 0) {
        wx_excp_cnt(WX_EXCP_RS422_SLAVE_SEND_DATA_FAIL_0);
    }

    return WX_SUCCESS;
}

/* 娑堟伅澶勭悊鍏ュ彛鍑芥暟 */
UINT32 WX_RS422SlaveDriver_Entry(VOID *module, WxMsg *evtMsg)
{
    WxRs422SlaverDriver *this = WX_GetModuleInfo(module);
    switch (evtMsg->msgType) {
        case WX_MSG_TYPE_RS422_SLAVE_TX_ADU_REQ: {
            return WX_RS422SlaveDriver_ProcTxAduReq(this, (WxRs422SlaverDriverTxAduReq *)evtMsg);
        }
        default: {
            wx_excp_cnt(WX_EXCP_RS422_SLAVE_UNSPT_MSG_TYPE);
            return WX_RS422_SLAVE_DRIVER_UNSPT_MSG_TYPE;
        } 
    }
}

UINT32 WX_RS422SlaveDriver_Destruct(VOID *module)
{
    WxRs422SlaverDriver *this = WX_GetModuleInfo(module);
    if (this == NULL) {
        return WX_SUCCESS;
    }
    WX_Mem_Free(this);
    WX_SetModuleInfo(module, NULL);
    return WX_SUCCESS;
}
