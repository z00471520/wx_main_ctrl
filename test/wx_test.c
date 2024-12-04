
#include "wx_test.h"
#include "wx_rs422_master_wr_data_req_intf.h"
#include "wx_frame.h"
#include "sleep.h"
#include "wx_tasks.h"
VOID WX_TestHandle(VOID *param)
{
    usleep(1000000);

    do {
    	boot_debug("Test task start...");
    	WxTask *testTask = WX_QueryTask("task_test");
		if (testTask == NULL) {
			boot_debug("Test task not found");
			break;
		}

		WX_RegModuleRouter(WX_MODULE_TEST, 1, testTask, NULL);


		/* create a event message */
		WxRs422MasterWrDataReqMsg *msg = WX_ApplyEvtMsg(WX_MSG_TYPE_RS422_MASTER_WR_DATA_REQ);
		if (msg == NULL) {
			wx_excp_cnt(WX_EXCP_CREATE_EVENT_MSG_FAIL);
			boot_debug("WX_ApplyEvtMsg fail");
			return;
		}
		/* clear the message */
		WX_CLEAR_OBJ((WxMsg *)msg);
		/* init the message */
		msg->sender = WX_MODULE_TEST;
		msg->receiver = WX_MODULE_RS422_MASTER;
		msg->subMsgType = WX_RS422_MASTER_MSG_WRITE_MMM;
		/* set the message data */
		msg->wrData.MMM.data16 = 0x0001;
		msg->wrData.MMM.data32 = 0x12345678;
		msg->wrData.MMM.data8 = 0x12;
		msg->wrData.MMM.data64 = 0x123456789ABCDEF0;
		/* send msg to driver */
		UINT32 ret = WX_MsgShedule(msg->sender, msg->receiver, msg);
		if (ret != WX_SUCCESS) {
			wx_excp_cnt(WX_EXCP_SEND_MSG_FAIL);
			boot_debug("WX_MsgShedule fail(%u)", ret);
			WX_FreeEvtMsg((WxMsg **)&msg);
		}
    } while(0);

    boot_debug("Test task finish");
    vTaskDelete(NULL);
}
