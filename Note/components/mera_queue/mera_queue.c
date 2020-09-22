/******************** (C) COPYRIGHT 2015 merafour ********************
* File Name          : mera_queue.c
* Author             : ����׷�� && Merafour
* Version            : V1.0.0
* Last Modified Date : 09/18/2020
* Description        : ѭ������,ģ�����Cʵ��.
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

/*_____________________________________________________________ ����ģ��ʹ�÷��� ________________________________________________________________*/
#ifndef  _MQ_UART_
#define  _MQ_UART_
// ʹ��ģ�����, *.h ͷ�ļ�����,����������Ӧ�����������������������
#include "mera_queue/mera_queue_template_data.h"
// ���º����¶���,ע����Ƕ������˳��
// ����ӿں�����Ϊ:mq_uart_xxx
#define mq_func(name)	  mq_uart_##name
// ���д�С
#define MQ_LEN	 512
// ��������Ԫ������
#define MQT      uint8_t
mq_queue(uart,MQT,MQ_LEN);          // �˴����к�չ��,�������ݽṹ struct mq_uart
// mq_type��Ϊ����,��ģ����ʹ�õĶ�������
#define mq_type struct mq_uart
#define MQ_NINLINE   // ��ʹ������
#include "mera_queue/mera_queue_template.h"	 // ���ڲ����Զ�ȡ���궨��
#endif  // _MQ_UART_

// *.h && *.c ��������ȫһ��,���� mera_queue_template.h,mera_queue_template.c 
// ʹ��ģ�����, *.c Դ�ļ�����, �������ɽӿں���,������������ͬһ�������벻���ظ�����
#include "mera_queue/mera_queue_template_data.h"
// ���º����¶���,ע����Ƕ������˳��
#define mq_func(name)	  mq_uart_##name
#define MQ_LEN	 512
#define MQT      uint8_t
//mq_queue(uart,MQT,MQ_LEN);          // �˴����к�չ��,�������ݽṹ struct mq_uart,�������Ѿ����������ݽṹ����,�����ظ�����
#define mq_type struct mq_uart
#define MQ_NINLINE   // ��ʹ������
#include "mera_queue/mera_queue_template.c"	  // ���ڲ����Զ�ȡ���궨��

/*_____________________________________________________________ USART Queue ________________________________________________________________*/
// ʹ��ģ�����, *.h ͷ�ļ�����,����������Ӧ�����������������������
#include "mera_queue_template_data.h"
// ���º����¶���,ע����Ƕ������˳��
// ����ӿں�����Ϊ:mq_usart_xxx
#define mq_func(name)	  mq_usart_##name
// ���д�С
#define MQ_LEN	 4096
// ��������Ԫ������
#define MQT      uint8_t
//mq_queue(usart,MQT,MQ_LEN);          // �˴����к�չ��,�������ݽṹ struct mq_uart
// mq_type��Ϊ����,��ģ����ʹ�õĶ�������
#define mq_type struct mq_usart
#define MQ_NINLINE   // ��ʹ������
#include "mera_queue_template.c"	 // ���ڲ����Զ�ȡ���궨��

/*_____________________________________________________________ EC20 Queue ________________________________________________________________*/
// ʹ��ģ�����, *.h ͷ�ļ�����,����������Ӧ�����������������������
#include "mera_queue_template_data.h"
// ���º����¶���,ע����Ƕ������˳��
// ����ӿں�����Ϊ:mq_ec20_xxx
#define mq_func(name)	  mq_ec20_func(name)
// ���д�С
#define MQ_LEN	 MQ_EC20_LEN
// ��������Ԫ������
#define MQT      MQT_EC20//uint8_t
//mq_queue(ec20,MQT,MQ_LEN);          // �˴����к�չ��,�������ݽṹ struct mq_usart
// mq_type��Ϊ����,��ģ����ʹ�õĶ�������
#define mq_type mq_ec20_type//struct mq_ec20
#ifdef  MQ_EC20_NINLINE
#define MQ_NINLINE   // ��ʹ������
#endif
#include "mera_queue_template.c"	 // ���ڲ����Զ�ȡ���궨��

/*_____________________________________________________________ CAN Queue ________________________________________________________________*/
//#include "cache_queue_can_data.h"
// ʹ��ģ�����
#include "mera_queue_template_data.h"
// ���º����¶���,ע����Ƕ������˳��
#define mq_func(name)	  mq_can_##name
#define MQ_LEN	 512
#define MQT      struct cache_can_rxbuf
//mq_queue(can,MQT,MQ_LEN);
#define mq_type struct mq_can
#define MQ_NINLINE   // ��ʹ������
#include "mera_queue_template.c"	 // ���ڲ����Զ�ȡ���궨��
