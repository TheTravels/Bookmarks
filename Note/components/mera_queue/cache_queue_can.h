/**
  ******************************************************************************
  * File Name          : cache_queue_can.h
  ******************************************************************************
  * *reads_safe func : just read, dont delete member
  * *reads func      : read and delete member
  */
/* Includes ------------------------------------------------------------------*/
#ifndef _CACHE_QUEUE_CAN_H_
#define _CACHE_QUEUE_CAN_H_

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "cache_queue_can_data.h"

/*
* 数据接收关键两点，buf要小、接收要快，实测 2017字节 200ms频率收发连续测试 445757字节无数据丢失。
 */
#if 0
struct cache_can_rxbuf{
		uint32_t id;
		uint32_t rx_time;
		uint8_t data[8];
		uint8_t DLC;
		uint8_t flag;
};
#endif
#define CACHE_QUEUE_CAN_LEN    (512)
struct cache_queue_can{
		uint16_t index_r;
		uint16_t index_w;
		uint16_t index_safe;
		//uint8_t buf[CACHE_QUEUE_CAN_LEN];
		struct cache_can_rxbuf buf[CACHE_QUEUE_CAN_LEN];
};

static __inline void init_queue_can(struct cache_queue_can* const _queue)
{ 
		memset(_queue, 0, sizeof(struct cache_queue_can)); 
		_queue->index_r = CACHE_QUEUE_CAN_LEN-1;
}
static __inline void init_queue_can_safe(struct cache_queue_can* const _queue)
{ 
		_queue->index_safe = _queue->index_r;
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
static __inline uint16_t cache_queue_can_isempty(const struct cache_queue_can* const _queue)
{
	uint16_t r_tmp;
	r_tmp = _queue->index_r+1;
	if(r_tmp>=CACHE_QUEUE_CAN_LEN) r_tmp=0;
	if(r_tmp == _queue->index_w) return 1;  /* empty */
	return 0;
}
// 获取缓存中数据长度
static __inline uint16_t cache_queue_can_size(const struct cache_queue_can* const _queue)
{
	uint16_t r_tmp;
	uint16_t index_w = _queue->index_w;
	r_tmp = _queue->index_r+1;
	if(r_tmp>=CACHE_QUEUE_CAN_LEN) r_tmp=0;
	if(index_w>=r_tmp) return (index_w-r_tmp); // 数据长度
	// 循环队列, index_w<r_tmp
	else
	{
		return (index_w+CACHE_QUEUE_CAN_LEN-r_tmp);
	}
}

#define macro_queue_can_reads(index,_unit,_size,_queue)  do{ \
			/*memset(buf, 0, _size);*/ \
			for(index=0; index<_size; index++) \
			{ \
				    uint16_t r_tmp; \
					r_tmp = _queue.index_r+1; \
					if(r_tmp>=CACHE_QUEUE_CAN_LEN) r_tmp=0; \
					if(r_tmp == _queue.index_w) break;  /* empty */ \
					memcpy(&_unit[index], &_queue.buf[r_tmp], sizeof(struct cache_can_rxbuf));/*buf[index] = _queue.buf[r_tmp];*/ \
					_queue.index_r = r_tmp; \
			} \
}while(0);
#define macro_queue_can_reads_safe(index,_unit,_size,_queue)  do{ \
			/*memset(_unit, 0, _size); */_queue.index_safe = _queue.index_r; \
			for(index=0; index<_size; index++) \
			{ \
				  uint16_t r_tmp; \
					r_tmp = _queue.index_safe+1; \
					if(r_tmp>=CACHE_QUEUE_CAN_LEN) r_tmp=0; \
					if(r_tmp == _queue.index_w) break;  /* empty */ \
					memcpy(&_unit[index], &_queue.buf[r_tmp], sizeof(struct cache_can_rxbuf));/*buf[index] = _queue.buf[r_tmp];*/ \
					_queue.index_safe = r_tmp; \
			} \
}while(0);
// need call init_queue_can_safe Initialize the index_safe member
#if 1
static __inline uint16_t queue_can_reads_safe(struct cache_can_rxbuf _unit[], const uint16_t _size, struct cache_queue_can* const _queue)  
{ 
	uint16_t index;
	uint16_t index_r, index_w;
	index_r = _queue->index_safe;
	index_w = _queue->index_w;
	for(index=0; index<_size; index++) 
	{ 
		index_r++; 
		if(index_r>=CACHE_QUEUE_CAN_LEN) index_r=0;
		if(index_r == index_w) break;  /* empty */
		memcpy(&_unit[index], &_queue->buf[index_r], sizeof(struct cache_can_rxbuf));/*buf[index] = _queue.buf[r_tmp];*/ 
	} 
	_queue->index_safe = index_r;
	return index;
}
#endif
// delete a _size member
static __inline void queue_can_reads_safe_del(struct cache_queue_can* const _queue, const uint16_t _size)
{
	uint16_t index;
	uint16_t index_r, index_w;
	index_r = _queue->index_r;
	index_w = _queue->index_w;
	for(index=0; index<_size; index++)
	{
		index_r++;
		if(index_r>=CACHE_QUEUE_CAN_LEN) index_r=0;
		if(index_r == index_w) break;  /* empty */
		//memset(&_queue->buf[index_r], 0, sizeof(struct cache_can_rxbuf));
		//_queue->buf[index_r].flag = 0;
	}
	_queue->index_r = index_r;
}
#define macro_queue_can_read(_unit,_queue)  do{ \
				  uint16_t r_tmp; \
					r_tmp = _queue.index_r+1; \
					if(r_tmp>=CACHE_QUEUE_CAN_LEN) r_tmp=0; \
					if(r_tmp == _queue.index_w) break;  /* empty */ \
					memcpy(&(_unit), &_queue.buf[r_tmp], sizeof(struct cache_can_rxbuf)); \
					_queue.index_r = r_tmp; \
}while(0);
// need call init_queue_can_safe Initialize the index_safe member
#define macro_queue_can_read_safe(_unit,_queue)  do{ \
				  uint16_t r_tmp; \
					r_tmp = _queue.index_safe+1; \
					if(r_tmp>=CACHE_QUEUE_CAN_LEN) r_tmp=0; \
					memcpy(&(_unit), &_queue.buf[r_tmp], sizeof(struct cache_can_rxbuf)); \
}while(0);
// read one data unit
static __inline uint16_t queue_can_read(struct cache_can_rxbuf* const _unit, struct cache_queue_can* const _queue)
{
	uint16_t r_tmp; 
	r_tmp = _queue->index_r+1; 
	if(r_tmp>=CACHE_QUEUE_CAN_LEN) r_tmp=0; 
	if(r_tmp == _queue->index_w) return 0;  /* empty */ 
	memcpy(_unit, &_queue->buf[r_tmp], sizeof(struct cache_can_rxbuf)); 
	_queue->index_r = r_tmp; 
	return 1;
}
static __inline uint16_t queue_can_read_safe(struct cache_can_rxbuf* const _unit, struct cache_queue_can* const _queue)
{ 
	uint16_t r_tmp; 
	r_tmp = _queue->index_safe+1; 
	if(r_tmp>=CACHE_QUEUE_CAN_LEN) r_tmp=0; 
	if(r_tmp == _queue->index_w) return 0;  /* empty */
	memcpy(_unit, &_queue->buf[r_tmp], sizeof(struct cache_can_rxbuf)); 
	_queue->index_safe = r_tmp; 
	return 1;
}
// 取最新的数据
#if 0
#define macro_queue_can_read_new_safe(_unit,_queue)  do{ \
				  uint16_t r_tmp; \
					r_tmp = _queue.index_w; \
					if(r_tmp<=0) r_tmp=CACHE_QUEUE_CAN_LEN; \
					memcpy(&(_unit), &_queue.buf[--r_tmp], sizeof(struct cache_can_rxbuf)); \
}while(0);
#endif
#define macro_queue_can_write(_unit,_queue)  do{ \
			if(_queue.index_w != _queue.index_r) memcpy(&_queue.buf[_queue.index_w++], &(_unit), sizeof(struct cache_can_rxbuf));/*_queue.buf[_queue.index_w++] = _byte;*/ \
			if(_queue.index_w>=CACHE_QUEUE_CAN_LEN) _queue.index_w=0; \
}while(0);
static __inline void queue_can_write(struct cache_can_rxbuf* const _unit, struct cache_queue_can* const _queue)
{ 
		uint16_t index_w = _queue->index_w;
		if(index_w != _queue->index_r) memcpy(&_queue->buf[index_w++], _unit, sizeof(struct cache_can_rxbuf));
		if(index_w>=CACHE_QUEUE_CAN_LEN) index_w=0;
		_queue->index_w = index_w;
}

#endif //_CACHE_QUEUE_CAN_H_
