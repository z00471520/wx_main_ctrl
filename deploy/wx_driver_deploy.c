/* 驱动部署信息 */
typedef struct {
    const CHAR *driveName;   /* 驱动模块名 */
    WxModuleId driveId;      /* 驱动模块ID */
    UINT32 coreId;           /* 驱动模块部署的核ID */
    WxModuleConstructFunc   constructFunc; /* 模块构建函数 */
    WxModuleDestructFunc    destructFunc;  /* 模块的析构函数 */
    WxModuleEntryFunc       entryFunc;     /* 模块消息处理函数 */
} WxDriverDeploy;
