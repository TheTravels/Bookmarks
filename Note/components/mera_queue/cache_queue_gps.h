/**
  ******************************************************************************
  * File Name          : cache_queue_gps.h
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#ifndef _TERMINAL_GPS_H_
#define _TERMINAL_GPS_H_

#include <stdint.h>
#include <string.h>
#include <stdio.h>

struct cache_queue_gps_unit{
		uint32_t  lat;    // 纬度,精度:0.00001°,即十万分之一
		uint32_t  lon;    // 经度,精度:0.00001°,即十万分之一
		uint32_t  utc;    // UTC 时间戳,秒数
		union {
			uint32_t status;
			struct {
				uint32_t  NLat:1;    // 纬度方向, 1: N-北, 0：S-南
				uint32_t  ELon:1;    // 经度方向, 1：E-东, 0: W-西
				//uint32_t  valid:1;   // 定位有效标志
				uint32_t  numSv:5;   // 卫星数 0-24
				uint32_t  FS:3;      // 定位质量, 0, 1, 6
				uint32_t  HDOP:5;    // 水平精度因子
				uint32_t  msl:13;    // 海拔高度,精度 1m
				uint32_t  rev:4;     // 保留
			};
		};
};

/*
* 数据接收关键两点，buf要小、接收要快，实测 2017字节 200ms频率收发连续测试 445757字节无数据丢失。
 */
#define CACHE_QUEUE_GPS_LEN    (512)
typedef struct{
		uint16_t index_r;
		uint16_t index_w;
		uint16_t index_safe;
		struct cache_queue_gps_unit buf[CACHE_QUEUE_GPS_LEN];
}cache_queue_gps;
/*_____________________________________________________________ API ________________________________________________________________*/
static __inline void init_queue_gps(cache_queue_gps* const _queue)
{ 
		memset(_queue, 0, sizeof(cache_queue_gps)); 
		_queue->index_r = CACHE_QUEUE_GPS_LEN-1;
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
static __inline uint16_t cache_queue_gps_isempty(const cache_queue_gps* const _queue)
{
	uint16_t r_tmp;
	r_tmp = _queue->index_r+1;
	if(r_tmp>=CACHE_QUEUE_GPS_LEN) r_tmp=0;
	if(r_tmp == _queue->index_w) return 1;  /* empty */
	return 0;
}
// 获取缓存中数据长度
static __inline uint16_t cache_queue_gps_size(const cache_queue_gps* const _queue)
{
	uint16_t r_tmp;
	uint16_t index_w = _queue->index_w;
	r_tmp = _queue->index_r+1;
	if(r_tmp>=CACHE_QUEUE_GPS_LEN) r_tmp=0;
	if(index_w>=r_tmp) return (index_w-r_tmp); // 数据长度
	// 循环队列, index_w<r_tmp
	else
	{
		return (index_w+CACHE_QUEUE_GPS_LEN-r_tmp);
	}
}

#define macro_queue_gps_write(_byte,_queue)  do{ \
			if(_queue.index_w != _queue.index_r) memcpy(&_queue.buf[_queue.index_w++], &_byte, sizeof(struct cache_queue_gps_unit)); \
			if(_queue.index_w>=CACHE_QUEUE_GPS_LEN) _queue.index_w=0; \
}while(0);
#define macro_queue_gps_write_cover(_byte,_queue)  do{ \
			if(_queue.index_w != _queue.index_r) memcpy(&_queue.buf[_queue.index_w++], &_byte, sizeof(struct cache_queue_gps_unit)); \
			else {_queue.index_r++; if(_queue.index_r>=CACHE_QUEUE_GPS_LEN) _queue.index_r=0; memcpy(&_queue.buf[_queue.index_w++], &_byte, sizeof(struct cache_queue_gps_unit));} \
			if(_queue.index_w>=CACHE_QUEUE_GPS_LEN) _queue.index_w=0; \
}while(0);

static __inline int cache_queue_gps_write(cache_queue_gps* const _queue, const struct cache_queue_gps_unit* const _byte)  
{ 
			if(_queue->index_w != _queue->index_r) memcpy(&_queue->buf[_queue->index_w++], _byte, sizeof(struct cache_queue_gps_unit)); 
			if(_queue->index_w>=CACHE_QUEUE_GPS_LEN) _queue->index_w=0; 
			return 1;
}
static __inline int cache_queue_gps_write_cover(cache_queue_gps* const _queue, const struct cache_queue_gps_unit* const _byte)  
{ 
			if(_queue->index_w != _queue->index_r) memcpy(&_queue->buf[_queue->index_w++], _byte, sizeof(struct cache_queue_gps_unit)); 
			else 
			{
					_queue->index_r++; 
					if(_queue->index_r>=CACHE_QUEUE_GPS_LEN) _queue->index_r=0; 
					memcpy(&_queue->buf[_queue->index_w++], _byte, sizeof(struct cache_queue_gps_unit));
			}
			if(_queue->index_w>=CACHE_QUEUE_GPS_LEN) _queue->index_w=0; 
			return 1;
}
static __inline int cache_queue_gps_writes(cache_queue_gps* const _queue, const struct cache_queue_gps_unit* const _byte[], const uint16_t _size)  
{ 
		uint16_t len;
		uint16_t index_w;
		const uint16_t index_r = _queue->index_r;
		index_w = _queue->index_w;
		for(len=0; len<_size; len++)
		{
				if(index_r == index_w) break;
				memcpy(&_queue->buf[_queue->index_w++], &_byte[len], sizeof(struct cache_queue_gps_unit));//_queue->buf[index_w++] = _byte[len]; 
				if(index_w>=CACHE_QUEUE_GPS_LEN) index_w=0; 
		}		
		_queue->index_w = index_w;
		return len;
}
static __inline int cache_queue_gps_writes_cover(cache_queue_gps* const _queue, const struct cache_queue_gps_unit* const _byte[], const uint16_t _size)  
{ 
		uint16_t len;
		uint16_t index_w;
		uint16_t index_r = _queue->index_r;
		index_w = _queue->index_w;
		for(len=0; len<_size; len++)
		{
				if(index_r != index_w) memcpy(&_queue->buf[_queue->index_w++], &_byte[len], sizeof(struct cache_queue_gps_unit));//_queue->buf[index_w++] = _byte[len];
				else 
				{
					index_r++; 
					if(index_r>=CACHE_QUEUE_GPS_LEN) index_r=0; 
					memcpy(&_queue->buf[_queue->index_w++], &_byte[len], sizeof(struct cache_queue_gps_unit));//_queue->buf[index_w++] = _byte[len];
				}
				if(index_w>=CACHE_QUEUE_GPS_LEN) index_w=0; 
		}
		_queue->index_r = index_r;		
		_queue->index_w = index_w;
		return len;
}
static __inline int cache_queue_gps_read(cache_queue_gps* _queue, struct cache_queue_gps_unit* const _byte)
{
		int ret=-1;
	  uint16_t r_tmp;
		//__asm("CPSID  I");
		r_tmp = _queue->index_r+1;
		if(r_tmp>=CACHE_QUEUE_GPS_LEN) r_tmp=0;
		if(r_tmp != _queue->index_w)
		{
				memcpy(_byte, &_queue->buf[r_tmp], sizeof(struct cache_queue_gps_unit));//*_byte = _cache->buf[r_tmp];
				_queue->index_r = r_tmp;
				ret = 0;
		}
		//__asm("CPSIE  I");
		return ret;
}
/*_____________________________________________________________ API safe ________________________________________________________________*/
static void queue_gps_init_safe(cache_queue_gps* const _queue)
{ 
	_queue->index_safe = _queue->index_r;
}
static __inline uint16_t queue_gps_reads_safe(struct cache_queue_gps_unit buf[], const uint16_t _size, cache_queue_gps* const _queue)  
{
		uint16_t index;
		uint16_t index_r = _queue->index_safe;
		const uint16_t index_w = _queue->index_w;
		//index_r = _queue->index_safe;
		//memset(buf, 0, _size); 
		for(index=0; index<_size; index++) 
		{ 
				index_r++;
				if(index_r>=CACHE_QUEUE_GPS_LEN) index_r=0; 
				if(index_w == index_r) break;  /* empty */ 
				memcpy(&buf[index], &_queue->buf[index_r], sizeof(struct cache_queue_gps_unit));//buf[index] = _queue->buf[index_r];  
		}
		_queue->index_safe = index_r;
		return index;
}
// delete a _size member
static __inline void queue_gps_reads_safe_del(cache_queue_gps* const _queue, const uint16_t _size)
{
	uint16_t index;
	uint16_t index_r = _queue->index_r;
	const uint16_t index_w = _queue->index_w;
	for(index=0; index<_size; index++)
	{
		index_r++;
		if(index_r>=CACHE_QUEUE_GPS_LEN) index_r=0;
		if(index_w == index_r) break;  /* empty */
	}
	_queue->index_r = index_r;
}
static __inline uint16_t cache_queue_gps_reads(cache_queue_gps* _queue, struct cache_queue_gps_unit buf[], const uint16_t buf_size)
{
		uint16_t index=0;
	    uint16_t index_r = _queue->index_r;
		const uint16_t index_w = _queue->index_w;
		memset(buf, 0, buf_size);
		for(index=0; index<buf_size; index++)
		{
				if(index_w == index_r) break;  // empty
				memcpy(&buf[index], &_queue->buf[index_r++], sizeof(struct cache_queue_gps_unit));//buf[index] = _cache->buf[_cache->index_r++];
				if(index_r>=CACHE_QUEUE_GPS_LEN) index_r=0;
		}
		_queue->index_r = index_r;
		return index;
}

//#ifdef CACHE_QUEUE_GPS_LEN
//#undef CACHE_QUEUE_GPS_LEN
//#endif

#endif //_TERMINAL_H_
