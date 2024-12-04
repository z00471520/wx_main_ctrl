#include "wx_include.h"
#include "wx_debug_set.h"

WxDbgCmdItem g_wxDbgSetHelp[] = 
{
    {"?",           "Set help",             WX_Debug_SetHelp},
    /* if more please add here */
};
VOID WX_Debug_SetHelp(CHAR *argv[], UINT32 argc) 
{
    wx_show("DEBUG Set Help\n");
    wx_show("-----------------\n");
    wx_show("%-16s %s\n", "(Cmd)", "(Description)");
    WxDbgCmdItem *item = NULL;
    for (UINT32 i = 0; i < sizeof(g_wxDbgSetHelp) / sizeof(g_wxDbgSetHelp[0]); i++) {
    	item = &g_wxDbgSetHelp[i];
        wx_show("%-16s %s\n", item->cmdName, item->helpDesc);
    }
}

VOID WX_Debug_Set(CHAR *argv[], UINT32 argc)
{
    if(argc == 0){
        WX_Debug_SetHelp(argv, argc);
        return;
    }

    CHAR *itemName = argv[0];
    WxDbgCmdItem *handle = NULL;

    for (UINT32 i= 0; i < sizeof(g_wxDbgSetHelp) / sizeof(g_wxDbgSetHelp[0]); i++) {
        handle = &g_wxDbgSetHelp[i];
        
        if (strcmp(itemName, handle->cmdName) != 0) {/* 鎵句笉鍒� */
            continue;
        }

        if (handle->cmdExecFunc == NULL) {/* 涓嶆敮鎸佸綋鍓嶅懡浠� */
            wx_show("<cmd> Set %s not support.",handle->cmdName);
            return;
        } 
        
        handle->cmdExecFunc(&argv[1], argc - 1);
        return;
    }

    wx_show("(cmd) Set %s not exist.", argv[0]);/*娌℃湁鎵惧埌鍖归厤鐨刪andle*/
    return;
}