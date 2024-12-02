#include "wx_include.h"
#include "wx_debug.h"
#include "wx_debug_req_intf.h"
#include "wx_frame.h"

/* 鍏堝畾涔� */
VOID WX_Debug_Help(CHAR *argv[], UINT32 argc);
VOID WX_Debug_Show(CHAR *argv[], UINT32 argc);
VOID WX_Debug_Set(CHAR *argv[], UINT32 argc);

BOOL WX_IsNumStr(CHAR *str) 
{
	CHAR *p = str;
    while ((*p) != '\0') {
        if ((*p) >= '0' && (*p) <= '9') {
        	p++;
        }
        return FALSE;
    }
    return TRUE;
}

/* 瀛楃涓茶浆uint32 */
UINT32 WX_Str2U32Value(CHAR *str, UINT32 *value) 
{   
    if (str[0] == '\0'){/* 绌哄瓧绗︿覆 */
        return WX_DBG_CHECK_ID_EMPTY;
    }

    if (!WX_IsNumStr(str)) {/* 涓嶆槸鏁板瓧瀛楃涓�  */
        return WX_DBG_Str2U32_ID_NOT_NUM;
    }

    CHAR *endptr;
    UINT32 errorno = 0; /* 閿欒鍙� */
    UINT32 temp = strtoul(str, &endptr, 0);

    if (*endptr != '\0') { /* 鏈夐澶栭潪鏁板瓧瀛楃 */
        return WX_DBG_Str2U32_TRANS_FAIL;
    }
    
    if (temp > WX_U32_MAX) { /* 瓒呭嚭UINT32鑼冨洿 */
        return WX_DBG_Str2U32_OUT_OF_RANGE;
    }

    *value = temp;

    return errorno;
}

/* 妫�鏌d鍚堟硶鎬� */
UINT32 WX_CheckId(CHAR *str, UINT32 *value) 
{    
    UINT32 ret = WX_Str2U32Value(str, value);

    if (ret != 0) {/*  杞崲澶辫触  */
        wx_show("Error Exit: WX_CheckId %s fail(%u)", str, ret);
        return ret;
    }

    return ret;/*  杞崲鎴愬姛 */
}

/* 妫�鏌ヨ皟璇曞懡浠ゆ牸寮� */
UINT32 WX_CheckDebugCmd(CHAR *argv[], UINT32 argc)
{
    /* 鍙傛暟涓暟妫�鏌� */
    if (argc < WX_DBG_MIN_ARGC_NUM) {
        return WX_DBG_CHECK_ARGC_ERR;
    }

    /* 0 -- must be dbgcmd */
    if (strcmp(argv[WX_DBG_ARGV_DBGCMD], "dbgcmd") ) {
        return WX_DBG_CHECK_DBGCMD_ERR;
    }

    /* 1 -- must be board */
    if (strcmp(argv[WX_DBG_ARGV_BOARD], "board") ) {
        return WX_DBG_CHECK_BOARD_ERR;   
    }

    /* 3 -- must be core */
    if (strcmp(argv[WX_DBG_ARGV_CORE], "core") ) {
        return WX_DBG_CHECK_CORE_ERR;
    }

    return WX_SUCCESS;
}

/* 鍒ゆ柇鏄惁涓哄綋鍓嶈澶� */
UINT32 WX_Debug_IsBoardCoreLocate(UINT32 boardId,UINT32 coreId)
{
    return 1;
}

/* handle 鐩綍 */
WxDbgCmdItem g_wxDbgItems[] = 
{
    {"?",           "show help",        WX_Debug_Help},
	{"set", 	    "set param",        WX_Debug_Set},
	{"show",	    "show information", WX_Debug_Show},
    {"clear",       "xxxx",                   NULL}
    /* if more please add here */
};

/*
*  Debug Handle Help 
*  -----------------
*  <Cmd>         <Description>
*  ?           show help 
*  set         set param
*  show        show information
*  xxxx        xxxx
*/
VOID WX_Debug_Help(CHAR *argv[], UINT32 argc) 
{
    wx_show("DEBUG Handle Help\n");
    wx_show("-----------------\n");
    wx_show("%-16s %s\n", "(Cmd)", "(Description)");
    WxDbgCmdItem *item = NULL;
    for (UINT32 i = 0; i < sizeof(g_wxDbgItems) / sizeof(g_wxDbgItems[0]); i++) {
    	item = &g_wxDbgItems[i];
        wx_show("%-16s %s\n", item->cmdName, item->helpDesc);
    }
}

/* 鏈湴璋冭瘯鍛戒护 cmd func */
VOID WX_Debug_ThisDevice(CHAR *argv[], UINT32 argc)
{
    WxDbgCmdItem *item = NULL;
    for (UINT32 i= 0; i < sizeof(g_wxDbgItems) / sizeof(g_wxDbgItems[0]); i++) {
        item = &g_wxDbgItems[i];
        if (strcmp(argv[0], item->cmdName) != 0) {
            continue;/* 寰幆鐩村埌鎵惧埌鍖归厤handle */
        }

        if (item->cmdExecFunc == NULL) { /* 褰撳墠handle鏃犳搷浣滃嚱鏁�*/
            wx_show("<cmd> %s not support.", item->cmdName);
            return;
        }
        item->cmdExecFunc(&argv[1], argc - 1);
        return;
        
    }
    
    wx_show("(cmd) %s not exist.", argv[0]); /*娌℃湁鎵惧埌鍖归厤鐨刪andle*/
    return;
}

/* 杩滅璋冭瘯鍛戒护 */
UINT32 WX_Debug_OtherDevice(CHAR *argv[], UINT32 argc)
{
    return WX_ERR;
}

VOID WX_Debug_ProcCmd(CHAR *argv[], UINT32 argc)
{
    /*  鍒ゆ柇鍛戒护鍚堟硶 */
    UINT32 ret = WX_CheckDebugCmd(argv, argc);
    if (ret != WX_SUCCESS) {
        wx_show("Error Exit: WX_CheckDebugCmd fail(%u)", ret);
        return;
    }

    /* 鍒ゆ柇board鍜宑ore Id*/
    UINT32 boardId, coreId;
    if (WX_CheckId(argv[WX_DBG_ARGV_BOARDID], &boardId)){
        return;
    }
    if (WX_CheckId(argv[WX_DBG_ARGV_COREID], &coreId)){
        return;
    }

    // [dbgcmd][board][x][core][y][cmd]
    /*  鍒ゆ柇褰撳墠璁惧 */
    if (WX_Debug_IsBoardCoreLocate(boardId, coreId)) {
        WX_Debug_ThisDevice(&argv[WX_DBG_ARGV_HANDLE], argc - 5); /* 鏈湴璋冭瘯鍛戒护 */
    } else {
        WX_Debug_OtherDevice(argv, argc); /* 杩滅璋冭瘯鍛戒护锛岄潪鏈澶� */
    }   

    return;
}
/* cmd str 2 arg list */
UINT32 WX_Debug_Cmd2ArgList(WxDbgCmd *dbgCmd, CHAR *argv[], UINT32 *argc)
{
    *argc = 0;
    return WX_SUCCESS;
}

/* 澶勭悊DEBUG璇锋眰 */
UINT32 WX_DEBUG_ProcDbgReqMsg(WxDebug *this, WxMsg *msg)
{
    WxDebugReqMsg *dbgReqMsg = (WxDebugReqMsg *)msg->msgData;
    UINT32 ret = WX_Debug_Cmd2ArgList(&dbgReqMsg->dbgCmd, this->argv, &this->argc);
    if (ret != WX_SUCCESS) {
        return ret;
    }
    WX_Debug_ProcCmd(this->argv, this->argc);
	return WX_SUCCESS; /* WX_SUCCESS */

}

/* 妯″潡鏋勫缓鍑芥暟 */
UINT32 WX_DEBUG_Construct(VOID *module)
{
    WxDebug *this = WX_Mem_Alloc(WX_GetModuleName(module), 1, sizeof(WxDebug));
    if (this == NULL) {
        return WX_DEBUG_MALLOC_ERR;
    }
    this->moduleId = WX_GetModuleId(module);
    this->coreId = WX_GetModuleCoreId(module);
    /* 璁剧疆涓奙odule */
    WX_SetModuleInfo(module, this);
    return WX_SUCCESS;
}

/* 妯″潡鏋愭瀯鍑芥暟 */
UINT32 WX_DEBUG_Destruct(VOID *module)
{
    WxDebug *this = WX_GetModuleInfo(module);
    if (this == NULL) {
        return WX_SUCCESS;
    }
    WX_Mem_Free(this);
    WX_SetModuleInfo(module, NULL);
    return WX_SUCCESS;
}

/* 妯″潡娑堟伅澶勭悊鍏ュ彛 */
UINT32 WX_DEBUG_Entry(VOID *module, WxMsg *msg)
{
    WxDebug *this = WX_GetModuleInfo(module);
    switch (msg->msgType){
        case WX_MSG_TYPE_BEBUG_REQ: {
            return WX_DEBUG_ProcDbgReqMsg(this, msg);
        }
        /* if more please add here */
        default: {
            return WX_DEBUG_MSGTYPE_ERR;
        }
    }
}

