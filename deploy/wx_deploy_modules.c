
#include "xparameters.h"
#include "xil_exception.h"
#include "wx_deploy_modules.h"
#include "wx_deploy_tasks.h"
#include "wx_include.h"
#include "wx_msg_schedule.h"
#include "wx_can_driver_b.h"
#include "wx_can_driver_a.h"
#include "wx_rs422_driver_master.h"
#include "wx_rs422_slave_driver.h"
XScuGic_Config *g_wxScuGicCfg = NULL;

XScuGic_Config *WX_GetOrCreateScuGicCfg()
{
    
}
 XScuGic_LookupConfig(XPAR_SCUGIC_0_DEVICE_ID);

/* 模块部署信息 */
WxModuleDeploy g_wxModuleDepolyInfos[] = {
    {
        "can_slave_a",      /* 模块名 */
        WX_MODULE_CAN_SLAVE_A,/* 模块ID */
        WX_CORE_ID_0,              /* 模块运行的核  */
        "task_main",               /* 模块运行的核内任务名 */
        WX_CAN_SLAVE_Construct,  /* 模块构建函数-必选 */
        WX_CAN_SLAVE_Destruct,   /* 模块析构函数 */
        WX_CAN_SLAVE_Entry,      /* 模块消息 */
    },
    {
        "can_slave_b",       /* 模块名 */
        WX_MODULE_CAN_SLAVE_B, /* 模块ID */
        WX_CORE_ID_0,               /* 模块运行的核 */
        "task_main",                /* 模块运行的核内任务名, 需要保证核内有改函数 */
        WX_CAN_SLAVE_Construct,   /* 模块构建函数-必选 */
        WX_CAN_SLAVE_Destruct,    /* 模块析构函数 */
        WX_CAN_SLAVE_Entry,       /* 模块消息 */
    },
    {
        "driver_can_b",       /* 模块名 */
        WX_MODULE_DRIVER_CAN_A, /* 模块ID */
        WX_CORE_ID_0,               /* 模块运行的核 */
        "task_driver",              /* 驱动任务名 */
        WX_CAN_DRIVER_Construct,   /* 模块构建函数-必选 */
        WX_CAN_DRIVER_Destruct,    /* 模块析构函数 */
        WX_CAN_DRIVER_Entry,       /* 模块消息 */
    },
    {
        "driver_can_b",       /* 模块名 */
        WX_MODULE_DRIVER_CAN_B, /* 模块ID */
        WX_CORE_ID_0,                /* 模块运行的核 */
        "task_driver",               /* 驱动任务名 */
        WX_CAN_DRIVER_Construct,   /* 模块构建函数-必选 */
        WX_CAN_DRIVER_B_Destruct,    /* 模块析构函数 */
        WX_CAN_DRIVER_Entry,       /* 模块消息 */
    },
    {
        "driver_rs422_master",       /* 模块名 */
        WX_MODULE_DRIVER_RS422_MASTER, /* 模块ID */
        WX_CORE_ID_0,                /* 模块运行的核 */
        "task_driver",               /* 驱动任务名 */
        WX_RS422_DRIVER_MASTER_Construct,   /* 模块构建函数-必选 */
        WX_RS422_DRIVER_MASTER_Destruct,    /* 模块析构函数 */
        WX_RS422_DRIVER_MASTER_Entry,       /* 模块消息 */
    },
    {
        "driver_rs422_slave",           /* 模块名 */
        WX_MODULE_DRIVER_RS422_SLAVE,   /* 模块ID */
        WX_CORE_ID_0,                   /* 模块运行的核 */
        "task_driver",                  /* 驱动任务名 */
        WX_RS422SlaveDriver_Construct,   /* 模块构建函数-必选 */
        WX_RS422SlaveDriver_Destruct,    /* 模块析构函数 */
        WX_RS422SlaveDriver_Entry,       /* 模块消息 */
    },
};


/* 把一个任务部署到模块 */
UINT32 WX_DeployModules_DeployOneModule(WxTask *task, WxModule *module, WxModuleDeploy *moduleDeploy)
{
    WX_CLEAR_OBJ(module);
    module->moduleName = moduleDeploy->moduleName;
    module->coreId = task->coreId;
    module->moduleId = moduleDeploy->moduleId;
    module->constructFunc    = moduleDeploy->constructFunc;
    module->destructFunc     = moduleDeploy->destructFunc;
    module->entryFunc        = moduleDeploy->entryFunc;
    module->belongTask = task;
    
    /* 构建函数必须有效 */
    if (moduleDeploy->constructFunc == NULL) {
        return WX_ERR;
    }
    /* 根据构建函数创建该模块 */
    UINT32 ret = moduleDeploy->constructFunc(module);
    if (ret != WX_SUCCESS) {
        return ret;
    }

    return WX_SUCCESS;
}

/* 部署模块 */
UINT32 WX_DeployModules(UINT8 curCoreId)
{
    UINT32 ret;
    WxTask *task = NULL;
    WxModule *module = NULL;
    WxModuleDeploy *deploy = NULL;
    /* 遍历模块部署信息 */
    for (UINT32 i = 0; i < sizeof(g_wxModuleDepolyInfos) / g_wxModuleDepolyInfos[0]; i++) {
        deploy = &g_wxModuleDepolyInfos[i];
        if (!WX_IsValidModuleId(deploy->moduleId)) {
            continue;
        }

        /* 如果核ID不相等，则说明当前核任务不包含该模块, 但是需要注册路由以便消息调度 */
        if ((deploy->coreId != curCoreId)) {
            ret = WX_MsgShedule_RegRouter(deploy->moduleId, deploy->coreId, NULL, NULL, NULL);
            if (ret != WX_SUCCESS) {
                wx_critical("Error Exit: WX_MsgShedule_RegRouter(%s) fail(%u)", deploy->moduleName, ret);
                return ret;
            }
            continue;
        }
        task = WX_DeployTasks_QueryTask(deploy->taskName);
        if (task == NULL) {
            wx_critical("Error Exit: WX_DeployTasks_QueryTask(%s) fail", deploy->taskName);
            return WX_ERR;            
        }
        module = &task->modules[deploy->moduleId];
        ret = WX_DeployModules_DeployOneModule(task, module, deploy);
        if (ret != WX_SUCCESS) {
            wx_critical("Error Exit: WX_DeployModules_DeployOneModule(%s) fail(%u)", deploy->moduleName, ret);
            return ret;
        }
        ret = WX_MsgShedule_RegRouter(deploy->moduleId, deploy->coreId, task, module, deploy->entryFunc);
        if (ret != WX_SUCCESS) {
            wx_critical("Error Exit: WX_MsgShedule_RegRouter(%s) fail(%u)", deploy->moduleName, ret)
            return ret;
        }
    }
}