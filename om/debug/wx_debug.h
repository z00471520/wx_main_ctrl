#ifndef WX_DEBUG_H
#define WX_DEBUG_H

#include "wx_include.h"
#include "wx_msg_common.h"
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

/* 瑙ｉ噴鏉裤�佹牳鐨勭粨鏋勪綋*/
typedef struct {
	CHAR 	*Id;
    CHAR    *Desc;
}WxDbgBoardCore;

/* 璋冭瘯鍛戒护鍒楄〃 */
typedef struct {
	CHAR *cmdName;
    CHAR *helpDesc;
	VOID (*cmdExecFunc)(CHAR *argv[], UINT32 argc);
}WxDbgCmdItem;

/* 妫�鏌ヨ皟璇曞懡浠ゆ牸寮� */
#define WX_DBG_MIN_ARGC_NUM 5
#define WX_DBG_MAX_ARGC_NUM 64
typedef struct {
    UINT8 moduleId;   // 妯″潡ID 
    UINT8 coreId;     // 鎵�鍦ㄦ牳ID
    UINT8 resv[2];  /* 棰勭暀瀛楁 */
    UINT32 resv32;  /* 棰勭暀瀛楁 */
	CHAR *argv[WX_DBG_MAX_ARGC_NUM];	/* cmd para */
	UINT32 argc; /* cmd para num */
    //VOID (*debugEntry)(CHAR *argv[], UINT32 argc); // debug鍏ュ彛鍑芥暟
} WxDebug;


/* 妯″潡鏋勫缓鍑芥暟 */
UINT32 WX_DEBUG_Construct(VOID *module);
/* 妯″潡鏋愭瀯鍑芥暟 */
UINT32 WX_DEBUG_Destruct(VOID *module);
/* 妯″潡娑堟伅澶勭悊鍏ュ彛 */
UINT32 WX_DEBUG_Entry(VOID *module, WxMsg *msg);

#endif
