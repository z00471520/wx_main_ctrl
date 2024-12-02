#include "wx_include.h"
#include "wx_debug_show.h"
#include "wx_log.h"
#include "wx_cnt.h"
WxDbgCmdItem g_wxDbgShowHelp[] = 
{
    {"?",           "Show help",             WX_Debug_ShowHelp},
	{"cnt", 	    "Show counter",          WX_Cnt_DbgShow},
    /* if more please add here */
};

VOID WX_Debug_ShowHelp(CHAR *argv[], UINT32 argc) 
{
    WxDbgCmdItem *help = NULL;

    wx_show("DEBUG Show Help\n");
    wx_show("-----------------\n");
    wx_show("%-16s %s\n", "(Cmd)", "(Description)");
    for (UINT32 i = 0; i < sizeof(g_wxDbgShowHelp) / sizeof(g_wxDbgShowHelp[0]); i++) {
        help = &g_wxDbgShowHelp[i];
        wx_show("%-16s %s\n", help->helpDesc, help->helpDesc);
    }
}

/* ShowCnt鍦╳x_cnt閲屽畾涔�*/
VOID WX_Debug_ShowCnt(CHAR *argv[], UINT32 argc)
{
    wx_show("Cnt");
}


/* argv[]鏄痵how涔嬪悗鐨勫瓙鏁扮粍 */
VOID WX_Debug_Show(CHAR *argv[], UINT32 argc)
{
    if(argc == 0){
        WX_Debug_ShowHelp(argv, argc);
        return;
    }
    
    CHAR *itemName = argv[0];
    WxDbgCmdItem *handle = NULL;

    for (UINT32 i= 0; i < sizeof(g_wxDbgShowHelp) / sizeof(g_wxDbgShowHelp[0]); i++) {
        handle = &g_wxDbgShowHelp[i];
        
        if (strcmp(itemName, handle->helpDesc) != 0) {
            continue;
        }

        if (handle->cmdExecFunc == NULL) {
            wx_show("<cmd> Show %s not support.",handle->helpDesc);
            return;
        } 
        
        handle->cmdExecFunc(&argv[1], argc - 1);
        return;
    }

    wx_show("(cmd) Show %s not exist.", argv[0]);/*娌℃湁鎵惧埌鍖归厤鐨刪andle*/
    return;
}
