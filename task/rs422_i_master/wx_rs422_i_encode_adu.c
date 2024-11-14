#include "wx_rs422_inner_task.h"
#include "wx_failcode.h"
#include "wx_rs422_inner_intf.h"
#include "wx_log.h"
#include "wx_rs422_i_encode_adu.h"
#include "wx_modbus.h"

WxFailCode WX_RS422I_EncodeAdu(WxRs422IMsg *txMsg, WxRs422IAdu *txAdu)
{
    switch (txMsg->msgType) {
        case WX_RS422I_MSG_READ_DATA:
            return WX_RS422I_EncodeAduReadDataReq(txMsg, txAdu);
        case WX_RS422I_MSG_WRITE_DATA: {
            return WX_RS422I_EncodeAduWriteDataReq(txMsg, txAdu);
        }
        /* if more pleace add here */
        default: {
            return WX_RS422I_UNSPT_TX_MSGTYPE;
        }
    }
}
