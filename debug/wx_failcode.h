#ifndef WX_FAILCODE_H
#define WX_FAILCODE_H

typedef enum {
    WX_OK = 0,
    WX_SUCCESS = WX_OK,
    WX_ERR = 1,
    WX_INIT_UARTNS550_FAIL,
    WX_UARTNS550_SELF_TEST_FAIL,
    WX_SCUGIC_LOOKUP_CFG_FAIL,
    WX_SCUGIC_CFG_INIT_FAIL,
    WX_SCUGIC_CONNECT_RS422_FAIL,

    WX_UART_NS550_INTR_DEVICE_UNREADY,
    WX_UART_NS550_INTR_HANDLE_NULL,
    WX_UART_NS550_INTR_DATAREF_NULL,

    WX_RS422I_CREATE_MSG_QUE_FAIL,
    WX_RS422I_CREATE_TASK_FAIL_INST_NULL,
    WX_RS422I_CREATE_TASK_FAIL_TXSEMAPHORE_NULL,
    WX_RS422I_RECV_TIMEOUT,
    WX_RS422I_RECV_UNEXPECT_LEN,
    WX_RS422I_MSGQUE_HANDLE_NULL,
    WX_RS422I_MSGQUE_SEND_FAIL,
    WX_RS422I_UNSPT_TX_MSGTYPE,
    WX_RS422I_INVALID_SUB_OPR_TYPE,
    WX_RS422I_READ_REQ_ENCODE_INFO_UNDEF,
    WX_RS422I_WR_REQ_ENCODE_FUNC_UNDEF,
    WX_RS422I_SNED_ADU_BUFFER_FAIL,
    WX_RS422I_SEND_ADU_TIMEOUT,
    WX_RS422I_RECV_SLAVE_ADDR_MISMATCH,
    WX_RS422I_RECV_UNEXPECT_FUNC_CODE,
    WX_RS422I_RECV_UNSPT_FUNC_CODE,
    WX_RS422I_RECV_WR_DATA_LEN_MISMATCH,
    WX_RS422I_RECV_WR_DATA_RSP_CTX_MISMATCH,
    WX_RS422I_RECV_WR_FILE_RSP_UNSPT,
    WX_RS422I_RECV_RD_FILE_RSP_UNSPT,
    WX_RS422I_RECV_EXCP_RSP_LEN_ERR,
    WX_RS422I_RECV_EXCP_RSP_CRC_ERR,
    WX_RS422I_RECV_UNEXPEC_MB_OPR,
    WX_RS422I_RECV_RSP_RD_DATA_ERR,
    WX_RS422I_RECV_RSP_WR_DATA_ERR,
    WX_RS422I_RECV_RSP_RD_FILE_ERR,
    WX_RS422I_RECV_RSP_WR_FILE_ERR,
    WX_RS422I_RECV_RSP_FUNC_CODE_ERR,
    WX_RS422I_RECV_RSP_RD_LEN_MISMATCH,
    WX_RS422I_RECV_RSP_RD_LEN_ERR,
    WX_RS422I_RECV_RSP_RD_CRC_ERR,
    WX_RS422I_RECV_RSP_RD_GET_DEC_FUNC_FAIL,
} WxFailCode;

#endif
