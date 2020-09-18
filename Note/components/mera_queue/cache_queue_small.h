/**
  ******************************************************************************
  * File Name          : terminal.h
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#ifndef _TERMINAL_SMALL_H_
#define _TERMINAL_SMALL_H_

#include <stdint.h>
#include <string.h>
#include <stdio.h>
//#include "usart.h"

/*
* 数据接收关键两点，buf要小、接收要快，实测 2017字节 200ms频率收发连续测试 445757字节无数据丢失。
 */
#define CACHE_QUEUE_SMALL_LEN    (512)
typedef struct{
		uint16_t index_r;
		uint16_t index_w;
		uint16_t index_safe;
		uint8_t buf[CACHE_QUEUE_SMALL_LEN];
}cache_queue_small;
/*_____________________________________________________________ API ________________________________________________________________*/
static __inline void init_queue_small(cache_queue_small* const _queue)
{ 
		memset(_queue, 0, sizeof(cache_queue)); 
		_queue->index_r = CACHE_QUEUE_SMALL_LEN-1;
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
static __inline uint16_t cache_queue_small_isempty(const cache_queue_small* const _queue)
{
	uint16_t r_tmp;
	r_tmp = _queue->index_r+1;
	if(r_tmp>=CACHE_QUEUE_SMALL_LEN) r_tmp=0;
	if(r_tmp == _queue->index_w) return 1;  /* empty */
	return 0;
}
// 获取缓存中数据长度
static __inline uint16_t cache_queue_small_size(const cache_queue_small* const _queue)
{
	uint16_t r_tmp;
	uint16_t index_w = _queue->index_w;
	r_tmp = _queue->index_r+1;
	if(r_tmp>=CACHE_QUEUE_SMALL_LEN) r_tmp=0;
	if(index_w>=r_tmp) return (index_w-r_tmp); // 数据长度
	// 循环队列, index_w<r_tmp
	else
	{
		return (index_w+CACHE_QUEUE_SMALL_LEN-r_tmp);
	}
}

#define macro_queue_small_read(index,buf,_size,_queue)  do{ \
			/*memset(buf, 0, _size);*/ \
			for(index=0; index<_size; index++) \
			{ \
				    uint16_t r_tmp; \
					r_tmp = _queue.index_r+1; \
					if(r_tmp>=CACHE_QUEUE_SMALL_LEN) r_tmp=0; \
					if(r_tmp == _queue.index_w) break;  /* empty */ \
					buf[index] = _queue.buf[r_tmp]; \
					_queue.index_r = r_tmp; \
			} \
}while(0);
/*_____________________________________________________________ API write ________________________________________________________________*/
#define macro_queue_small_write(_byte,_queue)  do{ \
	if(_queue.index_w != _queue.index_r) _queue.buf[_queue.index_w++] = _byte; \
			if(_queue.index_w>=CACHE_QUEUE_SMALL_LEN) _queue.index_w=0; \
}while(0);
#define macro_queue_small_write_cover(_byte,_queue)  do{ \
			if(_queue.index_w != _queue.index_r) _queue.buf[_queue.index_w++] = _byte; \
			else {_queue.index_r++; if(_queue.index_r>=CACHE_QUEUE_SMALL_LEN) _queue.index_r=0; _queue.buf[_queue.index_w++] = _byte;} \
			if(_queue.index_w>=CACHE_QUEUE_SMALL_LEN) _queue.index_w=0; \
}while(0);

static __inline int cache_queue_small_write(cache_queue_small* const _queue, const uint8_t _byte)  
{ 
	if(_queue->index_w != _queue->index_r)_queue->buf[_queue->index_w++] = _byte; 
	if(_queue->index_w>=CACHE_QUEUE_SMALL_LEN) _queue->index_w=0; 
	return 1;
}
static __inline int cache_queue_small_write_cover(cache_queue_small* const _queue, const uint8_t _byte) 
{ 
		uint16_t index_w = _queue->index_w;
		uint16_t index_r = _queue->index_r;
		if(index_w != index_r)_queue->buf[index_w++] = _byte;
		else 
		{
				index_r++; 
				if(index_r>=CACHE_QUEUE_SMALL_LEN) index_r=0; 
				_queue->buf[index_w++] = _byte;
		} 
		if(index_w>=CACHE_QUEUE_SMALL_LEN) index_w=0;
		_queue->index_r = index_r;		
		_queue->index_w = index_w; 
		return 1;
} 

static __inline int cache_queue_small_writes(cache_queue_small* const _queue, const uint8_t _byte[], const uint16_t _size)  
{ 
		uint16_t len;
		uint16_t index_w;
		const uint16_t index_r = _queue->index_r;
		index_w = _queue->index_w;
		for(len=0; len<_size; len++)
		{
				if(index_r == index_w) break;
				_queue->buf[index_w++] = _byte[len]; 
				if(index_w>=CACHE_QUEUE_SMALL_LEN) index_w=0; 
		}		
		_queue->index_w = index_w;
		return len;
}
static __inline int cache_queue_small_writes_cover(cache_queue_small* const _queue, const uint8_t _byte[], const uint16_t _size)  
{ 
		uint16_t len;
		uint16_t index_w;
		uint16_t index_r = _queue->index_r;
		index_w = _queue->index_w;
		for(len=0; len<_size; len++)
		{
				if(index_r != index_w) _queue->buf[index_w++] = _byte[len];
				else 
				{
					index_r++; 
					if(index_r>=CACHE_QUEUE_SMALL_LEN) index_r=0; 
					_queue->buf[index_w++] = _byte[len];
				}
				if(index_w>=CACHE_QUEUE_SMALL_LEN) index_w=0; 
		}
		_queue->index_r = index_r;		
		_queue->index_w = index_w;
		return len;
}
/*_____________________________________________________________ API read ________________________________________________________________*/
static __inline int cache_queue_small_read(cache_queue_small* _cache, uint8_t* _byte)
{
		int ret=-1;
	  uint16_t r_tmp;
		//__asm("CPSID  I");
		r_tmp = _cache->index_r+1;
		if(r_tmp>=CACHE_QUEUE_SMALL_LEN) r_tmp=0;
		if(r_tmp != _cache->index_w)
		{
				*_byte = _cache->buf[r_tmp];
				_cache->index_r = r_tmp;
				ret = 0;
		}
		//__asm("CPSIE  I");
		return ret;
}
static __inline uint16_t cache_queue_small_reads(cache_queue_small* _queue, uint8_t buf[], const uint16_t buf_size)
{
		uint16_t index=0;
	    uint16_t index_r = _queue->index_r;
		const uint16_t index_w = _queue->index_w;
		memset(buf, 0, buf_size);
		for(index=0; index<buf_size; index++)
		{
				index_r++;
				if(index_r>=CACHE_QUEUE_SMALL_LEN) index_r=0;
				if(index_w == index_r) break;  // empty
				buf[index] = _queue->buf[index_r++];
		}
		return index;
}
/*_____________________________________________________________ API safe ________________________________________________________________*/
static void queue_small_init_safe(cache_queue_small* const _queue)
{ 
	_queue->index_safe = _queue->index_r;
}
static __inline uint16_t queue_small_reads_safe(uint8_t buf[], const uint16_t _size, cache_queue_small* const _queue)  
{
		uint16_t index;
		uint16_t index_r, index_w;
		index_r = _queue->index_safe;
		index_w = _queue->index_w;
		//memset(buf, 0, _size); 
		for(index=0; index<_size; index++) 
		{ 
				index_r++;
				if(index_r>=CACHE_QUEUE_SMALL_LEN) index_r=0; 
				if(index_r == index_w) break;  /* empty */ 
				buf[index] = _queue->buf[index_r];  
				//if(index_r>=sizeof(_queue->buf)) index_r=0;
		}
		_queue->index_safe = index_r;
		return index;
}
// delete a _size member
static __inline void queue_small_reads_safe_del(cache_queue_small* const _queue, const uint16_t _size)
{
	uint16_t index;
	uint16_t index_r, index_w;
	index_r = _queue->index_r;
	index_w = _queue->index_w;
	for(index=0; index<_size; index++)
	{
		index_r++;
		if(index_r>=CACHE_QUEUE_SMALL_LEN) index_r=0;
		if(index_r == index_w) break;  /* empty */
	}
	_queue->index_r = index_r;
}

#endif //_TERMINAL_SMALL_H_
