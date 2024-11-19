#ifndef WX_TASK_H
#define WX_TASK_H
#include "wx_include.h"

/* 任务配置信息 */
typedef struct {
    TaskFunction_t taskCode; /* 指向任务入口函数的指针 */
    CHAR * name;            /* 任务的描述性名称。此参数主要用于方便调试，但也可用于 获取任务句柄。
                            任务名称的最大长度 由 FreeRTOSConfig.h 中的 configMAX_TASK_NAME_LEN 定义。 */
    UINT32 stackDepth;  /* 要分配用作任务堆栈的字数（不是字节数！）。
                            例如，如果 堆栈宽度为 16 位，uxStackDepth 为 100，则将分配 200 字节用作任务 堆栈。
                            再举一例，如果堆栈宽度为 32 位，uxStackDepth 为 400， 则将分配 1600 字节用作任务堆栈。
                            堆栈深度与堆栈宽度的乘积不得超过 size_t 类型变量所能包含的最大值。*/
    VOID *param;        /* 作为参数传递给所创建任务的值。
                            如果 pvParameters 设置为某变量的地址， 
                            则在创建的任务执行时，该变量必须仍然存在， 
                            因此，不能传递堆栈变量的地址。 */
    UBaseType_t priority; /* 创建的任务将以该指定优先级执行。 tskIDLE_PRIORITY */
} WxTaskCfgInfo;

UINT32 WX_TASK_Create(TaskHandle_t *taskHandlePtr, WxTaskCfgInfo *taskCfg);

#endif
