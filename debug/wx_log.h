#ifndef WX_LOG_H
#define WX_LOG_H

/*
 * Alert: The device is experiencing a fatal anomaly, and the system is unable to recover to normal.
 * 		  The device must be restarted, such as due to program abnormalities causing device restart or memory usage errors.
 * Critical: The device is severely abnormal and immediate measures need to be taken, such as the device's memory usage
 *           reaching its limit, temperature exceeding the alarm line, etc.
 * Error: Incorrect operation or abnormal process of the device, which will not affect subsequent business, but requires
 *         attention and cause analysis, such as user error instructions, password errors, etc.
 * Warning: Abnormal points in device operation may cause business failures and require attention, such as routing process shutdown, protocol packet loss, etc.
 * Notice: Key operational information for the normal operation of the device, such as interface shutdown command, protocol state machine jump, etc.
 * Informational: General operational information for the normal operation of the device, such as the user executing the display command.
 * Debugging: Recording the internal operational status of a device, typically used for problem debugging.
 */
typedef enum {
	WX_DEBUG = 0, /* ecording the internal operational status of a device */
	WX_INFO = 1, /* General operational information for the normal operation of the device */
	WX_NOTICE = 2, /* Key operational information for the normal operation of the device, */
	WX_WARN = 3, /* Abnormal points in device operation may cause business failures and require attention */
	WX_ERROR = 4, /* Incorrect operation or abnormal process of the device, which will not affect subsequent business */
	WX_CRITICAL = 5, /* he device is severely abnormal and immediate measures need to be taken */
} WxLogLelevDef;
#define wx_log(lev,...)
#define wx_critical(excpCnt, ...)
#define wx_warning(excpCnt, ...)
#define wx_show(...) WX_UartPrint(...)
#endif
