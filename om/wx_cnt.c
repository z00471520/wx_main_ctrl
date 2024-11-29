#include "wx_include.h"
#include "wx_cnt_norm.h"
#include "wx_cnt_excp.h"
/* cnt */
VOID WX_Cnt_DbgShow(char *argv[], UINT32 argc)
{
    if (argc == 0){
        wx_show("<cmd> [excp|norm].");
        return;
    }

    if ((strcmp("norm", argv[0]) != 0)) {
        WX_NormCnt_DbgShow()
    } else if ((strcmp("excp", argv[0]) != 0)) {
        WX_ExcpCnt_DbgShow()
    } else {
        wx_show("<cmd> [excp|norm].");
    }
    return;
}