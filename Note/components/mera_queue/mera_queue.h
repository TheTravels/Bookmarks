/******************** (C) COPYRIGHT 2015 merafour ********************
* File Name          : mera_queue.h
* Author             : 冷月追风 && Merafour
* Version            : V1.0.0
* Last Modified Date : 09/18/2020
* Description        : 循环队列,C实现.
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
/*
* 数据接收关键两点，buf要小、接收要快，实测 2017字节 200ms频率收发连续测试 445757字节无数据丢失。
 */
#define MQ_LEN    (512+1024*4)
typedef struct{
		uint16_t Index_r;
		uint16_t Index_w;
		uint16_t Index_safe;
		//uint16_t w_tmp;
		//uint16_t r_tmp;
		uint8_t buf[MQ_LEN];
}mera_queue;
/*_____________________________________________________________ API ________________________________________________________________*/
static __inline void mq_init(mera_queue* const _queue)
{ 
		memset(_queue, 0, sizeof(mera_queue)); 
		_queue->Index_r = MQ_LEN-1;
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
// 判断缓存是否为空
static __inline uint16_t mq_isempty(const mera_queue* const _queue)
{
	uint16_t r_tmp;
	r_tmp = _queue->Index_r+1;
	if(r_tmp>=MQ_LEN) r_tmp=0;
	if(r_tmp == _queue->Index_w) return 1;  /* empty */
	return 0;
}
// 获取缓存中数据长度
static __inline uint16_t mq_size(const mera_queue* const _queue)
{
	uint16_t r_tmp;
	uint16_t index_w = _queue->Index_w;
	r_tmp = _queue->Index_r+1;
	if(r_tmp>=MQ_LEN) r_tmp=0;
	if(index_w>=r_tmp) return (index_w-r_tmp); // 数据长度
	// 循环队列, index_w<r_tmp
	else
	{
		return (index_w+MQ_LEN-r_tmp);
	}
}

#define macro_mq_read(index,buf,_size,_queue)  do{ \
			/*memset(buf, 0, _size);*/ \
			for(index=0; index<_size; index++) \
			{ \
				    uint16_t r_tmp; \
					r_tmp = _queue.index_r+1; \
					if(r_tmp>=MQ_LEN) r_tmp=0; \
					if(r_tmp == _queue.index_w) break;  /* empty */ \
					buf[index] = _queue.buf[r_tmp]; \
					_queue.index_r = r_tmp; \
			} \
}while(0);
#define macro_mq_read_safe(index,buf,_size,_queue)  do{ \
			memset(buf, 0, _size); \
			for(index=0; index<_size; index++) \
			{ \
				    uint16_t r_tmp; \
					r_tmp = _queue.index_safe+1; \
					if(r_tmp>=MQ_LEN) r_tmp=0; \
					if(r_tmp == _queue.index_w) break;  /* empty */ \
					buf[index] = _queue.buf[r_tmp]; \
					_queue.index_safe = r_tmp; \
			} \
}while(0);
/*_____________________________________________________________ API write ________________________________________________________________*/
#define macro_mq_write(_byte,_queue)  do{ \
			if(_queue.index_w != _queue.index_r) _queue.buf[_queue.index_w++] = _byte; \
			if(_queue.index_w>=MQ_LEN) _queue.index_w=0; \
}while(0);
#define macro_mq_write_cover(_byte,_queue)  do{ \
			if(_queue.index_w != _queue.index_r) _queue.buf[_queue.index_w++] = _byte; \
			else {_queue.index_r++; if(_queue.index_r>=MQ_LEN) _queue.index_r=0; _queue.buf[_queue.index_w++] = _byte;} \
			if(_queue.index_w>=MQ_LEN) _queue.index_w=0; \
}while(0);

static __inline int mq_write(mera_queue* const _queue, const uint8_t _byte)  
{ 
			if(_queue->Index_w != _queue->Index_r)_queue->buf[_queue->Index_w++] = _byte; 
			if(_queue->Index_w>=MQ_LEN) _queue->Index_w=0; 
			return 1;
}
static __inline int mq_write_cover(mera_queue* const _queue, const uint8_t _byte) 
{ 
		uint16_t index_w = _queue->Index_w;
		uint16_t index_r = _queue->Index_r;
		if(index_w != index_r)_queue->buf[index_w++] = _byte;
		else 
		{
				index_r++; 
				if(index_r>=MQ_LEN) index_r=0; 
				_queue->buf[index_w++] = _byte;
		} 
		if(index_w>=MQ_LEN) index_w=0;
		_queue->Index_r = index_r;		
		_queue->Index_w = index_w; 
		return 1;
} 

static __inline int mq_writes(mera_queue* const _queue, const uint8_t _byte[], const uint16_t _size)  
{ 
		uint16_t len;
		uint16_t index_w = _queue->Index_w;
		/*volatile*/ const uint16_t index_r = _queue->Index_r;
		//index_w = _queue->Index_w;
		for(len=0; len<_size; len++)
		{
				if(index_r == index_w) break;
				_queue->buf[index_w++] = _byte[len]; 
				if(index_w>=MQ_LEN) index_w=0; 
		}		
		_queue->Index_w = index_w;
		return len;
}
static __inline int mq_writes_cover(mera_queue* const _queue, const uint8_t _byte[], const uint16_t _size)  
{ 
		uint16_t len;
		uint16_t index_w = _queue->Index_w;
		uint16_t index_r = _queue->Index_r;
		//index_w = _queue->Index_w;
		for(len=0; len<_size; len++)
		{
				if(index_r != index_w) _queue->buf[index_w++] = _byte[len];
				else 
				{
					index_r++; 
					if(index_r>=MQ_LEN) index_r=0; 
					_queue->buf[index_w++] = _byte[len];
				}
				if(index_w>=MQ_LEN) index_w=0; 
		}
		_queue->Index_r = index_r;		
		_queue->Index_w = index_w;
		return len;
}
/*_____________________________________________________________ API read ________________________________________________________________*/
static __inline int mq_read(mera_queue* _queue, uint8_t* _byte)
{
		int ret=-1;
	  uint16_t r_tmp;
		//__asm("CPSID  I");
		r_tmp = _queue->Index_r+1;
		if(r_tmp>=MQ_LEN) r_tmp=0;
		if(r_tmp != _queue->Index_w)
		{
				*_byte = _queue->buf[r_tmp];
				_queue->Index_r = r_tmp;
				ret = 0;
		}
		//__asm("CPSIE  I");
		return ret;
}
static __inline uint16_t mq_reads(mera_queue* _queue, uint8_t buf[], const uint16_t buf_size)
{
		uint16_t index=0;
	  uint16_t index_r = _queue->Index_r;
		const uint16_t index_w = _queue->Index_w;
		memset(buf, 0, buf_size);
		for(index=0; index<buf_size; index++)
		{
				index_r++;
				if(index_r>=MQ_LEN) index_r=0;
				if(index_w == index_r) break;  // empty
				buf[index] = _queue->buf[index_r];
		}
		_queue->Index_r = index_r;
		return index;
}
/*_____________________________________________________________ API safe ________________________________________________________________*/
static void mq_init_safe(mera_queue* const _queue)
{ 
	_queue->Index_safe = _queue->Index_r;
}
static __inline uint16_t mq_reads_safe(uint8_t buf[], const uint16_t _size, mera_queue* const _queue)  
{
		uint16_t index;
		uint16_t index_r = _queue->Index_safe;
		const uint16_t index_w = _queue->Index_w;
		//index_r = _queue->Index_safe;
		//memset(buf, 0, _size); 
		for(index=0; index<_size; index++) 
		{ 
				index_r++;
				if(index_r>=MQ_LEN) index_r=0; 
				if(index_w == index_r) break;  /* empty */ 
				buf[index] = _queue->buf[index_r];  
		}
		_queue->Index_safe = index_r;
		return index;
}
// delete a _size member
static __inline void mq_reads_safe_del(mera_queue* const _queue, const uint16_t _size)
{
	uint16_t index;
	uint16_t index_r = _queue->Index_r;
	const uint16_t index_w = _queue->Index_w;
	for(index=0; index<_size; index++)
	{
		index_r++;
		if(index_r>=MQ_LEN) index_r=0;
		if(index_w == index_r) break;  /* empty */
	}
	_queue->Index_r = index_r;
}

#endif // __MERA_QUEUE_H__
