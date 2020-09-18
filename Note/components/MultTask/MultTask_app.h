/************************ (C) COPYLEFT 2018 Merafour *************************
* File Name          : timtask_app.h
* Author             : Merafour
* Last Modified Date : 03/24/2020
* Description        : APP.
********************************************************************************
* https://merafour.blog.163.com
* merafour@163.com
* https://github.com/merafour
******************************************************************************/

#include <stdint.h>
#include "MultTask.h"

#ifndef  __TIM_TASK_APP__
#define  __TIM_TASK_APP__

#ifdef __cplusplus
 extern "C" {
#endif

#define   USB_DEBUG_VCI_ON     1 // USB本地 VCI调试功能打开
#undef    USB_DEBUG_VCI_ON

// 任务运行标志
static inline int8_t task_obd4g_is_run(void)
{
    // _task_queue 为OBD4G所在任务队列
    return _task_queue.is_run;
}


/*_____________________________________________________________ TASK ________________________________________________________________*/
//extern void Task_app_ccm3310s(const uint32_t _start_ms);
extern void Task_simple(const uint32_t _start_ms);
extern void Task_can(const uint32_t _start_ms);
extern void Task_ec20(const uint32_t _start_ms);
// 4G 断网重连
extern void Task_ReConnect(const uint32_t _start_ms);
// 关闭 USB设备
extern void Task_close_usb(const uint32_t _start_ms);
// 通讯灯
extern void Task_conn_led(const uint32_t _start_ms);
// 锁机
extern void Task_Lock_Machine(const uint32_t _start_ms);
// 秒滴答
extern void Task_sec_tick(const uint32_t _start_ms);

#ifdef __cplusplus
 }
#endif

#endif  //__TIM_TASK_APP__
