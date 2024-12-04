#ifndef WX_FAILCODE_H
#define WX_FAILCODE_H

typedef enum {
    WX_OK = 0,
    WX_SUCCESS = WX_OK,
    WX_ERR = 1,
    WX_FAILURE,
    WX_TO_BE_CONTINUE,
    WX_MEM_ALLOC_FAIL,
    WX_SHEDULE_TO_CORE_NOT_SPT,
    WX_ERR_NO_ENTRY_FUNC,
    WX_NOT_SUPPORT,
    WX_INVALID_MODULE_ID,
    WX_INIT_UARTNS550_FAIL,
    WX_UARTNS550_SELF_TEST_FAIL,
    WX_SCUGIC_LOOKUP_CFG_FAIL,
    WX_SCUGIC_CFG_INIT_FAIL,
    WX_SCUGIC_CONNECT_RS422_FAIL,
    WX_CREATE_MSG_QUE_FAIL,
    WX_INVALID_TASK_HANDLER,
    WX_CREATE_QUEUE_FAIL,
    WX_APPLY_EVT_MSG_ERR,
    WX_MODULE_CONSTRUCT_FUNC_NULL,
    WX_MODULE_TASK_NOT_EXIST,
    
    WX_MSG_REG_INVALID_MODULE_ID,
    WX_MSG_REG_NO_MSG_ROUTER_LIST,
    WX_MSG_DISPATCH_INVALID_SEND_METHOD,
    WX_MSG_DISPATCH_INVALID_SENDER,
    WX_MSG_DISPATCH_NO_TASK_HANDLE,
    WX_MSG_DISPATCH_NO_MSG_ROUTER_LIST,
	WX_CAN_DRIVER_MSG_TYPE_INVALID,
    WX_UART_NS550_INTR_DEVICE_UNREADY,
    WX_UART_NS550_INTR_HANDLE_NULL,
    WX_UART_NS550_INTR_DATAREF_NULL,
    WX_UART_NS550_INTR_CTRL_UNREADY,
    
    /* MODBUS相关 */
    WX_MODBUS_ADU_CRC_CHECK_FAIL,
    WX_MODBUS_ADU_ENCODE_INVALID_VALUE_SIZE,
    WX_MODBUS_ADU_ENCODE_INSUFFICIENT_SPACE,
    WX_MODBUS_ADU_ENCODE_FAIL,
    WX_MODBUS_ADU_DECODE_BUF_SIZE_ERR,
    WX_MODBUS_ADU_DECODE_INVALID_START_LEN,
    WX_MODBUS_ADU_DECODE_INVALID_LEN,
    WX_MODBUS_CRC_CHECK_VALUE_LEN_ERR,

    /*WX_RS422_MASTER */
    WX_RS422_MASTER_CREATE_MSG_QUE_FAIL,
    WX_RS422_MASTER_CREATE_TASK_FAIL_INST_NULL,
    WX_RS422_MASTER_CREATE_TASK_FAIL_TXSEMAPHORE_NULL,
    WX_RS422_MASTER_CREATE_TASK_FAIL_RXSEMAPHORE_NULL,
    WX_RS422_MASTER_RECV_TIMEOUT,
    WX_RS422_MASTER_RECV_UNEXPECT_LEN,
    WX_RS422_MASTER_MSGQUE_HANDLE_NULL,
    WX_RS422_MASTER_MSGQUE_SEND_FAIL,
    WX_RS422_MASTER_UNSPT_TX_MSGTYPE,
    WX_RS422_MASTER_INVALID_SUB_OPR_TYPE,
    WX_RS422_MASTER_READ_REQ_ENCODE_INFO_UNDEF,
    WX_RS422_MASTER_WR_REQ_ENCODE_FUNC_UNDEF,
    WX_RS422_MASTER_WR_REQ_ENCODE_FAIL,
    WX_RS422_MASTER_SNED_ADU_BUFFER_FAIL,
    WX_RS422_MASTER_SEND_ADU_TIMEOUT,
    WX_RS422_MASTER_RECV_SLAVE_ADDR_MISMATCH,
    WX_RS422_MASTER_RECV_UNEXPECT_FUNC_CODE,
    WX_RS422_MASTER_RECV_UNSPT_FUNC_CODE,
    WX_RS422_MASTER_RECV_WR_DATA_LEN_MISMATCH,
    WX_RS422_MASTER_RECV_UNSPT_MSG_TYPE,
    WX_RS422_MASTER_RECV_WR_DATA_RSP_CTX_MISMATCH,
    WX_RS422_MASTER_RECV_WR_FILE_RSP_UNSPT,
    WX_RS422_MASTER_RECV_RD_FILE_RSP_UNSPT,
    WX_RS422_MASTER_RECV_EXCP_RSP_LEN_ERR,
    WX_RS422_MASTER_RECV_EXCP_RSP_CRC_ERR,
    WX_RS422_MASTER_RECV_UNEXPEC_MB_OPR,
    WX_RS422_MASTER_RECV_RSP_RD_DATA_ERR,
    WX_RS422_MASTER_RECV_RSP_WR_DATA_ERR,
    WX_RS422_MASTER_RECV_RSP_RD_FILE_ERR,
    WX_RS422_MASTER_RECV_RSP_WR_FILE_ERR,
    WX_RS422_MASTER_RECV_RSP_FUNC_CODE_ERR,
    WX_RS422_MASTER_RECV_RSP_RD_LEN_MISMATCH,
    WX_RS422_MASTER_RECV_RSP_RD_LEN_ERR,
    WX_RS422_MASTER_RECV_RSP_RD_CRC_ERR,
    WX_RS422_MASTER_RECV_RSP_RD_GET_DEC_FUNC_FAIL,
    WX_RS422_MASTER_CACHE_MSG_FAIL,
    WX_RS422_MASTER_RECV_RSP_RD_VALUE_LEN_ERR,
    WX_RS422_MASTER_PROC_RSP_ADU_SUB_MSG_TYPE_ERR,
    WX_RS422_MASTER_MEM_ALLOC_FAIL,
    WX_RS422_MASTER_UNSPT_MSGTYPE,
    WX_RS422_MASTER_UNSPT_SUBTYPE,
	WX_RS422_MASTER_DRIVER_UNSPT_MSGTYPE,

    /* RS422 从机相关 */
    WX_RS422_SLAVE_DATA_READ_HANDLE_UNFINE,
    WX_RS422_SLAVE_DATA_READ_GET_FAIL,
    WX_RS422_SLAVE_DATA_READ_ENC_FAIL,
    WX_RS422_SLAVE_DATA_WRITE_HANDLE_NULL,
    WX_RS422_SLAVE_DATA_WRITE_HANDLE_UNFINE,
    WX_RS422_SLAVE_DATA_WRITE_LOCAL_FAIL,
    WX_RS422_SLAVE_DATA_WRITE_DEC_FAIL,
    WX_RS422_SLAVE_FILE_READ_UNSPT,
    WX_RS422_SLAVE_FILE_WRITE_UNSPT,
    WX_RS422_SLAVE_SEND_ADU_FAIL,
    WX_RS422_SLAVE_SEND_ADU_TIMEOUT,
    WX_RS422_SLAVE_RECV_ADU_UNEXPECT_LEN,
    WX_RS422_SLAVE_UNSPT_MSG_TYPE,
    WX_RS422_SLAVE_DRIVER_UNSPT_MSG_TYPE,
    WX_RS322_SLAVE_INVALID_EXCP_CODE,
    WX_RS422_SLAVE_DRIVER_MEM_ALLOC_FAIL,
    WX_RS422_SLAVE_DRIVER_INIT_GPIO_FAIL,

    /* 消息调度相关 */
    WX_MSG_QUE_REG_INVALID_RECVER,
    WX_MSG_DISPATCH_INVALID_RECVER,
    WX_MSG_DISPATCH_RECVER_UNREG_MSG_QUE,
    WX_MSG_DISPATCH_INVALID_MSG_HEADER,
    WX_MSG_DISPATCH_MSG_LEN_ERR,
    WX_MSG_DISPATCH_MSG_QUE_FULL,

    /* TASK相关 */
    WX_TASK_CREATE_FAIL,
    
    /* CAN 从机任务 */
    WX_CAN_SLAVE_MODULE_CFG_UNDEF,
    WX_CAN_SLAVE_CREATE_MSG_QUE_FAIL,
    WX_CAN_SLAVE_UNSPT_MSG_TYPE,
    WX_CAN_SLAVE_CAN_FRAME_DATA_LEN_ERR,
    WX_CAN_SLAVE_INVALID_RMT_CTRL_CODE,
    WX_CAN_SLAVE_INVALID_REQ_MSG_TYPE,
    WX_CAN_SLAVE_UNDEFINE_REQ_MSG_HANDLE,
    WX_CAN_SLAVE_CTRL_CODE_DEC_UNSPT,
    WX_CAN_SLAVE_FRAME_NUM_EXCEED_LIMIT,
    WX_CAN_SLAVE_CFG_INFO_UN_INIT,
    WX_CAN_SLAVE_CAN_FRAME_LEN_ZERO,
    WX_CAN_SLAVE_ENC_RSP_PDU_TYPE_ERR,
    WX_CAN_SLAVE_UNDEFINE_RSP_PDU_ENC_HANDLE,
    WX_CAN_DRIVER_CONFIG_TX_QUEUE_CREATE_FAIL,
    /* CAN驱动相关 */
    WX_CAN_DRIVER_LOOKUP_CONFIG_FAIL,
    WX_CAN_DRIVER_CONFIG_INIT_FAIL,
    WX_CAN_DRIVER_CONFIG_INVALID_CAN_INST_PRT,
    WX_CAN_DRIVER_CONFIG_TX_BUFF_FULL,
    WX_CAN_DRIVER_CONFIG_TX_FAIL_FULL,
    WX_CAN_DRIVER_CONFIG_SELF_TEST_FAIL,
    WX_CAN_DRIVER_CONFIG_TX_QUEUE_SEND_FAIL,
    WX_CAN_DRIVER_MEM_ALLOC_FAIL,
    WX_CAN_DRIVER_CONFIG_NOT_FOUND,

    /* DEBUG相关 */
    WX_DEBUG_MSGTYPE_ERR,
    WX_DBG_CHECK_ARGC_ERR,
    WX_DBG_CHECK_DBGCMD_ERR,
    WX_DBG_CHECK_BOARD_ERR,
    WX_DBG_CHECK_CORE_ERR,
    WX_DBG_CHECK_ID_EMPTY,
    WX_DBG_Str2U32_ID_NOT_NUM,
    WX_DBG_Str2U32_TRANS_FAIL,
    WX_DBG_Str2U32_OUT_OF_RANGE,
    WX_DEBUG_MALLOC_ERR,

} WxFailCode;

#define wx_fail_code_cnt(c)
#endif
