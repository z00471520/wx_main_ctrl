#include "wx_include.h"
#include "wx_rs422_driver_master.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "wx_msg_common.h"
#include "wx_rs422_master_driver_intf.h"
#include "xuartns550.h"
#include "wx_uart_ns550.h"
#include "wx_msg_res_pool.h"
#include "wx_frame.h"
#include "xuartns550.h"
WxRs422MasterDriverCfg g_rs422DriverMasterCfg = {
    .rs422DevId = XPAR_UARTNS550_0_DEVICE_ID,
    .rs422Format = {
        .DataBits = XUN_FORMAT_8_BITS,
        .StopBits = XUN_FORMAT_1_STOP_BIT,
        .Parity = XUN_FORMAT_NO_PARITY,
        .BaudRate = 115200
    },
   .intrId = XPAR_FABRIC_UARTNS550_0_VEC_ID,
   .msgQueItemNum = 128,
};

VOID WX_RS422MasterDriver_SentRspAdu2Rs422Master(WxRs422DriverMaster *this, WxModbusAdu *rxAdu)
{
    WxMsg *msg = WX_ApplyEvtMsg(WX_MSG_TYPE_CAN_FRAME);
    if (msg == NULL) {
        return;
    }

    WX_CLEAR_OBJ(msg);
    /* 濉啓娑堟伅淇℃伅 */
    *(WxModbusAdu *)(msg->msgData) = *rxAdu;
    msg->sender = this->moduleId;
    msg->receiver = WX_MODULE_RS422_I_MASTER;
    msg->msgType = WX_MSG_TYPE_RS422_MASTER_ADU_RSP;
    /* 鍙戦�佹秷鎭� */
    UINT32 ret = WX_MsgShedule(this->moduleId, msg->receiver, msg);
    if (ret != WX_SUCCESS) {
        WX_FreeEvtMsg(&msg);
    }
}

/* 澶勭悊鎺ユ敹鍒扮殑PDU */
VOID WX_RS422MasterDriver_ProcRecvAduFromISR(WxRs422DriverMaster *this)
{
    this->status = WX_RS422_MASTER_STATUS_IDLE;
    /* 澶勭悊鎺ユ敹鍒扮殑鎶ユ枃 */
    WX_RS422MasterDriver_SentRspAdu2Rs422Master(this, &this->rxAdu);

    /* 鍙戦�佹秷鎭�氱煡RS422缂栬В鐮佹ā鍧楀鐞� */
    WxModbusAdu *txAdu = &this->txAdu;
    /* 濡傛灉鏈夋秷鎭緟鍙戦�佸垯缁х画鍙戦�� */
    if (xQueueReceiveFromISR(this->msgQueHandle, txAdu, 0) == pdPASS) {
        /* 娓呯┖骞堕璁鹃鏈熸帴鏀剁殑鎶ユ枃澶у皬锛岄槻姝换鍔¤鎶㈠崰鏉ヤ笉鍙婄紦瀛樺鑷翠覆鍙ｆ秷鎭涪澶� */
        UINT32 recvCount; 
        do {
            recvCount = XUartNs550_Recv(&this->rs422Inst, this->rxAdu.value,
            	(unsigned int)txAdu->expectRspLen);
        } while (recvCount);
        
        /* 棣栨鍙戦�佹秷鎭細琚紦瀛樺埌瀹炰緥锛岃繑鍥炵紦瀛樹簡澶氬皯鎶ユ枃 */
        UINT32 sendCount = XUartNs550_Send(&this->rs422Inst, txAdu->value, (unsigned int)txAdu->valueLen);
        if (sendCount == 0) {
            /* 鏄笉鍙兘鍑虹幇鍙戦��0鎯呭喌锛岃繖閲岀畻鏄紓甯镐簡 */
            return;
        }
        this->status = WX_RS422_MASTER_STATUS_TX_ADU;
    }

    return;
}


/* RS422 master涓柇澶勭悊鍑芥暟 */
VOID WX_RS422I_DRIVER_MASTER_IntrHandler(VOID *callBackRef, UINT32 event, UINT32 eventData)
{
    WxRs422DriverMaster *this = callBackRef;
    /*
	 * All of the data has been sent.
	 */
	if (event == XUN_EVENT_SENT_DATA) {
        /* 鍙戦�佸畬姣曞悗鍒囨崲鍒版帴鏀剁姸鎬� */
        this->status = WX_RS422_MASTER_STATUS_RX_ADU;
	}

    /*
	 * All of the data has been received.
	 */
	if (event == XUN_EVENT_RECV_DATA) {
        
        this->rxAdu.valueLen = eventData;
        WX_RS422MasterDriver_ProcRecvAduFromISR(this);
	}

	/*
	 * Data was received, but not the expected number of bytes, a
	 * timeout just indicates the data stopped for 4 character times.
	 */
	if (event == XUN_EVENT_RECV_TIMEOUT) {
        this->rxAdu.valueLen = eventData;
        WX_RS422MasterDriver_ProcRecvAduFromISR(this);
	}
}

UINT32 WX_RS422MasterDriver_ProcTxAduMsg(WxRs422DriverMaster *this, WxRs422MasterDriverMsg *msg)
{
    if (this->status != WX_RS422_MASTER_STATUS_IDLE) {
        /* 缂撳瓨寰呭彂閫佺殑娑堟伅 */
        if (xQueueSend(this->msgQueHandle, &msg->modbusAdu, 0) != pdPASS) {
            return WX_RS422_MASTER_CACHE_MSG_FAIL;
        }
        return WX_SUCCESS;
    }
    WxModbusAdu *rxAdu = &this->rxAdu;
    WxModbusAdu *txAdu = &this->txAdu;
    /* 娓呯┖骞堕璁鹃鏈熸帴鏀剁殑鎶ユ枃澶у皬锛岄槻姝换鍔¤鎶㈠崰鏉ヤ笉鍙婄紦瀛樺鑷翠覆鍙ｆ秷鎭涪澶� */
    UINT32 recvCount; 
    do {
        recvCount = XUartNs550_Recv(&this->rs422Inst, rxAdu->value, txAdu->expectRspLen);
    } while (recvCount);
    
    /* 棣栨鍙戦�佹秷鎭細琚紦瀛樺埌瀹炰緥锛岃繑鍥炵紦瀛樹簡澶氬皯鎶ユ枃 */
    UINT32 sendCount = XUartNs550_Send(&this->rs422Inst, txAdu->value, (unsigned int)txAdu->valueLen);
    if (sendCount == 0) {
        /* 鏄笉鍙兘鍑虹幇鍙戦��0鎯呭喌锛岃繖閲岀畻鏄紓甯镐簡 */
        return WX_RS422_MASTER_SNED_ADU_BUFFER_FAIL;
    }
    this->status = WX_RS422_MASTER_STATUS_TX_ADU;
    return WX_SUCCESS;
}

UINT32 WX_RS422MasterDriver_Construct(VOID *module)
{
    UINT32 ret;
    WxRs422DriverMaster *this = WX_Mem_Alloc(WX_GetModuleName(module), 1, sizeof(WxRs422DriverMaster));
    if (this == NULL) {
        return WX_RS422_MASTER_MEM_ALLOC_FAIL;
    }
    WxRs422MasterDriverCfg *cfg = &g_rs422DriverMasterCfg;
    /* 鍒涘缓RS422娑堟伅缂撳瓨闃熷垪 */
    this->msgQueHandle = xQueueCreate(cfg->msgQueItemNum, sizeof(WxModbusAdu));
    if (this->msgQueHandle == NULL) {
        boot_debug("Error Exit: xQueueCreate fail");
        return WX_RS422_MASTER_CREATE_MSG_QUE_FAIL;
    }
    /* the inst or rs422 used for uart data tx/rx */
    ret = WX_InitUartNs550(&this->rs422Inst, cfg->rs422DevId, &cfg->rs422Format);
    if (ret != WX_SUCCESS) {
        return ret;
    }

    /* 璁剧疆涓柇 */
    ret = WX_SetupUartNs550Interrupt(&this->rs422Inst,
    	WX_RS422I_DRIVER_MASTER_IntrHandler, cfg->intrId, this);
    if (ret != WX_SUCCESS) {
        return ret;
    }

    /* 璁剧疆涓奙odule */
    WX_SetModuleInfo(module, this);
    return WX_SUCCESS;
}

UINT32 WX_RS422MasterDriver_Entry(VOID *module, WxMsg *evtMsg)
{
    WxRs422DriverMaster *this = WX_GetModuleInfo(module);
    if (evtMsg->msgType != WX_MSG_TYPE_RS422_MASTER_DRIVER) {
        return WX_RS422_MASTER_DRIVER_UNSPT_MSGTYPE;
    }
    /* 瀛愭秷鎭被鍨� */
    switch (evtMsg->subMsgType) {
        case WX_SUB_MSG_RS422_DRIVER_MASTER_TX_ADU: {
            return WX_RS422MasterDriver_ProcTxAduMsg(this, (WxRs422MasterDriverMsg *)evtMsg);
        }
        default: {
            return WX_RS422_MASTER_UNSPT_SUBTYPE;
        }
    }
}

UINT32 WX_RS422MasterDriver_Destruct(VOID *module)
{
    WxRs422DriverMaster *this = WX_GetModuleInfo(module);
    if (this == NULL) {
        return WX_SUCCESS;
    }
    WX_Mem_Free(this);
    WX_SetModuleInfo(module, NULL);
    return WX_SUCCESS;
}
