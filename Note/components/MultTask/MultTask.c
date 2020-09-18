/************************ (C) COPYLEFT 2018 Merafour *************************
* File Name          : MultTask.c
* Author             : Merafour
* Last Modified Date : 03/19/2020
* Description        : 非阻塞多任务模型.
* Description        : 参考《单片机程序构架》(https://blog.csdn.net/yyz_1987/article/details/17961375).
********************************************************************************
* https://merafour.blog.163.com
* merafour@163.com
* https://github.com/merafour
******************************************************************************/

#include <stdio.h>
#include "MultTask.h"

// 定义一个任务队列
struct Multi_Task _task_queue;  // default, 默认定义

/*_____________________________________________________________ 多任务模型 ________________________________________________________________*/
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
 // fsttim 任务启动时间   cirtim 任务循环时间
 int __MTask_Add(struct Multi_Task* const _block, const uint32_t fsttim, const uint32_t cirtim, void (*const _loop)(const uint32_t _start_ms), const uint8_t type)
#else
 int __MTask_Add(struct Multi_Task* const _block, const uint32_t fsttim, const uint32_t cirtim, struct app_task_block* const _task, const uint8_t type)
#endif
 {
    int i;
    int pos = -1;
    //查找位置
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
    //任务已满
    if (pos == -1)
    {
        return -1;
    }
#ifdef MULTI_TASK_MODE_FUNC
    _block->tcb[pos].loop = _loop;
#else
    _block->tcb[pos].task = _task;
#endif
    //  计时模式
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
//内联函数
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
// 更新任务计时器
 void MTask_Update(struct Multi_Task* const _block)
 {
    int i;
    const uint32_t _start_ms = get_app_time_ms();
    for (i=0; i<TIM_TASK_NUMBER; i++)
    {
        if (0 != _block->tcb[i].flagState) // 运行方式     0: 无任务,     1: 运行	
        {
            // 减数计时模式容易出错，不使用
            if ( (_block->tcb[i].cntRun>0) && (_block->tcb[i].cntRun<=_start_ms) )    // 计时时间到
            {
                _block->tcb[i].cntRun = 0; // 运行计时器重置
                //判断处理位置
#ifdef MULTI_TASK_MODE_FUNC
                if (_block->tcb[i].flagType != 0) // 处理方式	0: 主任务处理, 1: 中断处理
                    (*_block->tcb[i].loop)(_start_ms);  // 任务, 注:规定loop只能以非阻塞方式工作
                else
                    _block->tcb[i].flagRun = 1;  // 完成状态	0: 任务不能被执行,   1: 任务需要被执行
#else
                if (_block->tcb[i].flagType != 0)
                    __MTask_Proc(_block->tcb[i].task, _start_ms);
                else
                    _block->tcb[i].flagRun = 1;
#endif
                //判断重载,更新计时器
                if (_block->tcb[i].numCircle)
                    _block->tcb[i].cntRun = _start_ms + _block->tcb[i].numCircle;
                else
                    _block->tcb[i].flagState = 0;
            }
        }
    }
}
// 执行需要执行的任务
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
/*_____________________________________________________________ 任务控制块--子任务 ________________________________________________________________*/
// 进入新的任务块
void __task_block_Enter(struct app_task_block* const _block, const uint8_t status, int (* const loop)(struct app_sub_task_loop* const _app))  
{
    // 安全检查
    //if(_block->sp < 0) _block->sp = 0;
    if(_block->sp > TASK_BLOCK_STACK_DEPTH) _block->sp = 0; 
    if(0==_block->sp) // 栈指针，后进先出
    {
        _block->stack[_block->sp].loop = _block->start;     // 保存栈底防止出错
    }
    if (loop == _block->start)		//栈底函数重置整个栈
    {
        _block->sp = 0;   // 不允许直接跳转到栈底
    }
    _block->stack[++_block->sp].loop = loop;     // 保存栈底
    if(0x00==(0x80&_block->stack[_block->sp].status)) _block->stack[_block->sp].status = 0;
    if(status>0) _block->stack[_block->sp].status = status;   // 给定初值
    _block->stack[_block->sp].count = 0;
    _block->stack[_block->sp].err = 0;
    _block->stack[_block->sp].sub = 0;
}
// 返回上一个任务块
void task_block_Esc(struct app_task_block* const _block)
{
    //if(_block->sp < 0) _block->sp = 0;
    if(_block->sp > TASK_BLOCK_STACK_DEPTH) _block->sp = 0;
    if (0 != _block->sp)
    {
        _block->sp--;  // 返回上一个任务
    }
    else
    {
        //_block->stack[_block->sp] = &_block->start;     // 保存栈底
    }
}
// 获取当前任务块
struct app_sub_task_loop* task_block_Get(struct app_task_block* const _block)
{
    //if(_block->sp < 0) _block->sp = 0;
    if(_block->sp > TASK_BLOCK_STACK_DEPTH) _block->sp = 0;
    if(0==_block->sp) 
    {
        //return &_block->start;
        _block->stack[_block->sp].loop = _block->start; // 保存栈底
    }
    return &_block->stack[_block->sp];
}
