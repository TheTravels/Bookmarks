/******************** (C) COPYRIGHT 2015 merafour ********************
* File Name          : mera_queue_template_data.h
* Author             : 冷月追风 && Merafour
* Version            : V1.0.0
* Last Modified Date : 09/18/2020
* Description        : 循环队列,C实现,仿 C++ 模板写法,数据结构部分.
* Reference          : https://blog.csdn.net/zoomdy/article/details/79242528.
********************************************************************************
* https://merafour.blog.163.com
* merafour@163.com
* https://github.com/merafour
*******************************************************************************/

#include <stddef.h>
#include <string.h>
#include <stdint.h>

#ifndef __MERA_QUEUE_TEMPTATE_DATE_H__
#define __MERA_QUEUE_TEMPTATE_DATE_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifdef MQ_LEN
#undef MQ_LEN
#endif

#ifdef MQT
#undef MQT
#endif

//struct mq_temp {
//		uint16_t Index_r;
//		uint16_t Index_w;
//		uint16_t Index_safe;
//		MQT _value[MQ_LEN];
//};
// 队列数据结构定义
#define mq_queue(name,mqt,mql) struct mq_##name { \
		uint16_t Index_r;\
		uint16_t Index_w;\
		uint16_t Index_safe;\
		mqt _value[mql];\
};

// 以下宏需要重新定义,注意宏的嵌套引用顺序
#define mq_func(name)	  mq_##name##_temp
#define MQ_LEN	 512
#define MQT      uint8_t
//mq_queue(temp,MQT,MQ_LEN);
#define mq_type struct mq_temp

// 取消宏定义
#undef mq_func
#undef MQ_LEN
#undef MQT
#undef mq_type

#ifdef __cplusplus
}
#endif

#endif // __MERA_QUEUE_TEMPTATE_DATE_H__
