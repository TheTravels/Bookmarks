/************************ (C) COPYLEFT 2018 Merafour *************************
* File Name          : MultTask.c
* Author             : Merafour
* Last Modified Date : 03/19/2020
* Description        : ������������ģ��.
* Description        : �ο�����Ƭ�����򹹼ܡ�(https://blog.csdn.net/yyz_1987/article/details/17961375).
********************************************************************************
* https://merafour.blog.163.com
* merafour@163.com
* https://github.com/merafour
******************************************************************************/

#include <stdio.h>
#include "MultTask.h"

// ����һ���������
struct Multi_Task _task_queue;  // default, Ĭ�϶���

/*_____________________________________________________________ ������ģ�� ________________________________________________________________*/
 void MTask_Init(struct Multi_Task* const _block)
 {
    int i;
    for (i=0; i<TIM_TASK_NUMBER; i++)
    {
#ifdef MULTI_TASK_MODE_FUNC
        _block->tcb[i].loop = NULL;
#else
        _block->tcb[i].task = NULL;
#endif
        _block->tcb[i].cntRun = 0;
        _block->tcb[i].numCircle = 0;
        _block->tcb[i].flagRun = 0;
        _block->tcb[i].flagState = 0;
    }
 }
#ifdef MULTI_TASK_MODE_FUNC
 // fsttim ��������ʱ��   cirtim ����ѭ��ʱ��
 int __MTask_Add(struct Multi_Task* const _block, const uint32_t fsttim, const uint32_t cirtim, void (*const _loop)(const uint32_t _start_ms), const uint8_t type)
#else
 int __MTask_Add(struct Multi_Task* const _block, const uint32_t fsttim, const uint32_t cirtim, struct app_task_block* const _task, const uint8_t type)
#endif
 {
    int i;
    int pos = -1;
    //����λ��
    for (i=0; i<TIM_TASK_NUMBER; i++)
    {
#ifdef MULTI_TASK_MODE_FUNC
        if (_loop == _block->tcb[i].loop)
#else
        if (_task == _block->tcb[i].task)
#endif
        {
            pos = i;
            break;
        }
        if ((pos == -1) && (NULL==_block->tcb[i].flagState))
        {
            pos = i;
        }
    }
    //��������
    if (pos == -1)
    {
        return -1;
    }
#ifdef MULTI_TASK_MODE_FUNC
    _block->tcb[pos].loop = _loop;
#else
    _block->tcb[pos].task = _task;
#endif
    //  ��ʱģʽ
    _block->tcb[pos].cntRun = get_app_time_ms() + fsttim;
    _block->tcb[pos].numCircle = cirtim;
    _block->tcb[pos].flagRun = 0;
    _block->tcb[pos].flagType = type;
    _block->tcb[pos].flagState = 1;

    return 0;
 }
 /*int MTask_Add(struct Multi_Task* const _block, const uint32_t fsttim, const uint32_t cirtim, void (*const _loop)(const uint32_t _start_ms))
 {
     return __MTask_Add(_block, fsttim, cirtim, _loop, 0);
 }*/
#ifdef MULTI_TASK_MODE_FUNC
 void MTask_Del(struct Multi_Task* const _block, void (*const _loop)(const uint32_t _start_ms))
#else
 void MTask_Del(struct Multi_Task* const _block, struct app_task_block* const _task)
#endif
 {
    int i;
    for (i=0; i<TIM_TASK_NUMBER; i++)
    {
#ifdef MULTI_TASK_MODE_FUNC
        if (_loop==_block->tcb[i].loop)
#else
        if (_task==_block->tcb[i].task)
#endif
        {
            _block->tcb[i].flagState = 0;
            _block->tcb[i].flagRun = 0;
            return;
        }
    }
 }
//��������
#pragma inline
void __MTask_Proc(struct app_task_block* const _task, const uint32_t _start_ms)
{
    struct app_sub_task_loop* const _sub_task = task_block_Get(_task);;
    if(_task->_delay_ms<_start_ms)
    {
        _task->_delay_ms=_start_ms;
        _sub_task->loop(_sub_task);
    }
}
// ���������ʱ��
 void MTask_Update(struct Multi_Task* const _block)
 {
    int i;
    const uint32_t _start_ms = get_app_time_ms();
    for (i=0; i<TIM_TASK_NUMBER; i++)
    {
        if (0 != _block->tcb[i].flagState) // ���з�ʽ     0: ������,     1: ����	
        {
            // ������ʱģʽ���׳�����ʹ��
            if ( (_block->tcb[i].cntRun>0) && (_block->tcb[i].cntRun<=_start_ms) )    // ��ʱʱ�䵽
            {
                _block->tcb[i].cntRun = 0; // ���м�ʱ������
                //�жϴ���λ��
#ifdef MULTI_TASK_MODE_FUNC
                if (_block->tcb[i].flagType != 0) // ����ʽ	0: ��������, 1: �жϴ���
                    (*_block->tcb[i].loop)(_start_ms);  // ����, ע:�涨loopֻ���Է�������ʽ����
                else
                    _block->tcb[i].flagRun = 1;  // ���״̬	0: �����ܱ�ִ��,   1: ������Ҫ��ִ��
#else
                if (_block->tcb[i].flagType != 0)
                    __MTask_Proc(_block->tcb[i].task, _start_ms);
                else
                    _block->tcb[i].flagRun = 1;
#endif
                //�ж�����,���¼�ʱ��
                if (_block->tcb[i].numCircle)
                    _block->tcb[i].cntRun = _start_ms + _block->tcb[i].numCircle;
                else
                    _block->tcb[i].flagState = 0;
            }
        }
    }
}
// ִ����Ҫִ�е�����
void MTask_Proc(struct Multi_Task* const _block)
{
    int i;
    const uint32_t _start_ms = get_app_time_ms();
    for (i=0; i<TIM_TASK_NUMBER; i++)
    {
        if (_block->tcb[i].flagRun != 0)
        {
#ifdef MULTI_TASK_MODE_FUNC
            _block->tcb[i].flagRun = 0;
            (*_block->tcb[i].loop)(_start_ms);
#else
            __MTask_Proc(_block->tcb[i].task, _start_ms);
#endif
        }
    }
}
/*_____________________________________________________________ ������ƿ�--������ ________________________________________________________________*/
// �����µ������
void __task_block_Enter(struct app_task_block* const _block, const uint8_t status, int (* const loop)(struct app_sub_task_loop* const _app))  
{
    // ��ȫ���
    //if(_block->sp < 0) _block->sp = 0;
    if(_block->sp > TASK_BLOCK_STACK_DEPTH) _block->sp = 0; 
    if(0==_block->sp) // ջָ�룬����ȳ�
    {
        _block->stack[_block->sp].loop = _block->start;     // ����ջ�׷�ֹ����
    }
    if (loop == _block->start)		//ջ�׺�����������ջ
    {
        _block->sp = 0;   // ������ֱ����ת��ջ��
    }
    _block->stack[++_block->sp].loop = loop;     // ����ջ��
    if(0x00==(0x80&_block->stack[_block->sp].status)) _block->stack[_block->sp].status = 0;
    if(status>0) _block->stack[_block->sp].status = status;   // ������ֵ
    _block->stack[_block->sp].count = 0;
    _block->stack[_block->sp].err = 0;
    _block->stack[_block->sp].sub = 0;
}
// ������һ�������
void task_block_Esc(struct app_task_block* const _block)
{
    //if(_block->sp < 0) _block->sp = 0;
    if(_block->sp > TASK_BLOCK_STACK_DEPTH) _block->sp = 0;
    if (0 != _block->sp)
    {
        _block->sp--;  // ������һ������
    }
    else
    {
        //_block->stack[_block->sp] = &_block->start;     // ����ջ��
    }
}
// ��ȡ��ǰ�����
struct app_sub_task_loop* task_block_Get(struct app_task_block* const _block)
{
    //if(_block->sp < 0) _block->sp = 0;
    if(_block->sp > TASK_BLOCK_STACK_DEPTH) _block->sp = 0;
    if(0==_block->sp) 
    {
        //return &_block->start;
        _block->stack[_block->sp].loop = _block->start; // ����ջ��
    }
    return &_block->stack[_block->sp];
}
