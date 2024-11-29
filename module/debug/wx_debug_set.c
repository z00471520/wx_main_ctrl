#include "wx_include.h"

WxDbgCmdItem g_wxDbgSetHelp[] = 
{
    {"?",           "Set help",             WX_Dbg_SetHelp},
    /* if more please add here */
};
VOID WX_Debug_SetHelp(CHAR *argv[], UINT32 argc) 
{
    WxDbgCmdItem *help = NULL;

    wx_show("DEBUG Set Help\n");
    wx_show("-----------------\n");
    wx_show("%-16s %s\n", "(Cmd)", "(Description)");
    for (UINT32 i = 0; i < sizeof(g_wxDbgSetHelp) / sizeof(g_wxDbgSetHelp[0]); i++) {
        help = &g_wxDbgSetHelp[i];
        wx_show("%-16s %s\n", help->helpItem, help->helpDesc);
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
        
        if (strcmp(itemName, handle->helpItem) != 0) {/* 找不到 */
            continue;
        }

        if (handle->helpFunc == NULL) {/* 不支持当前命令 */
            wx_show("<cmd> Set %s not support.",handle->helpItem);
            return;
        } 
        
        handle->helpFunc(&argv[1], argc - 1);
        return;
    }

    wx_show("(cmd) Set %s not exist.", argv[0]);/*没有找到匹配的handle*/
    return;
}
