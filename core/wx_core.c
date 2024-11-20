

#include "wx_core.h"
#include "task.h"
#include "wx_include.h"

typedef struct {
    CHAR *moduleName;
} WxCoreModuleCfgInfo;

/* CORE模块部署配置，不同核部署的文件不一样 */
typedef struct {
    UINT32 coreId;
    
} WxCoreDeployCfg;

typedef struct
{
    UINT32 coreId;
    QueueSetHandle_t queSet; /* 队列集 */      
} WxCore;


WxCore g_wxCore = {0};

UINT32 WX_CoreInit(UINT32 coreId)
{
    WxCore *this = &g_wxCore;
    this->coreId = coreId;
}

VOID WX_CORE_MainFunc(VOID *para)
{

}