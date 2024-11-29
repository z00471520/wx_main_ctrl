#ifndef WX_DEBUG_H
#define WX_DEBUG_H

#include "wx_include.h"

/* 
 * dbgcmd				0
 * board				1
 * boardid				2
 * core					3
 * coreid				4
 * handle				5
 * handleitem			6
*/
#define WX_DBG_ARGV_DBGCMD      0
#define WX_DBG_ARGV_BOARD       1
#define WX_DBG_ARGV_BOARDID     2
#define WX_DBG_ARGV_CORE        3
#define WX_DBG_ARGV_COREID      4
#define WX_DBG_ARGV_HANDLE      5
#define WX_DBG_ARGV_HANDLEITEM  6

/* 解释板、核的结构体*/
typedef struct {
	CHAR 	*Id;
    CHAR    *Desc;
}WxDbgBoardCore;

/* 调试命令列表 */
typedef struct {
	CHAR *cmdName;
    CHAR *helpDesc;
	VOID (*cmdExecFunc)(CHAR *argv[], UINT32 argc);
}WxDbgCmdItem;

/* 检查调试命令格式 */
#define WX_DBG_MIN_ARGC_NUM 5
#define WX_DBG_MAX_ARGC_NUM 64
typedef struct {
    UINT8 moduleId;   // 模块ID 
    UINT8 coreId;     // 所在核ID
    UINT8 resv[2];  /* 预留字段 */
    UINT32 resv32;  /* 预留字段 */
	CHAR *argv[WX_DBG_MAX_ARGC_NUM];	/* cmd para */
	UINT32 argc; /* cmd para num */
    //VOID (*debugEntry)(CHAR *argv[], UINT32 argc); // debug入口函数
} WxDebug;


/* 模块构建函数 */
UINT32 WX_DEBUG_Construct(VOID *module);
/* 模块析构函数 */
UINT32 WX_DEBUG_Destruct(VOID *module);
/* 模块消息处理入口 */
UINT32 WX_DEBUG_Entry(VOID *module, WxMsg *msg);

#endif
