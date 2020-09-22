/******************** (C) COPYRIGHT 2015 merafour ********************
* File Name          : mera_queue.h
* Author             : ����׷�� && Merafour
* Version            : V1.0.0
* Last Modified Date : 09/18/2020
* Description        : ѭ������,Cʵ��.
********************************************************************************
* https://merafour.blog.163.com
* merafour@163.com
* https://github.com/merafour
*******************************************************************************/

#include <stddef.h>
#include <string.h>
#include <stdint.h>

#ifndef __MERA_QUEUE_H__
#define __MERA_QUEUE_H__

#ifdef __cplusplus
extern "C" {
#endif

/*_____________________________________________________________ ��ͳ Cʵ�ֶ��� ________________________________________________________________*/
/*
* ���ݽ��չؼ����㣬bufҪС������Ҫ�죬ʵ�� 2017�ֽ� 200msƵ���շ��������� 445757�ֽ������ݶ�ʧ��
 */
#define MQ_LEN    (512+1024*4)
typedef struct{
		uint16_t Index_r;
		uint16_t Index_w;
		uint16_t Index_safe;
		//uint16_t w_tmp;
		//uint16_t r_tmp;
		uint8_t _value[MQ_LEN];
}mera_queue;
#define mq_type mera_queue
/*_____________________________________________________________ API ________________________________________________________________*/
static __inline void mq_init(mq_type* const _mq)
{ 
		memset(_mq, 0, sizeof(mq_type)); 
		_mq->Index_w = 0; 
		_mq->Index_r = MQ_LEN-1;
}
/*
 * mera_queue1: size = index_w - index_r - 1;
 *           |-----------------|
 *           | empty           |
 * index_w   | empty           |
 *           |                 |
 *        ...| data            |
 *           |                 |
 * index_r   | empty           |
 *           |                 |
 *        ...| empty           |
 *           |                 |
 *           |------start------|
 * mera_queue2: size = index_w + SIZE - index_r - 1;
 *           |-----------------|
 *           | data            |
 * index_r   | empty           |
 *           |                 |
 *        ...| empty           |
 *           |                 |
 * index_w   | empty           |
 *           |                 |
 *        ...| data            |
 *           |                 |
 *           |------start------|
 */
// �жϻ����Ƿ�Ϊ��
static __inline uint16_t mq_isempty(const mq_type* const _mq)
{
	uint16_t index_r;
	index_r = _mq->Index_r+1;
	if(index_r>=MQ_LEN) index_r=0;
	if(index_r == _mq->Index_w) return 1;  /* empty */
	return 0;
}
// ��ȡ���������ݳ���
static __inline uint16_t mq_size(const mq_type* const _mq)
{
	uint16_t index_r;
	uint32_t index_w = _mq->Index_w;
	index_r = _mq->Index_r+1;
	if(index_r>=MQ_LEN) index_r=0;
	if(index_w>=index_r) return (index_w-index_r); // ���ݳ���
	// ѭ������, index_w<r_tmp
	else
	{
		return (index_w+MQ_LEN-index_r);
	}
}
/*_____________________________________________________________ API write ________________________________________________________________*/
static __inline int mq_write(mq_type* const _mq, const uint8_t _byte)  
{ 
			if(_mq->Index_w != _mq->Index_r)_mq->_value[_mq->Index_w++] = _byte; 
			if(_mq->Index_w>=MQ_LEN) _mq->Index_w=0; 
			return 1;
}
static __inline int mq_write_cover(mq_type* const _mq, const uint8_t _byte) 
{ 
		uint16_t index_w = _mq->Index_w;
		uint16_t index_r = _mq->Index_r;
		if(index_w != index_r)_mq->_value[index_w++] = _byte;
		else 
		{
				index_r++; 
				if(index_r>=MQ_LEN) index_r=0; 
				_mq->_value[index_w++] = _byte;
		} 
		if(index_w>=MQ_LEN) index_w=0;
		_mq->Index_r = index_r;		
		_mq->Index_w = index_w; 
		return 1;
} 

static __inline int mq_writes(mq_type* const _mq, const uint8_t _byte[], const uint16_t _size)  
{ 
		uint16_t len;
		uint16_t index_w = _mq->Index_w;
		/*volatile*/ const uint16_t index_r = _mq->Index_r;
		//index_w = _queue->Index_w;
		for(len=0; len<_size; len++)
		{
				if(index_r == index_w) break;
				_mq->_value[index_w++] = _byte[len]; 
				if(index_w>=MQ_LEN) index_w=0; 
		}		
		_mq->Index_w = index_w;
		return len;
}
static __inline int mq_writes_cover(mq_type* const _mq, const uint8_t _byte[], const uint16_t _size)  
{ 
		uint16_t len;
		uint16_t index_w = _mq->Index_w;
		uint16_t index_r = _mq->Index_r;
		//index_w = _queue->Index_w;
		for(len=0; len<_size; len++)
		{
				if(index_r != index_w) _mq->_value[index_w++] = _byte[len];
				else 
				{
					index_r++; 
					if(index_r>=MQ_LEN) index_r=0; 
					_mq->_value[index_w++] = _byte[len];
				}
				if(index_w>=MQ_LEN) index_w=0; 
		}
		_mq->Index_r = index_r;		
		_mq->Index_w = index_w;
		return len;
}
/*_____________________________________________________________ API read ________________________________________________________________*/
static __inline int mq_read(mq_type* _mq, uint8_t* _byte)
{
		int ret=-1;
	  uint16_t r_tmp;
		//__asm("CPSID  I");
		r_tmp = _mq->Index_r+1;
		if(r_tmp>=MQ_LEN) r_tmp=0;
		if(r_tmp != _mq->Index_w)
		{
				*_byte = _mq->_value[r_tmp];
				_mq->Index_r = r_tmp;
				ret = 0;
		}
		//__asm("CPSIE  I");
		return ret;
}
static __inline uint16_t mq_reads(mq_type* _mq, uint8_t buf[], const uint16_t buf_size)
{
		uint16_t index=0;
	  uint16_t index_r = _mq->Index_r;
		const uint16_t index_w = _mq->Index_w;
		memset(buf, 0, buf_size);
		for(index=0; index<buf_size; index++)
		{
				index_r++;
				if(index_r>=MQ_LEN) index_r=0;
				if(index_w == index_r) break;  // empty
				_mq->Index_r = index_r;
				buf[index] = _mq->_value[index_r];
		}
		return index;
}
/*_____________________________________________________________ API safe ________________________________________________________________*/
static void mq_init_safe(mq_type* const _mq)
{ 
	_mq->Index_safe = _mq->Index_r;
}
static __inline uint16_t mq_reads_safe(mq_type* const _mq, uint8_t buf[], const uint16_t _size)  
{
		uint16_t index;
		uint16_t index_r = _mq->Index_safe;
		const uint16_t index_w = _mq->Index_w;
		//index_r = _queue->Index_safe;
		//memset(buf, 0, _size); 
		for(index=0; index<_size; index++) 
		{ 
				index_r++;
				if(index_r>=MQ_LEN) index_r=0; 
				if(index_w == index_r) break;  /* empty */ 
				_mq->Index_safe = index_r;
				buf[index] = _mq->_value[index_r];  
		}
		return index;
}
// delete a _size member
static __inline void mq_safe_del(mq_type* const _mq, const uint16_t _size)
{
	uint16_t index;
	uint16_t index_r = _mq->Index_r;
	const uint16_t index_w = _mq->Index_w;
	for(index=0; index<_size; index++)
	{
		index_r++;
		if(index_r>=MQ_LEN) index_r=0;
		if(index_w == index_r) break;  /* empty */
		_mq->Index_r = index_r;
	}
}

/*_____________________________________________________________ ����� ________________________________________________________________*/
#define macro_mq_write(_byte,_mq)  do{ \
			if(_mq.Index_w != _mq.Index_r) _mq._value[_mq.Index_w++] = _byte; \
			if(_mq.Index_w>=(sizeof(_mq._value)/sizeof(_mq._value[0])) ) _mq.Index_w=0; \
}while(0);
#define macro_mq_write_cover(_byte,_mq)  do{ \
			if(_mq.Index_w != _mq.Index_r) _mq._value[_mq.Index_w++] = _byte; \
			else {_mq.Index_r++; if(_mq.Index_r>=(sizeof(_mq._value)/sizeof(_mq._value[0])) ) _mq.Index_r=0; _mq._value[_mq.Index_w++] = _byte;} \
			if(_mq.Index_w>=(sizeof(_mq._value)/sizeof(_mq._value[0])) ) _mq.Index_w=0; \
}while(0);
#define macro_mq_reads(index,buf,_size,_mq)  do{ \
			/*memset(buf, 0, _size);*/ \
			for(index=0; index<_size; index++) \
			{ \
				    uint16_t index_r; \
					index_r = _mq.Index_r+1; \
					if(index_r>=(sizeof(_mq._value)/sizeof(_mq._value[0])) ) index_r=0; \
					if(index_r == _mq.index_w) break;  /* empty */ \
					buf[index] = _mq._value[index_r]; \
					_mq.Index_r = index_r; \
			} \
}while(0);
#define macro_mq_reads_safe(index,buf,_size,_mq)  do{ \
			/*memset(buf, 0, _size);*/ \
			for(index=0; index<_size; index++) \
			{ \
				    uint16_t index_r; \
					index_r = _mq.Index_safe+1; \
					if(index_r>=(sizeof(_mq._value)/sizeof(_mq._value[0]))) index_r=0; \
					if(index_r == _mq.index_w) break;  /* empty */ \
					buf[index] = _mq._value[index_r]; \
					_mq.Index_safe = index_r; \
			} \
}while(0);

//#undef mq_func
#undef mq_type
#undef MQ_LEN
//#undef MQT

/*_____________________________________________________________ ʹ��ģ��ʵ�� C���� ________________________________________________________________*/

/*_____________________________________________________________ USART Queue ________________________________________________________________*/
#ifndef  _MQ_USART_
#define  _MQ_USART_
// ʹ��ģ�����, *.h ͷ�ļ�����,����������Ӧ�����������������������
#include "mera_queue_template_data.h"
// ���º����¶���,ע����Ƕ������˳��
// ����ӿں�����Ϊ:mq_usart_xxx
#define mq_func(name)	  mq_usart_##name
// ���д�С
#define MQ_LEN	 (4096+128)
// ��������Ԫ������
#define MQT      uint8_t
mq_queue(usart,MQT,MQ_LEN);          // �˴����к�չ��,�������ݽṹ struct mq_usart
// mq_type��Ϊ����,��ģ����ʹ�õĶ�������
#define mq_type struct mq_usart
#define MQ_NINLINE   // ��ʹ������
#include "mera_queue_template.h"	 // ���ڲ����Զ�ȡ���궨��
#endif  // _MQ_USART_

/*_____________________________________________________________ EC20 Queue ________________________________________________________________*/
#ifndef  _MQ_EC20_
#define  _MQ_EC20_
#define mq_ec20_func(name)	  mq_ec20_##name
#define MQ_EC20_LEN    (1024*24-6)  // 24K
#define MQT_EC20        uint8_t
#define mq_ec20_type struct mq_ec20
#define MQ_EC20_NINLINE   // ��ʹ������
// ʹ��ģ�����, *.h ͷ�ļ�����,����������Ӧ�����������������������
#include "mera_queue_template_data.h"
// ���º����¶���,ע����Ƕ������˳��
// ����ӿں�����Ϊ:mq_ec20_xxx
#define mq_func(name)	  mq_ec20_func(name)
// ���д�С
#define MQ_LEN	 MQ_EC20_LEN
// ��������Ԫ������
#define MQT      MQT_EC20//uint8_t
mq_queue(ec20,MQT,MQ_LEN);          // �˴����к�չ��,�������ݽṹ struct mq_usart
// mq_type��Ϊ����,��ģ����ʹ�õĶ�������
#define mq_type mq_ec20_type//struct mq_ec20
#ifdef  MQ_EC20_NINLINE
#define MQ_NINLINE   // ��ʹ������
#endif
#include "mera_queue_template.h"	 // ���ڲ����Զ�ȡ���궨��
#endif  // _MQ_EC20_

/*_____________________________________________________________ CAN Queue ________________________________________________________________*/
#ifndef  _MQ_CAN_
#define  _MQ_CAN_
#include "mq_data_unit.h"
// ʹ��ģ�����
#include "mera_queue_template_data.h"
// ���º����¶���,ע����Ƕ������˳��
#define mq_func(name)	  mq_can_##name
#define MQ_LEN	 512
#define MQT      struct cache_can_rxbuf
mq_queue(can,MQT,MQ_LEN);
#define mq_type struct mq_can
#define MQ_NINLINE   // ��ʹ������
#include "mera_queue_template.h"
#endif  // _MQ_CAN_


#ifdef __cplusplus
}
#endif

#endif // __MERA_QUEUE_H__
