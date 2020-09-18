/************************ (C) COPYLEFT 2018 Merafour *************************

* File Name          : task_simple.c
* Author             : Merafour
* Last Modified Date : 04/09/2020
* Description        : task_simple.
********************************************************************************
* https://merafour.blog.163.com
* merafour@163.com
* https://github.com/merafour
******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include "MultTask.h"
#include "MultTask_app.h"
#include "Periphs/uart.h"

extern uint32_t HAL_GetTick(void);
static uint32_t tick;

static int task_simple_loop(struct app_sub_task_loop* const _app)  // ��ʼ��
{
	//const char info[] = "task_simple_loop\r\n";
	uint32_t _tick = HAL_GetTick();
	//app_debug("[%s-%d] _app->count:%3d ... Tick:%d\r\n", __func__, __LINE__, _app->count, HAL_GetTick());
	app_debug("[%s-%d] _tick:%8d  delay:%5d\r\n", __func__, __LINE__, _tick, _tick-tick);
	//app_debug("[%s-%d] time:%5d  %s\r\n", __func__, __LINE__, time(NULL), time_format());
	tick = _tick;
	//uart1_send((const uint8_t*)info, sizeof(info));
	//��ȡGPS���ݣ������浽ָ���ڴ�
	//GpsData_Savebuff();
	//app_debug("[%s--%d] \r\n", __func__, __LINE__);
	return 0;
}

static struct app_task_block _task_simple = {
    .sp = 0, 
    //startΪ��һ��������� loop����
    .start = task_simple_loop,
    .stack = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    ._delay_ms = 0,
};

void Task_simple(const uint32_t _start_ms)
{
#if 0
    // ģ�庯����ԭʼ����,����
    struct app_sub_task_loop* const _task = app_obd4g_Get();
    if(_task_obd4g._delay_ms<_start_ms)
    {
        const uint8_t status = _task->status;
        _task->count++;
        _task_obd4g._delay_ms=_start_ms;
        // �ָ����˳�֮ǰ��״̬
        _task->status = _task->status&0x7F;     // ȥ�����λ��־λ
        _task->loop(_task);
        // ״̬���л����ѽ���������,�������
        if((status!=_task->status) || (_task!=app_obd4g_Get())) 
        {
            _task->count=0;
            _task->sub=0;
        }
    }
#endif
    //ʹ��ģ�庯��
    Task_Tiny(&_task_simple, _start_ms);
}

