


WxDbgShowHandle g_wxDbgShowHandle[] = 
{
	{"cnt", 	"show cnt", WX_CNT_DbgShow},
	{"cancfg",	"xxxx",      NULL,}
	{"rs422cfg", }
};

... show ?

show spurr 如下命令
-------------
cmd         descpiton
cnt         show cnt
cancfg      show can cfg
?           show help


VOID WX_DebugShow(char *argv[], UINT32 argc)
{
    FOR () {
        if (STRCMP(argv[], g[i]) == 0) {
            if ( g[i].showFunc == NULL) {
                wx_show("the %s not hanle is null",argv[i]);
                return;
            }
            g[i].showFunc(argc[1], argc - 1);
        }
    }    
}

UINT32 WX_Str2U32Value(CHAR *str, UINT32 *value) {
    WX_SUCCESS;
    WX_XXXX;

}

/* dbgcmd board <param> core <param> [set|show|?] */
VOID WX_Degbug(char *argv[], UINT32 argc)
{
    strcmp("show") {

    }
    set xxx timer 10
    
    argv[i]  "65535"

    UINT32 value;
    
    65335 
    show xxx
    i    i+1

    WX_DebugShow(&argv[i+1], argc - i);
}

