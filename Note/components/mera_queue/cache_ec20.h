/**
  ******************************************************************************
  * File Name          : cache_ec20.h
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#ifndef _CACHE_EC20_H_
#define _CACHE_EC20_H_

#include <stdint.h>
#include <string.h>
#include <stdio.h>
//#include "usart.h"

/*
* 数据接收关键两点，buf要小、接收要快，实测 2017字节 200ms频率收发连续测试 445757字节无数据丢失。
 */
#define CACHE_QUEUE_EC20_LEN    (1024*24)  // 24K
typedef struct{
		uint16_t index_r;
		uint16_t index_w;
		uint16_t index_safe;
		uint8_t buf[CACHE_QUEUE_EC20_LEN-6];
}cache_queue_ec20;

static __inline void init_queue_ec20(cache_queue_ec20* const _queue)
{ 
		memset(_queue, 0, sizeof(cache_queue_ec20)); 
		_queue->index_r = sizeof(_queue->buf)-1;
}
/*
 * cache_queue1: size = index_w - index_r - 1;
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
 * cache_queue2: size = index_w + SIZE - index_r - 1;
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
static __inline uint16_t cache_queue_ec20_isempty(const cache_queue_ec20* const _queue)
{
	uint16_t r_tmp;
	r_tmp = _queue->index_r+1;
	if(r_tmp>=sizeof(_queue->buf)) r_tmp=0;
	if(r_tmp == _queue->index_w) return 1;  /* empty */
	return 0;
}
// 获取缓存中数据长度
static __inline uint16_t cache_queue_ec20_size(const cache_queue_ec20* const _queue)
{
	uint16_t r_tmp;
	uint16_t index_w = _queue->index_w;
	r_tmp = _queue->index_r+1;
	if(r_tmp>=sizeof(_queue->buf)) r_tmp=0;
	if(index_w>=r_tmp) return (index_w-r_tmp); // 数据长度
	// 循环队列, index_w<r_tmp
	else
	{
		return (index_w+sizeof(_queue->buf)-r_tmp);
	}
}

#define macro_queue_ec20_read(index,buf,_size,_queue)  do{ \
			memset(buf, 0, _size); \
			for(index=0; index<_size; index++) \
			{ \
				    uint16_t r_tmp; \
					r_tmp = _queue.index_r+1; \
					if(r_tmp>=sizeof(_queue.buf)) r_tmp=0; \
					if(r_tmp == _queue.index_w) break;  /* empty */ \
					buf[index] = _queue.buf[r_tmp]; \
					_queue.index_r = r_tmp; \
			} \
}while(0);
#define macro_queue_ec20_read_safe(index,buf,_size,_queue)  do{ \
			memset(buf, 0, _size); \
			for(index=0; index<_size; index++) \
			{ \
				    uint16_t r_tmp; \
					r_tmp = _queue.index_safe+1; \
					if(r_tmp>=sizeof(_queue.buf)) r_tmp=0; \
					if(r_tmp == _queue.index_w) break;  /* empty */ \
					buf[index] = _queue.buf[r_tmp]; \
					_queue.index_safe = r_tmp; \
			} \
}while(0);
#define macro_queue_ec20_write(_queue,_byte)  do{ \
			if(_queue.index_w != _queue.index_r) _queue.buf[_queue.index_w++] = _byte; \
			if(_queue.index_w>=sizeof(_queue.buf)) _queue.index_w=0; \
}while(0);

static __inline int cache_queue_ec20_write(cache_queue_ec20* const _queue, const uint8_t _byte)  
{ 
#if 0
			uint16_t w_tmp;
			_queue->buf[_queue->index_w] = _byte; 
			w_tmp = _queue->index_w+1; 
			if(w_tmp>=sizeof(_queue->buf)) w_tmp=0; 
			if(w_tmp != _queue->index_r) 
			{
				_queue->index_w=w_tmp;
				return 1;
			}				
			return 0;
#else
			if(_queue->index_w != _queue->index_r)_queue->buf[_queue->index_w++] = _byte; 
			if(_queue->index_w>=sizeof(_queue->buf)) _queue->index_w=0; 
			return 1;
#endif
}
static __inline int cache_queue_ec20_writes(cache_queue_ec20* const _queue, const uint8_t _byte[], const uint16_t _size)  
{ 
			uint16_t w_tmp;
			uint16_t len;
	for(len=0; len<_size; len++)
	{
		_queue->buf[_queue->index_w] = _byte[len]; 
		w_tmp = _queue->index_w+1; 
			if(w_tmp>=sizeof(_queue->buf)) w_tmp=0; 
			if(w_tmp == _queue->index_r) 
			{
				break;
			}	
	}		
			return len;
}
static __inline uint16_t cache_queue_ec20_reads(cache_queue_ec20* _cache, uint8_t buf[], const uint16_t buf_size)
{
		uint16_t index=0;
	  //uint16_t r_tmp;
		memset(buf, 0, buf_size);
		for(index=0; index<buf_size; index++)
		{
				if(_cache->index_r == _cache->index_w) break;  // empty
				buf[index] = _cache->buf[_cache->index_r++];
				if(_cache->index_r>=sizeof(_cache->buf)) _cache->index_r=0;
		}
		return index;
}

#endif //_CACHE_EC20_H_
