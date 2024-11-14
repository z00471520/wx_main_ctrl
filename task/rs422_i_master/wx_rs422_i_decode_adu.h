#ifndef WX_RS422_I_DECODE_ADU_H
#define WX_RS422_I_DECODE_ADU_H
#include "wx_failcode.h"
WxFailCode WX_RS422I_DecodeAdu(WxRs422ITxAdu *txAdu, WxRs422IRxAdu *rxAdu, WxRs422IMsg *rxMsgBuf);
#endif