/************************ (C) COPYLEFT 2018 Merafour *************************
* File Name          : MultTask.h
* Author             : Merafour
* Last Modified Date : 03/19/2020
* Description        : ������������ģ��.
* Description        : �ο�����Ƭ�����򹹼ܡ�(https://blog.csdn.net/yyz_1987/article/details/17961375).
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

// ����ģʽ��Ƶϵ��
#define  TIM_TASK_PERIOD  10
#undef   TIM_TASK_PERIOD
// TCB ����,�����������ж��ٸ�����
#define  TIM_TASK_NUMBER  32
// ����ú���ú���ָ��ģʽ,������� struct app_task_block�ṹģʽ
#define  MULTI_TASK_MODE_FUNC   1
//#undef   MULTI_TASK_MODE_FUNC
// ����������ջ���
#define TASK_BLOCK_STACK_DEPTH    10

 /*_____________________________________________________________ ������ƿ�--������ ________________________________________________________________*/
struct app_sub_task_loop{   // ������ѭ��,������
   uint8_t status;           // ״̬�л���, һ��������״̬,��дʹ�� sub��status���λ����������֮ǰ��״̬
   uint8_t sub;              // ��һ��״̬����Ҫ��ɶ������ʱʹ�� sub��Ϊ��״̬�л�
   int8_t err;               // ������룬 0�����, other������
   uint8_t count;            // ������,���ڴ���ִ�г�ʱ����,ע�����ڴ������ÿ���countӦ��Multi_Task�������д�������������ʱ����Ҫ�� loop������
   // loop���� 0��ʾ��������ִ�����,��ִ�г�ջ����,���˻���һ��������,ͨ������ -1����
   int (* loop)(struct app_sub_task_loop* const _app);     // ҵ��ѭ������,������ִ��һ�μ�����
};
struct app_task_block{  // ������ƿ�
   //struct app_sub_task_loop   start;        // ջ�׼���һ������
   int (* const start)(struct app_sub_task_loop* const _app);  // ջ�׼���һ������
   struct app_sub_task_loop stack[TASK_BLOCK_STACK_DEPTH];       // ������ջ,TASK_BLOCK_STACK_DEPTHΪջ��������
   uint8_t sp;                           // ջָ��
   uint8_t sleep;                        // ���߱�־	0: ��������,   1: ������
   uint32_t _delay_ms;                   // ���������ڲ�����ʱ����
};

//������нṹ
struct Multi_Task{
    struct {                    // ������ƿ�
        char 	  flagState;	// ���з�ʽ     0: ������,     1: ����		
        char 	  flagRun;	    // ���״̬	0: ���񻹲���ִ��,   1: ������Ҫִ��		
        char 	  flagType;	    // ����ʽ	0: ��������, 1: �жϴ���		
        //char 	  flagSleep;    // ���߱�־	0: ��������,   1: ������
        uint32_t  cntRun;	    // ���м���(��ʱ)��,���Ѳ��ü�ʱ��ʽ������������ʽ����ʹ��
        uint32_t  numCircle;	// ѭ��������
#ifdef MULTI_TASK_MODE_FUNC
        void (*loop)(const uint32_t _start_ms); //����, ע:�涨loopֻ���Է�������ʽ����
#else
        struct app_task_block* task;             //����
#endif
    }tcb[TIM_TASK_NUMBER];  //�������
    int8_t    is_run;       // ���������б�־,1:������пɼ�������,0:������н��˳�����.
};
 // ��ʼ��
extern void MTask_Init(struct Multi_Task* const _block);
// �������
#ifdef MULTI_TASK_MODE_FUNC
// ��ʱ�����������ַ�ʽ�������Ҫ��MTask_Update�����ӵ��ж�����ִ�е�����������������
// fsttim ��������ʱ��   cirtim ����ѭ��ʱ��          
extern int __MTask_Add(struct Multi_Task* const _block, const uint32_t fsttim, const uint32_t cirtim, void (*const _loop)(const uint32_t _start_ms), const uint8_t type);
//��������
// ����������������ַ�ʽ
static inline int MTask_Add(struct Multi_Task* const _block, const uint32_t fsttim, const uint32_t cirtim, void (*const _loop)(const uint32_t _start_ms))
{
   return __MTask_Add(_block, fsttim, cirtim, _loop, 0);
}
// ɾ������
extern void MTask_Del(struct Multi_Task* const _block, void (*const _loop)(const uint32_t _start_ms));
#else
extern int __MTask_Add(struct Multi_Task* const _block, const uint32_t fsttim, const uint32_t cirtim, struct app_task_block* const _task, const uint8_t type);
//��������
//extern int MTask_Add(struct Multi_Task* const _block, const uint32_t fsttim, const uint32_t cirtim, void (*const _loop)(const uint32_t _start_ms));
static inline int MTask_Add(struct Multi_Task* const _block, const uint32_t fsttim, const uint32_t cirtim, struct app_task_block* const _task)
{
   return __MTask_Add(_block, fsttim, cirtim, _task, 0);
}
// ɾ������
extern void MTask_Del(struct Multi_Task* const _block, struct app_task_block* const _task);
#endif
 // ��������
 extern void MTask_Update(struct Multi_Task* const _block);
 // ��������
 extern void MTask_Proc(struct Multi_Task* const _block);
// ����������б�־,�豸����ʱ�ñ�־������,����ִ���˳���ز���
static inline void MTask_set_run(struct Multi_Task* const _block, const int8_t is_run)
{
    _block->is_run = is_run;
}
static inline int8_t MTask_is_run(struct Multi_Task* const _block)
{
    return _block->is_run;
}
 // ����һ���������
 extern struct Multi_Task _task_queue;  // default, Ĭ�϶���
static inline int8_t _task_queue_is_run(void)
{
    return _task_queue.is_run;
}
 
 /*_____________________________________________________________ ������ƿ�--������ ________________________________________________________________*/

// �����µ������
//extern void task_block_Enter(struct app_task_block* const _block, struct app_sub_task_loop* const _app);
extern void __task_block_Enter(struct app_task_block* const _block, const uint8_t status, int (* const loop)(struct app_sub_task_loop* const _app));
static inline void task_block_Enter(struct app_task_block* const _block, int (* const loop)(struct app_sub_task_loop* const _app))
{
    __task_block_Enter(_block, 0, loop);
}
// ������һ�������
extern void task_block_Esc(struct app_task_block* const _block);
// ��ȡ��ǰ�����, ���� .stack[0] ������δ��ʼ���ģ�ֱ�ӷ��ʿ��ܳ���ʹ�� task_block_Get��ȡ��ǰ������ǰ�ȫ��
extern struct app_sub_task_loop* task_block_Get(struct app_task_block* const _block);
static inline void task_block_delay(struct app_task_block* const _block, const uint32_t _ms)   // ��ʱ����
{
    _block->_delay_ms += _ms;
}
 // ����,��������
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

/*_____________________________________________________________ ������ģ�庯�� ________________________________________________________________*/
static inline void Task_Tiny(struct app_task_block* const _block, const uint32_t _start_ms)
{
    struct app_sub_task_loop* const _task = task_block_Get(_block);
    if(_block->_delay_ms<_start_ms)
    {
        const uint8_t status = _task->status;
        _task->count++;
        _block->_delay_ms=_start_ms;
        // �ָ����˳�֮ǰ��״̬
        _task->status = _task->status&0x7F;     // ȥ�����λ��־λ
        _task->loop(_task);
        // ״̬���л����ѽ���������,�������
        if((status!=_task->status) || (_task!=task_block_Get(_block))) 
        {
            _task->count=0;
            _task->sub=0;
        }
    }
}

/*_____________________________________________________________ �ⲿ�ӿں��� ________________________________________________________________*/
extern uint32_t get_app_time_ms(void);  // ��ȡϵͳ����ʱ�� ms��,��ϵͳ�ṩ

#ifdef __cplusplus
 }
#endif

#endif  //__MULTI_TASK__
