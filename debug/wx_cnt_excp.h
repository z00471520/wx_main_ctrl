#ifndef WX_CNT_EXCP_H
#define WX_CNT_EXCPM_H
/* excp cnt from 10000  */
typedef enum {
	WX_EXCP_CNT_START = 10000, // keep this first
	WX_EXCP_UNEXPECT_MSG_TYPE,
	WX_EXCP_INVALID_MSG_BODY_LEN,
	WX_EXCP_CAN_FRAME_DATA_LEN_ERR,
	WX_EXCP_RS422_SLAVE_RD_SEND_ADU_FAIL,
	WX_EXCP_RS422_SLAVE_WR_SEND_ADU_FAIL,
	WX_EXCP_RS422_SLAVE_SEND_ERR_ADU_FAIL,

	/* module A  */



	/* module B */


	/* if more please add here */
	WX_EXCP_CNT_BUTT,
} WxExcpCntDef;
#endif
