# 1.任务部署

# 2.模块部署步骤
步骤1：定义新增模块，需要提供模块名和模块ID
```c
WxModuleDef g_wxModuleInfo[];
```

步骤2：定义模块运行的任务和核ID，提供模块构建函数, 在如下数组中定义
```c
WxModuleDeploy g_wxModuleDepolyInfos[]
```

