#include "wx_comm_rs422.h"


/*
 * communication module init
 */
UINT32 WX_Comm_Init(void)
{
	UINT32 ret = WxRs422Init(void);
	if (ret != VOS_OK) {}
}


