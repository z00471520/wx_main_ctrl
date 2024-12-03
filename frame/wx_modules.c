
#include "xparameters.h"
#include "xil_exception.h"
#include "wx_include.h"
 
#include "wx_can_driver.h"
#include "wx_rs422_driver_master.h"
#include "wx_rs422_slave_driver.h"
#include "wx_can_slave.h"
#include "wx_frame.h"
#include "wx_modules.h"
#include "wx_tasks.h"
#include "wx_debug.h"
/* 妯″潡閮ㄧ讲淇℃伅 */
WxModuleDeploy g_wxModuleDepolyInfos[] = {
     {
        "debug_c0",           /* 妯″潡鍚� */
        WX_MODULE_DEBUG_C0,   /* 妯″潡ID */
        WX_CORE_ID_0,                   /* 妯″潡杩愯鐨勬牳 */
        "task_om",                  /* 椹卞姩浠诲姟鍚� */
        WX_DEBUG_Construct,   /* 妯″潡鏋勫缓鍑芥暟-蹇呴�� */
        WX_DEBUG_Destruct,    /* 妯″潡鏋愭瀯鍑芥暟 */
        WX_DEBUG_Entry,       /* 妯″潡娑堟伅 */
    },
    {
        "debug_c1",           /* 妯″潡鍚� */
        WX_MODULE_DEBUG_C1,   /* 妯″潡ID */
        WX_CORE_ID_1,                   /* 妯″潡杩愯鐨勬牳 */
        "task_om",                  /* 椹卞姩浠诲姟鍚� */
        WX_DEBUG_Construct,   /* 妯″潡鏋勫缓鍑芥暟-蹇呴�� */
        WX_DEBUG_Destruct,    /* 妯″潡鏋愭瀯鍑芥暟 */
        WX_DEBUG_Entry,       /* 妯″潡娑堟伅 */
    },
    {
        "debug_c2",           /* 妯″潡鍚� */
        WX_MODULE_DEBUG_C2,   /* 妯″潡ID */
        WX_CORE_ID_2,                   /* 妯″潡杩愯鐨勬牳 */
        "task_om",                  /* 椹卞姩浠诲姟鍚� */
        WX_DEBUG_Construct,   /* 妯″潡鏋勫缓鍑芥暟-蹇呴�� */
        WX_DEBUG_Destruct,    /* 妯″潡鏋愭瀯鍑芥暟 */
        WX_DEBUG_Entry,       /* 妯″潡娑堟伅 */
    },
    {
        "debug_c3",      /* 妯″潡鍚� */
        WX_MODULE_DEBUG_C3,   /* 妯″潡ID */
        WX_CORE_ID_3,          /* 妯″潡杩愯鐨勬牳 */
        "task_om",                  /* 椹卞姩浠诲姟鍚� */
        WX_DEBUG_Construct,   /* 妯″潡鏋勫缓鍑芥暟-蹇呴�� */
        WX_DEBUG_Destruct,    /* 妯″潡鏋愭瀯鍑芥暟 */
        WX_DEBUG_Entry,       /* 妯″潡娑堟伅 */
    },
    {
        "can_slave_a",      /* 妯″潡鍚� */
        WX_MODULE_CAN_SLAVE_A,/* 妯″潡ID */
        WX_CORE_ID_0,              /* 妯″潡杩愯鐨勬牳  */
        "task_main",               /* 妯″潡杩愯鐨勬牳鍐呬换鍔″悕 */
        WX_CAN_SLAVE_Construct,  /* 妯″潡鏋勫缓鍑芥暟-蹇呴�� */
        WX_CAN_SLAVE_Destruct,   /* 妯″潡鏋愭瀯鍑芥暟 */
        WX_CAN_SLAVE_Entry,      /* 妯″潡娑堟伅 */
    },
    {
        "can_slave_b",     
        WX_MODULE_CAN_SLAVE_B, 
        WX_CORE_ID_0,            
        "task_main",            
        WX_CAN_SLAVE_Construct, 
        WX_CAN_SLAVE_Destruct,   
        WX_CAN_SLAVE_Entry,     
    },
    {
        "driver_can_a",      
        WX_MODULE_DRIVER_CAN_A,
        WX_CORE_ID_0,            
        "task_driver",           
        WX_CAN_DRIVER_Construct,  
        WX_CAN_DRIVER_Destruct,    
        WX_CAN_DRIVER_Entry,     
    },
    {
        "driver_can_b",      
        WX_MODULE_DRIVER_CAN_B, /* 妯″潡ID */
        WX_CORE_ID_0,                /* 妯″潡杩愯鐨勬牳 */
        "task_driver",               /* 椹卞姩浠诲姟鍚� */
        WX_CAN_DRIVER_Construct,   /* 妯″潡鏋勫缓鍑芥暟-蹇呴�� */
        WX_CAN_DRIVER_Destruct,    /* 妯″潡鏋愭瀯鍑芥暟 */
        WX_CAN_DRIVER_Entry,       /* 妯″潡娑堟伅 */
    },
    {
        "driver_rs422_master",       /* 妯″潡鍚� */
        WX_MODULE_DRIVER_RS422_MASTER, /* 妯″潡ID */
        WX_CORE_ID_0,                /* 妯″潡杩愯鐨勬牳 */
        "task_driver",               /* 椹卞姩浠诲姟鍚� */
        WX_RS422MasterDriver_Construct,   /* 妯″潡鏋勫缓鍑芥暟-蹇呴�� */
        WX_RS422MasterDriver_Destruct,    /* 妯″潡鏋愭瀯鍑芥暟 */
        WX_RS422MasterDriver_Entry,       /* 妯″潡娑堟伅 */
    },
    {
        "driver_rs422_slave",           /* 妯″潡鍚� */
        WX_MODULE_DRIVER_RS422_SLAVE,   /* 妯″潡ID */
        WX_CORE_ID_0,                   /* 妯″潡杩愯鐨勬牳 */
        "task_driver",                  /* 椹卞姩浠诲姟鍚� */
        WX_RS422SlaveDriver_Construct,   /* 妯″潡鏋勫缓鍑芥暟-蹇呴�� */
        WX_RS422SlaveDriver_Destruct,    /* 妯″潡鏋愭瀯鍑芥暟 */
        WX_RS422SlaveDriver_Entry,       /* 妯″潡娑堟伅 */
    },
};


/* 鎶婁竴涓换鍔￠儴缃插埌妯″潡 */
UINT32 WX_DeployOneModule(WxTask *task, WxModule *module, WxModuleDeploy *moduleDeploy)
{
    WX_CLEAR_OBJ(module);
    module->moduleName = moduleDeploy->moduleName;
    module->coreId = task->coreId;
    module->moduleId = moduleDeploy->moduleId;
    module->constructFunc    = moduleDeploy->constructFunc;
    module->destructFunc     = moduleDeploy->destructFunc;
    module->entryFunc        = moduleDeploy->entryFunc;
    module->belongTask     = task;

    if (moduleDeploy->constructFunc == NULL) {
        return WX_MODULE_CONSTRUCT_FUNC_NULL;
    }

    UINT32 ret = moduleDeploy->constructFunc(module);
    if (ret != WX_SUCCESS) {
        return ret;
    }

    return WX_SUCCESS;
}

UINT32 WX_DeployModules(UINT8 curCoreId)
{
    UINT32 ret;
    WxTask *task = NULL;
    WxModule *module = NULL;
    WxModuleDeploy *deploy = NULL;
    UINT32 moduleNum = sizeof(g_wxModuleDepolyInfos) / sizeof(g_wxModuleDepolyInfos[0]);
    for (UINT32 i = 0; i < moduleNum; i++) {
        deploy = &g_wxModuleDepolyInfos[i];
        if (!WX_IsValidModuleId(deploy->moduleId)) {
            continue;
        }

        if ((deploy->coreId != curCoreId)) {
            ret = WX_RegModuleRouter(deploy->moduleId, deploy->coreId, NULL, NULL);
            if (ret != WX_SUCCESS) {
                boot_debug("Error Exit: WX_RegModuleRouter(%s) fail(%u)", deploy->moduleName, ret);
                return ret;
            }
            continue;
        }
        boot_debug("Deploy Module(%s) start...", deploy->moduleName);
        task = WX_QueryTask(deploy->taskName);
        if (task == NULL) {
            boot_debug("Error Exit: module(%s) WX_QueryTask(%s) fail", deploy->moduleName, deploy->taskName);
            return WX_MODULE_TASK_NOT_EXIST;            
        }
        module = &task->modules[deploy->moduleId];
        ret = WX_DeployOneModule(task, module, deploy);
        if (ret != WX_SUCCESS) {
            boot_debug("Error Exit: WX_DeployOneModule(%s) fail(%u)", deploy->moduleName, ret);
            return ret;
        }
        ret = WX_RegModuleRouter(deploy->moduleId, deploy->coreId, task, module);
        if (ret != WX_SUCCESS) {
            boot_debug("Error Exit: WX_RegModuleRouter(%s) fail(%u)", deploy->moduleName, ret);
            return ret;
        }
        
        boot_debug("Deploy Module(%s) success!", deploy->moduleName);
    }

    return WX_SUCCESS;
}
