
#include "xparameters.h"
#include "xil_exception.h"
#include "wx_include.h"
 
#include "wx_can_driver.h"
#include "wx_rs422_master_driver.h"
#include "wx_rs422_slave_driver.h"
#include "wx_can_slave.h"
#include "wx_rs422_master.h"
#include "wx_rs422_slave.h"
#include "wx_frame.h"
#include "wx_modules.h"
#include "wx_tasks.h"
#include "wx_debug.h"
/* module deploy info */
WxModuleDeploy g_wxModuleDepolyInfos[] = {
    {"debug_c0",        WX_MODULE_DEBUG_C0,  	WX_CORE_ID_0, "task_om",  	WX_DEBUG_Construct,  WX_DEBUG_Destruct, WX_DEBUG_Entry},
    {"debug_c1",        WX_MODULE_DEBUG_C1,  	WX_CORE_ID_1, "task_om",  	WX_DEBUG_Construct,  WX_DEBUG_Destruct, WX_DEBUG_Entry},
    {"debug_c2",        WX_MODULE_DEBUG_C2,  	WX_CORE_ID_2, "task_om",  	WX_DEBUG_Construct,  WX_DEBUG_Destruct, WX_DEBUG_Entry},
    {"debug_c3",        WX_MODULE_DEBUG_C3,     WX_CORE_ID_3, "task_om",  	WX_DEBUG_Construct,  WX_DEBUG_Destruct, WX_DEBUG_Entry},
    {"rs422_master",    WX_MODULE_RS422_MASTER, WX_CORE_ID_0, "task_main",  WX_RS422_MASTER_Construct, WX_RS422_MASTER_Destruct,  WX_RS422_MASTER_Entry},
	{"rs422_slave",     WX_MODULE_RS422_SLAVE, 	WX_CORE_ID_0, "task_main",  WX_RS422Slave_Construct, WX_RS422Slave_Destruct,  WX_RS422Slave_Entry},
    {
        "can_slave_a",   
        WX_MODULE_CAN_SLAVE_A,
        WX_CORE_ID_0,              
        "task_main",              
        WX_CAN_SLAVE_Construct,   
        WX_CAN_SLAVE_Destruct,    
        WX_CAN_SLAVE_Entry,       
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
        "can_a_driver",      
        WX_MODULE_DRIVER_CAN_A,
        WX_CORE_ID_0,            
        "task_driver",           
        WX_CAN_DRIVER_Construct,  
        WX_CAN_DRIVER_Destruct,    
        WX_CAN_DRIVER_Entry,     
    },
    {
        "can_b_driver",      
        WX_MODULE_DRIVER_CAN_B, /* 妯″潡ID */
        WX_CORE_ID_0,               
        "task_driver",               
        WX_CAN_DRIVER_Construct,    
        WX_CAN_DRIVER_Destruct,     
        WX_CAN_DRIVER_Entry,        
    },
    {
        "rs422_master_driver",    
        WX_MODULE_RS422_MASTER_DRIVER, /* 妯″潡ID */
        WX_CORE_ID_0,               
        "task_driver",               
        WX_RS422MasterDriver_Construct,    
        WX_RS422MasterDriver_Destruct,     
        WX_RS422MasterDriver_Entry,        
    },
    {
        "rs422_slave_driver",        
        WX_MODULE_RS422_SLAVE_DRIVER,  
        WX_CORE_ID_0,                  
        "task_driver",                  
        WX_RS422SlaveDriver_Construct,    
        WX_RS422SlaveDriver_Destruct,     
        WX_RS422SlaveDriver_Entry,        
    },
};


/* deploy one module on current task */
UINT32 WX_SaveModuleInfo2Task(WxTask *task, WxModule *module, WxModuleDeploy *moduleDeploy)
{
    WX_CLEAR_OBJ(module);
    module->moduleName = moduleDeploy->moduleName;
    module->coreId = WX_GetCurCoreId();
    module->moduleId = moduleDeploy->moduleId;
    module->constructFunc    = moduleDeploy->constructFunc;
    module->destructFunc     = moduleDeploy->destructFunc;
    module->entryFunc        = moduleDeploy->entryFunc;
    module->belongTask     = task;
    boot_debug("Deploy Module(%s, %u) to Task(%s) Core(%u) success!", module->moduleName,
        module->moduleId, task->taskName, module->coreId);
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
        ret = WX_SaveModuleInfo2Task(task, module, deploy);
        if (ret != WX_SUCCESS) {
            boot_debug("Error Exit: WX_SaveModuleInfo2Task(%s) fail(%u)", deploy->moduleName, ret);
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
