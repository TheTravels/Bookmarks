/******************** (C) COPYRIGHT 2015 merafour ********************
* File Name          : mera_queue_template_data.h
* Author             : ����׷�� && Merafour
* Version            : V1.0.0
* Last Modified Date : 09/18/2020
* Description        : ѭ������,Cʵ��,�� C++ ģ��д��,���ݽṹ����.
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
// �������ݽṹ����
#define mq_queue(name,mqt,mql) struct mq_##name { \
		uint16_t Index_r;\
		uint16_t Index_w;\
		uint16_t Index_safe;\
		mqt _value[mql];\
};

// ���º���Ҫ���¶���,ע����Ƕ������˳��
#define mq_func(name)	  mq_##name##_temp
#define MQ_LEN	 512
#define MQT      uint8_t
//mq_queue(temp,MQT,MQ_LEN);
#define mq_type struct mq_temp

// ȡ���궨��
#undef mq_func
#undef MQ_LEN
#undef MQT
#undef mq_type

#ifdef __cplusplus
}
#endif

#endif // __MERA_QUEUE_TEMPTATE_DATE_H__
