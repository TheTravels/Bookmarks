/******************** (C) COPYRIGHT 2015 merafour ********************
* File Name          : mera_queue.c
* Author             : 冷月追风 && Merafour
* Version            : V1.0.0
* Last Modified Date : 09/18/2020
* Description        : 循环队列,模板操作C实现.
********************************************************************************
* https://merafour.blog.163.com
* merafour@163.com
* https://github.com/merafour
*******************************************************************************/
#include <stddef.h>
#include <string.h>
#include <stdint.h>
#include "mera_queue.h"
#include "mq_data_unit.h"

/*_____________________________________________________________ 队列模板使用范例 ________________________________________________________________*/
#ifndef  _MQ_UART_
#define  _MQ_UART_
// 使用模板队列, *.h 头文件部分,用于生成相应代码的声明或内联函数代码
#include "mera_queue/mera_queue_template_data.h"
// 以下宏重新定义,注意宏的嵌套引用顺序
// 定义接口函数名为:mq_uart_xxx
#define mq_func(name)	  mq_uart_##name
// 队列大小
#define MQ_LEN	 512
// 队列数据元素类型
#define MQT      uint8_t
mq_queue(uart,MQT,MQ_LEN);          // 此处进行宏展开,定义数据结构 struct mq_uart
// mq_type即为队列,在模板中使用的队列类型
#define mq_type struct mq_uart
#define MQ_NINLINE   // 不使用内联
#include "mera_queue/mera_queue_template.h"	 // 其内部会自动取消宏定义
#endif  // _MQ_UART_

// *.h && *.c 必须我完全一样,除了 mera_queue_template.h,mera_queue_template.c 
// 使用模板队列, *.c 源文件部分, 用于生成接口函数,非内联函数的同一操作代码不可重复生成
#include "mera_queue/mera_queue_template_data.h"
// 以下宏重新定义,注意宏的嵌套引用顺序
#define mq_func(name)	  mq_uart_##name
#define MQ_LEN	 512
#define MQT      uint8_t
//mq_queue(uart,MQT,MQ_LEN);          // 此处进行宏展开,定义数据结构 struct mq_uart,因上面已经生成了数据结构代码,不可重复生成
#define mq_type struct mq_uart
#define MQ_NINLINE   // 不使用内联
#include "mera_queue/mera_queue_template.c"	  // 其内部会自动取消宏定义

/*_____________________________________________________________ USART Queue ________________________________________________________________*/
// 使用模板队列, *.h 头文件部分,用于生成相应代码的声明或内联函数代码
#include "mera_queue_template_data.h"
// 以下宏重新定义,注意宏的嵌套引用顺序
// 定义接口函数名为:mq_usart_xxx
#define mq_func(name)	  mq_usart_##name
// 队列大小
#define MQ_LEN	 4096
// 队列数据元素类型
#define MQT      uint8_t
//mq_queue(usart,MQT,MQ_LEN);          // 此处进行宏展开,定义数据结构 struct mq_uart
// mq_type即为队列,在模板中使用的队列类型
#define mq_type struct mq_usart
#define MQ_NINLINE   // 不使用内联
#include "mera_queue_template.c"	 // 其内部会自动取消宏定义

/*_____________________________________________________________ EC20 Queue ________________________________________________________________*/
// 使用模板队列, *.h 头文件部分,用于生成相应代码的声明或内联函数代码
#include "mera_queue_template_data.h"
// 以下宏重新定义,注意宏的嵌套引用顺序
// 定义接口函数名为:mq_ec20_xxx
#define mq_func(name)	  mq_ec20_func(name)
// 队列大小
#define MQ_LEN	 MQ_EC20_LEN
// 队列数据元素类型
#define MQT      MQT_EC20//uint8_t
//mq_queue(ec20,MQT,MQ_LEN);          // 此处进行宏展开,定义数据结构 struct mq_usart
// mq_type即为队列,在模板中使用的队列类型
#define mq_type mq_ec20_type//struct mq_ec20
#ifdef  MQ_EC20_NINLINE
#define MQ_NINLINE   // 不使用内联
#endif
#include "mera_queue_template.c"	 // 其内部会自动取消宏定义

/*_____________________________________________________________ CAN Queue ________________________________________________________________*/
//#include "cache_queue_can_data.h"
// 使用模板队列
#include "mera_queue_template_data.h"
// 以下宏重新定义,注意宏的嵌套引用顺序
#define mq_func(name)	  mq_can_##name
#define MQ_LEN	 512
#define MQT      struct cache_can_rxbuf
//mq_queue(can,MQT,MQ_LEN);
#define mq_type struct mq_can
#define MQ_NINLINE   // 不使用内联
#include "mera_queue_template.c"	 // 其内部会自动取消宏定义
