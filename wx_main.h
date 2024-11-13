
#ifndef WX_MAIN_CTRL_CPU0_H
#define WX_MAIN_CTRL_CPU0_H

/* main ctrl info */
typedef struct {
	INTC intCtrlInst; /* Instance of the Interrupt Controller */
	WxRs422Devices rs422Devices; /* RS422 设备列表 */
	WxTaskList taskList; /* 任务列表 */
} WxMainCtrlCpu0Info;



#endif
