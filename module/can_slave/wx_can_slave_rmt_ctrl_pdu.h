#ifndef __WX_CAN_SLAVE_RMT_CTRL_PDU_H__
#define __WX_CAN_SLAVE_RMT_CTRL_PDU_H__
#include "wx_can_slave.h"
UINT32 WX_CAN_SLAVE_DecRmtCtrlPduReset(VOID *this, WxRmtCtrlPdu *pdu, WxRmtCtrlReqMsg *msg);
UINT32 WX_CAN_SLAVE_DecRmtCtrlPdu(WxCanSlave *this, WxRmtCtrlPdu *pdu, WxRmtCtrlReqMsg *msg);
UINT32 WX_CAN_SLAVE_SendPdu2CanIf(WxCanSlave *this, WxRmtCtrlPdu *pdu);
#endif
