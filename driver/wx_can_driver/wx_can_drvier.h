#ifndef __WX_CAN_DRIVER_H__
#define __WX_CAN_DRIVER_H__
#define WX_CAN_DRIVER_MAX_DATA_LEN 8 /* 支持的最大数据长度，BYTE */
#define WX_CAN_DRIVER_FRAME_U32_LEN ((XCANPS_MAX_FRAME_SIZE) / (sizeof(u32))) /* FRAM U32长度 */

typedef struct {
    UINT32 deviceId; /* 设备ID */
    UINT8 baudPrescalar; /* 需要结合输入时钟的频率进行设置，用于确定最终的波特率 */
    UINT8 syncJumpWidth; /* SyncJumpWidth is the Synchronization Jump Width value to set. Valid values are from 0 to 3 */
    UINT8 timeSegment1;  /* TimeSegment1 is the Time Segment 1 value to set. Valid values are from 0 to 15 */
    UINT8 timeSegment2;  /* TimeSegment2 is the Time Segment 2 value to set. Valid values are from 0 to 7 */
    UINT32 bufferQueLen; /* 缓冲CAN帧数量 */
    /* if more please add here */
} WxCanDriverDevCfg;

typedef struct {
    UINT32 intrId;  /* 中断ID */
    XCanPs_SendRecvHandler sendHandle;  /* 发送Handle */
    XCanPs_SendRecvHandler recvHandle;  /* 接收handle */
    XCanPs_ErrorHandler errHandle;      /* 错误Handle */
    XCanPs_EventHandler eventHandle;    /* 事件Handle */
    VOID *callBackRef;                  /* 中断Handle处理函数 */
} WxCanDriverIntrCfg;

typedef struct {
    UINT32 moduleId;  /* 模块ID */
    WxCanDriverDevCfg devCfg;   /* device cfg */  
    WxCanDriverIntrCfg intrCfg;  /* interrupt cfg */
} WxCanDriverCfg;


typedef struct {
    UINT32 standardMessID;      /* 11:bit, Standard Message ID
                                    The Identifier portion for a Standard Frame is 11 bits.
                                    These bits indicate the Standard Frame ID.
                                    This field is valid for both Standard and Extended Frames.
                                */
    UINT32 subRemoteTransReq;   /* 1bit, SRR/RTR Substitute Remote Transmission Request
                                    This bit differentiates between data frames and remote 
                                    frames. Valid only for Standard Frames. 
                                    ‘1’ = Indicates that the message frame is a Remote Frame.
                                    ‘0’ = Indicates that the message frame is a Data Frame.
                                    For Extended frames this bit is 1.
                                */

    UINT32 idExtension;         /* 1bit, Identifier Extension
                                    This bit differentiates between frames using the Standard 
                                    Identifier and those using the Extended Identifier. Valid for 
                                    both Standard and Extended Frames.
                                    ‘1’ = Indicates the use of an Extended Message Identifier.
                                    ‘0’= Indicates the use of a Standard Message Identifier.
                                */
    UINT32 extendedId;          /* 19bit, Extended Message ID
                                    This field indicates the Extended Identifier.
                                    Valid only for Extended Frames. 
                                */
    UINT32 remoteTransReq;      /* 1bit, RTR, Remote Transmission Request
                                    This bit differentiates between data frames and remote 
                                    frames. 
                                    Valid only for Extended Frames.
                                    ‘1’ = Indicates the message object is a Remote Frame
                                    ‘0’ = Indicates the message object is a Data Frame */
    UINT32 dataLengCode;        /* 4bit, Data Length Code
                                    This is the data length portion of the control field of the 
                                    CAN frame. This indicates the number valid data bytes in 
                                    Data Word 1 and Data Word 2 registers. */
    UINT8 data[WX_CAN_DRIVER_MAX_DATA_LEN]; /* Data Byte 0~8 */
} WxCanFrame;


typedef struct {
    XCanPs canInst;
    QueueHandle_t sendQueue;
    WxModuleId moduleId;
    UINT32 resv;
    WxCanDriverCfg *cfg; /* driver cfg */
} WxCanDriver;

UINT32 WX_CAN_DRIVER_Construct(VOID *module);
UINT32 WX_CAN_DRIVER_Entry(VOID *module, WxMsg *evtMsg);
UINT32 WX_CAN_DRIVER_Destruct(VOID *module);
#endif