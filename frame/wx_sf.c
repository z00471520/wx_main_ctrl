
#include "wx_sf.h"
#include "queue.h"
#include "wx_include.h"
WxModuleInfo g_wxModuleInfo[] = {
    /* 模块名(小写)   模块ID */
    {"rs422_i_slave",     WX_MODULE_ID_RS422_I_SLAVE},
    {"rs422_i_master",    WX_MODULE_ID_RS422_I_MASTER},
    {"can_slave_a",       WX_MODULE_ID_CAN_SLAVE_A},
    {"can_slave_b",       WX_MODULE_ID_CAN_SLAVE_B},
};

WxModuleDeployInfo g_wxModuleDepolyInfos[] = {
    /* 模块名       模块所在任务,  模块构建函数， 模块析构函数   模块消息处理入口  */
    {"rs422_slave", "task_core", NULL, NULL, NULL},
};


typedef struct {
    QueueSetMemberHandle_t queSetHandle; /* queue set handle */
} WxCoreTask;

/* MSG QUE到消息处理入口函数的映射 */
WxModuleEntryFunc g_wxMsgQue2EntryFunc[]
{

}

WxModuleEntryFunc WX_CORE_GetMsgEntryFunc(QueueSetMemberHandle_t msgQueHandle)
{

}

/* 核心任务的运行函数 */
VOID WX_CORE_Task(VOID *param)
{
    WxCoreTask *this = param;
    QueueSetMemberHandle_t activeMem;
    WxModuleEntryFunc entryFunc = NULL;
    WxEntryPara para = {0};
    UINT32 ret;
    for (;;) {
        /* 等待外界发送的消息 */
        para->msgQueHandle = xQueueSelectFromSet(this->queSetHandle, portMAX_DELAY);
        if (para->msgQueHandle == NULL) {
            continue;
        }
        /* 获取消息队列对应的处理入口函数 */
        entryFunc = WX_CORE_GetMsgEntryFunc(para->msgQueHandle);
        if (entryFunc == NULL) {
            /* 一般不能出，除了属于异常，看看注册表是不是写的有问题 */
            wx_excp_cnt(WX_EXCP_CORE_TASK_ENTRY_UNFOUND);
            continue;
        }
        /* 调用消息处理入口函数处理该消息队列 */
        ret = entryFunc(&para);
        if (ret != WX_SUCCESS) {
            wx_fail_code_cnt(ret);
        }
    }

}
