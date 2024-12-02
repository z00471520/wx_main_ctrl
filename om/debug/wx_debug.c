#include "wx_include.h"
#include "wx_debug.h"
#include "wx_debug_req_intf.h"
/*  主板子类型  */
WxDbgBoardCore g_wxDbgBoard[] = 
{
	{"0", 	    "BOARD_MAIN_CTRL"},
    /* if more please add here */
	{"1",	    "BOARD_BUTT"}
    
};

/*  Core子类型  */
WxDbgBoardCore g_wxDbgCore[] = 
{
	{"0", 	    "CORE_ID_0"},
	{"1",	    "CORE_ID_1"}
    /* if more please add here */
};

/* 先定义 */
VOID WX_Debug_Help(CHAR *argv[], UINT32 argc);
VOID WX_Debug_Show(CHAR *argv[], UINT32 argc);
VOID WX_Debug_Set(CHAR *argv[], UINT32 argc);

/* 判断是否为数字字符串 */
BOOL WX_IsNumStr(CHAR *str) 
{
    while (*str) {
        if (!isdigit(*str)) {
            return 0;
        }
        str++;
    }
    return 1;
}

/* 字符串转uint32 */
UINT32 WX_Str2U32Value(CHAR *str, UINT32 *value) 
{   
    if (str[0] == '\0'){/* 空字符串 */
        return WX_DBG_CHECK_ID_EMPTY;
    }

    if (!WX_IsNumStr(str)) {/* 不是数字字符串  */
        return WX_DBG_Str2U32_ID_NOT_NUM;
    }

    CHAR *endptr;
    UINT32 errorno = 0; /* 错误号 */
    UINT32 temp = strtoul(str, &endptr, 0);

    if (*endptr != '\0') { /* 有额外非数字字符 */
        return WX_DBG_Str2U32_TRANS_FAIL;
    }
    
    if (temp > WX_U32_MAX) { /* 超出UINT32范围 */
        return WX_DBG_Str2U32_OUT_OF_RANGE;
    }

    *value = temp;

    return errorno;
}

/* 检查Id合法性 */
UINT32 WX_CheckId(CHAR *str, UINT32 *value) 
{    
    UINT32 ret = WX_Str2U32Value(str, value);

    if (ret != 0) {/*  转换失败  */
        wx_show("Error Exit: WX_CheckId %s fail(%u)", str, ret);
        return ret;
    }

    return ret;/*  转换成功 */
}

/* 检查调试命令格式 */
UINT32 WX_CheckDebugCmd(CHAR *argv[], UINT32 argc)
{
    /* 参数个数检查 */
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

/* 判断是否为当前设备 */
UINT32 WX_Debug_IsBoardCoreLocate(UINT32 boardId,UINT32 coreId)
{
    return 1;
}

/* handle 目录 */
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
    WxDbgCmdItem *help = NULL;

    wx_show("DEBUG Handle Help\n");
    wx_show("-----------------\n");
    wx_show("%-16s %s\n", "(Cmd)", "(Description)");
    for (UINT32 i = 0; i < sizeof(g_wxDbgItems) / sizeof(g_wxDbgItems[0]); i++) {
        help = &g_wxDbgItems[i];
        wx_show("%-16s %s\n", help->helpItem, help->helpDesc);
    }
}

/* 本地调试命令 cmd func */
VOID WX_Debug_ThisDevice(CHAR *argv[], UINT32 argc)
{
    WxDbgCmdItem *item = NULL;
    for (UINT32 i= 0; i < sizeof(g_wxDbgItems) / sizeof(g_wxDbgItems[0]); i++) {
        item = &g_wxDbgItems[i];
        if (strcmp(argv[0], item->cmdName) != 0) {
            continue;/* 循环直到找到匹配handle */
        }

        if (item->cmdExecFunc == NULL) { /* 当前handle无操作函数*/
            wx_show("<cmd> %s not support.", item->cmdName);
            return;
        }
        item->cmdExecFunc(&argv[1], argc - 1);
        return;
        
    }
    
    wx_show("(cmd) %s not exist.", argv[0]); /*没有找到匹配的handle*/
    return;
}

/* 远端调试命令 */
UINT32 WX_Debug_OtherDevice(CHAR *argv[], UINT32 argc)
{
    UINT32 ret;
    /* 申请消息 */
    
    /* 初始化消息头 */
    /* 填写消息信息 */
    /* 发送消息 */
    
    return ret;
}

VOID WX_Debug_ProcCmd(CHAR *argv[], UINT32 argc)
{
    /*  判断命令合法 */
    UINT32 ret = WX_CheckDebugCmd(argv, argc);
    if (ret != WX_SUCCESS) {
        wx_show("Error Exit: WX_CheckDebugCmd fail(%u)", ret);
        return;
    }

    /* 判断board和core Id*/
    UINT32 boardId, coreId;
    if (WX_CheckId(argv[WX_DBG_ARGV_BOARDID], &boardId)){
        return;
    }
    if (WX_CheckId(argv[WX_DBG_ARGV_COREID], &coreId)){
        return;
    }

    // [dbgcmd][board][x][core][y][cmd]
    /*  判断当前设备 */
    if (WX_Debug_IsBoardCoreLocate(boardId, coreId)) {
        WX_Debug_ThisDevice(&argv[WX_DBG_ARGV_HANDLE], argc - 5); /* 本地调试命令 */
    } else {
        WX_Debug_OtherDevice(argv, argc); /* 远端调试命令，非本设备 */
    }   

    return;
}

UINT32 WX_Debug_Cmd2ArgList(WxDbgCmd *dbgCmd, CHAR *argv[], UINT32 *argc)
{
    *argc = 0;
    return WX_SUCCESS;
}

/* 处理DEBUG请求 */
UINT32 WX_DEBUG_ProcDbgReqMsg(WxDebug *this, WxMsg *msg)
{
    WxDebugReqMsg *dbgReqMsg = (WxDebugReqMsg *)msg->msgData;
    UINT32 ret = WX_Debug_CmdStr2ArgList(&dbgReqMsg->dbgCmd, this->argv, &this->argc);
    if (ret != WX_SUCCESS) {
        return ret;
    }
    UINT32 ret = WX_Debug_ProcCmd(this->argv, this->argc);
	if (ret != WX_SUCCESS) {
		return ret; /* 错误码 */
	}
	
	return WX_SUCCESS; /* WX_SUCCESS */

}

/* 模块构建函数 */
UINT32 WX_DEBUG_Construct(VOID *module)
{
    UINT32 ret;
    WxDebug *this = WX_Mem_Alloc(WX_GetModuleName(module), 1, sizeof(WxDebug));
    if (this == NULL) {
        return WX_DEBUG_MALLOC_ERR;
    }
    this->moduleId = WX_GetModuleId(module);
    this->coreId = WX_GetCoreId(module);
    /* 设置上Module */
    WX_SetModuleInfo(module, this);
    return WX_SUCCESS;
}

/* 模块析构函数 */
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

/* 模块消息处理入口 */
UINT32 WX_DEBUG_Entry(VOID *module, WxMsg *msg)
{
    WxDebugInfo *this = WX_GetModuleInfo(module);
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

