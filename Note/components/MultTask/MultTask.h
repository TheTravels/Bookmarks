/************************ (C) COPYLEFT 2018 Merafour *************************
* File Name          : MultTask.h
* Author             : Merafour
* Last Modified Date : 03/19/2020
* Description        : 非阻塞多任务模型.
* Description        : 参考《单片机程序构架》(https://blog.csdn.net/yyz_1987/article/details/17961375).
********************************************************************************
* https://merafour.blog.163.com
* merafour@163.com
* https://github.com/merafour
******************************************************************************/

#include <stdint.h>

#ifndef  __MULTI_TASK__
#define  __MULTI_TASK__

#ifdef __cplusplus
 extern "C" {
#endif

// 计数模式分频系数
#define  TIM_TASK_PERIOD  10
#undef   TIM_TASK_PERIOD
// TCB 数量,即多多可以运行多少个任务
#define  TIM_TASK_NUMBER  32
// 定义该宏采用函数指针模式,否则采用 struct app_task_block结构模式
#define  MULTI_TASK_MODE_FUNC   1
//#undef   MULTI_TASK_MODE_FUNC
// 子任务的最大栈深度
#define TASK_BLOCK_STACK_DEPTH    10

 /*_____________________________________________________________ 任务控制块--子任务 ________________________________________________________________*/
struct app_sub_task_loop{   // 子任务循环,非阻塞
   uint8_t status;           // 状态切换用, 一个完整的状态,读写使用 sub。status最高位可用来保存之前的状态
   uint8_t sub;              // 当一个状态中需要完成多个步骤时使用 sub作为子状态切换
   int8_t err;               // 错误代码， 0：完成, other：出错
   uint8_t count;            // 计数器,用于代码执行超时控制,注：出于代码重用考虑count应该Multi_Task任务函数中处理，进入子任务时才需要在 loop中清零
   // loop返回 0表示该子任务执行完成,将执行出栈操作,即退回上一个子任务,通常返回 -1即可
   int (* loop)(struct app_sub_task_loop* const _app);     // 业务循环函数,非阻塞执行一次即返回
};
struct app_task_block{  // 任务控制块
   //struct app_sub_task_loop   start;        // 栈底即第一个任务
   int (* const start)(struct app_sub_task_loop* const _app);  // 栈底即第一个任务
   struct app_sub_task_loop stack[TASK_BLOCK_STACK_DEPTH];       // 子任务栈,TASK_BLOCK_STACK_DEPTH为栈的最大深度
   uint8_t sp;                           // 栈指针
   uint8_t sleep;                        // 休眠标志	0: 不能休眠,   1: 可休眠
   uint32_t _delay_ms;                   // 用于任务内部的延时控制
};

//任务队列结构
struct Multi_Task{
    struct {                    // 任务控制块
        char 	  flagState;	// 运行方式     0: 无任务,     1: 运行		
        char 	  flagRun;	    // 完成状态	0: 任务还不能执行,   1: 任务需要执行		
        char 	  flagType;	    // 处理方式	0: 主任务处理, 1: 中断处理		
        //char 	  flagSleep;    // 休眠标志	0: 不能休眠,   1: 可休眠
        uint32_t  cntRun;	    // 运行计数(计时)器,现已采用计时方式工作，计数方式不再使用
        uint32_t  numCircle;	// 循环计数器
#ifdef MULTI_TASK_MODE_FUNC
        void (*loop)(const uint32_t _start_ms); //任务, 注:规定loop只能以非阻塞方式工作
#else
        struct app_task_block* task;             //任务
#endif
    }tcb[TIM_TASK_NUMBER];  //任务队列
    int8_t    is_run;       // 多任务运行标志,1:任务队列可继续运行,0:任务队列将退出运行.
};
 // 初始化
extern void MTask_Init(struct Multi_Task* const _block);
// 添加任务
#ifdef MULTI_TASK_MODE_FUNC
// 暂时不用上面这种方式添加任务，要是MTask_Update函数加到中断里面执行到有锁的任务会出问题
// fsttim 任务启动时间   cirtim 任务循环时间          
extern int __MTask_Add(struct Multi_Task* const _block, const uint32_t fsttim, const uint32_t cirtim, void (*const _loop)(const uint32_t _start_ms), const uint8_t type);
//内联函数
// 添加任务用以下这种方式
static inline int MTask_Add(struct Multi_Task* const _block, const uint32_t fsttim, const uint32_t cirtim, void (*const _loop)(const uint32_t _start_ms))
{
   return __MTask_Add(_block, fsttim, cirtim, _loop, 0);
}
// 删除任务
extern void MTask_Del(struct Multi_Task* const _block, void (*const _loop)(const uint32_t _start_ms));
#else
extern int __MTask_Add(struct Multi_Task* const _block, const uint32_t fsttim, const uint32_t cirtim, struct app_task_block* const _task, const uint8_t type);
//内联函数
//extern int MTask_Add(struct Multi_Task* const _block, const uint32_t fsttim, const uint32_t cirtim, void (*const _loop)(const uint32_t _start_ms));
static inline int MTask_Add(struct Multi_Task* const _block, const uint32_t fsttim, const uint32_t cirtim, struct app_task_block* const _task)
{
   return __MTask_Add(_block, fsttim, cirtim, _task, 0);
}
// 删除任务
extern void MTask_Del(struct Multi_Task* const _block, struct app_task_block* const _task);
#endif
 // 更新任务
 extern void MTask_Update(struct Multi_Task* const _block);
 // 处理任务
 extern void MTask_Proc(struct Multi_Task* const _block);
// 任务队列运行标志,设备休眠时该标志被设置,任务将执行退出相关操作
static inline void MTask_set_run(struct Multi_Task* const _block, const int8_t is_run)
{
    _block->is_run = is_run;
}
static inline int8_t MTask_is_run(struct Multi_Task* const _block)
{
    return _block->is_run;
}
 // 定义一个任务队列
 extern struct Multi_Task _task_queue;  // default, 默认定义
static inline int8_t _task_queue_is_run(void)
{
    return _task_queue.is_run;
}
 
 /*_____________________________________________________________ 任务控制块--子任务 ________________________________________________________________*/

// 进入新的任务快
//extern void task_block_Enter(struct app_task_block* const _block, struct app_sub_task_loop* const _app);
extern void __task_block_Enter(struct app_task_block* const _block, const uint8_t status, int (* const loop)(struct app_sub_task_loop* const _app));
static inline void task_block_Enter(struct app_task_block* const _block, int (* const loop)(struct app_sub_task_loop* const _app))
{
    __task_block_Enter(_block, 0, loop);
}
// 返回上一个任务快
extern void task_block_Esc(struct app_task_block* const _block);
// 获取当前任务块, 由于 .stack[0] 可能是未初始化的，直接访问可能出错，使用 task_block_Get获取当前任务块是安全的
extern struct app_sub_task_loop* task_block_Get(struct app_task_block* const _block);
static inline void task_block_delay(struct app_task_block* const _block, const uint32_t _ms)   // 延时函数
{
    _block->_delay_ms += _ms;
}
 // 休眠,内联函数
static inline void task_sleep(struct app_task_block* const _block, const uint8_t sleep)
{
    //_block->sleep = sleep;
    _block->sleep = 0;
    if(sleep>0) _block->sleep = 1;
}
static inline uint8_t task_is_sleep(struct app_task_block* const _block)
{
    return (_block->sleep&0x01);
}

/*_____________________________________________________________ 任务处理模板函数 ________________________________________________________________*/
static inline void Task_Tiny(struct app_task_block* const _block, const uint32_t _start_ms)
{
    struct app_sub_task_loop* const _task = task_block_Get(_block);
    if(_block->_delay_ms<_start_ms)
    {
        const uint8_t status = _task->status;
        _task->count++;
        _block->_delay_ms=_start_ms;
        // 恢复到退出之前的状态
        _task->status = _task->status&0x7F;     // 去掉最高位标志位
        _task->loop(_task);
        // 状态已切换或已进入子任务,清除计数
        if((status!=_task->status) || (_task!=task_block_Get(_block))) 
        {
            _task->count=0;
            _task->sub=0;
        }
    }
}

/*_____________________________________________________________ 外部接口函数 ________________________________________________________________*/
extern uint32_t get_app_time_ms(void);  // 获取系统启动时间 ms数,由系统提供

#ifdef __cplusplus
 }
#endif

#endif  //__MULTI_TASK__
