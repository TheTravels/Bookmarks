/******************** (C) COPYRIGHT 2015 merafour ********************
* File Name          : mera_queue_template.c
* Author             : 冷月追风 && Merafour
* Version            : V1.0.0
* Last Modified Date : 09/18/2020
* Description        : 循环队列,C实现,仿 C++ 模板写法,此处为模板通过宏展开生成具体的操作代码.
* Reference          : https://blog.csdn.net/zoomdy/article/details/79242528.
********************************************************************************
* https://merafour.blog.163.com
* merafour@163.com
* https://github.com/merafour
*******************************************************************************/

#include <stddef.h>
#include <string.h>
#include <stdint.h>

//#if defined(mq_type) && defined(MQ_LEN) && defined(MQT)
#if defined(mq_func) && defined(mq_type) && defined(MQ_LEN) && defined(MQT) && defined(MQ_NINLINE)

//#define mq_func(name)	  mq_##name##_temp

#if 0 // 队列数据结构定义
#define mq_queue(name,mqt,mql) struct mq_##name { \
		uint16_t Index_r;\
		uint16_t Index_w;\
		uint16_t Index_safe;\
		mqt _value[mql];\
};
mq_queue(temp,MQT,MQ_LEN);
//
//#define mq_type struct mq_temp
#endif

/*_____________________________________________________________ API ________________________________________________________________*/
void mq_func(init)(mq_type* const _mq)
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
// 判断缓存是否为空
uint16_t mq_func(isempty)(const mq_type* const _mq)
{
	uint16_t r_tmp;
	r_tmp = _mq->Index_r+1;
	if(r_tmp>=MQ_LEN) r_tmp=0;
	if(r_tmp == _mq->Index_w) return 1;  /* empty */
	return 0;
}
// 获取缓存中数据长度
uint16_t mq_func(size)(const mq_type* const _mq)
{
	uint16_t r_tmp;
	uint16_t index_w = _mq->Index_w;
	r_tmp = _mq->Index_r+1;
	if(r_tmp>=MQ_LEN) r_tmp=0;
	if(index_w>=r_tmp) return (index_w-r_tmp); // 数据长度
	// 循环队列, index_w<r_tmp
	else
	{
		return (index_w+MQ_LEN-r_tmp);
	}
}

/*_____________________________________________________________ API write ________________________________________________________________*/

int mq_func(write)(mq_type* const _mq, const MQT _byte)  
{ 
			if(_mq->Index_w != _mq->Index_r) memcpy(&_mq->_value[_mq->Index_w++], &_byte, sizeof(MQT)); 
			if(_mq->Index_w>=MQ_LEN) _mq->Index_w=0; 
			return 1;
}
int mq_func(writep)(mq_type* const _mq, const MQT* const _byte)  
{ 
			if(_mq->Index_w != _mq->Index_r) memcpy(&_mq->_value[_mq->Index_w++], _byte, sizeof(MQT)); 
			if(_mq->Index_w>=MQ_LEN) _mq->Index_w=0; 
			return 1;
}
int mq_func(write_cover)(mq_type* const _mq, const MQT* const _byte) 
{ 
		uint16_t index_w = _mq->Index_w;
		uint16_t index_r = _mq->Index_r;
		if(index_w != index_r) memcpy(&_mq->_value[index_w++], &_byte, sizeof(MQT)); 
		else 
		{
				index_r++; 
				if(index_r>=MQ_LEN) index_r=0; 
				memcpy(&_mq->_value[index_w++], &_byte, sizeof(MQT)); 
		} 
		if(index_w>=MQ_LEN) index_w=0;
		_mq->Index_r = index_r;		
		_mq->Index_w = index_w; 
		return 1;
} 
int mq_func(writep_cover)(mq_type* const _mq, const MQT _byte) 
{ 
		uint16_t index_w = _mq->Index_w;
		uint16_t index_r = _mq->Index_r;
		if(index_w != index_r) memcpy(&_mq->_value[index_w++], &_byte, sizeof(MQT)); 
		else 
		{
				index_r++; 
				if(index_r>=MQ_LEN) index_r=0; 
				memcpy(&_mq->_value[index_w++], &_byte, sizeof(MQT)); 
		} 
		if(index_w>=MQ_LEN) index_w=0;
		_mq->Index_r = index_r;		
		_mq->Index_w = index_w; 
		return 1;
} 

int mq_func(writes)(mq_type* const _mq, const MQT _byte[], const uint16_t _size)  
{ 
		uint16_t len;
		uint16_t index_w = _mq->Index_w;
		/*volatile*/ const uint16_t index_r = _mq->Index_r;
		//index_w = _queue->Index_w;
		for(len=0; len<_size; len++)
		{
				if(index_r == index_w) break;
				memcpy(&_mq->_value[index_w++], &_byte[len], sizeof(MQT)); 
				if(index_w>=MQ_LEN) index_w=0; 
		}		
		_mq->Index_w = index_w;
		return len;
}
int mq_func(writes_cover)(mq_type* const _mq, const MQT _byte[], const uint16_t _size)  
{ 
		uint16_t len;
		uint16_t index_w = _mq->Index_w;
		uint16_t index_r = _mq->Index_r;
		//index_w = _queue->Index_w;
		for(len=0; len<_size; len++)
		{
				if(index_r != index_w) memcpy(&_mq->_value[index_w++], &_byte[len], sizeof(MQT)); 
				else 
				{
					index_r++; 
					if(index_r>=MQ_LEN) index_r=0; 
					memcpy(&_mq->_value[index_w++], &_byte[len], sizeof(MQT)); 
				}
				if(index_w>=MQ_LEN) index_w=0; 
		}
		_mq->Index_r = index_r;		
		_mq->Index_w = index_w;
		return len;
}
/*_____________________________________________________________ API read ________________________________________________________________*/
int mq_func(read)(mq_type* _mq, MQT* const _byte)
{
		int ret=-1;
	  uint16_t r_tmp;
		//__asm("CPSID  I");
		r_tmp = _mq->Index_r+1;
		if(r_tmp>=MQ_LEN) r_tmp=0;
		if(r_tmp != _mq->Index_w)
		{
				memcpy(_byte, &_mq->_value[r_tmp], sizeof(MQT)); //*_byte = _queue->buf[r_tmp];
				_mq->Index_r = r_tmp;
				ret = 0;
		}
		//__asm("CPSIE  I");
		return ret;
}
uint16_t mq_func(reads)(mq_type* _mq, MQT buf[], const uint16_t buf_size)
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
				memcpy(&buf[index], &_mq->_value[index_r], sizeof(MQT));//buf[index] = _queue->buf[index_r++];
		}
		return index;
}
/*_____________________________________________________________ API safe ________________________________________________________________*/
void mq_func(init_safe)(mq_type* const _mq)
{ 
	_mq->Index_safe = _mq->Index_r;
}
uint16_t mq_func(read_safe)(mq_type* const _mq, MQT* const _byte)
{ 
	uint16_t index_r; 
	index_r = _mq->Index_safe+1; 
	if(index_r>=MQ_LEN) index_r=0; 
	if(index_r == _mq->Index_w) return 0;  /* empty */
	memcpy(_byte, &_mq->_value[index_r], sizeof(MQT)); 
	_mq->Index_safe = index_r; 
	return 1;
}
uint16_t mq_func(reads_safe)(mq_type* const _mq, MQT buf[], const uint16_t _size)  
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
				memcpy(&buf[index], &_mq->_value[index_r], sizeof(MQT));//buf[index] = _queue->buf[index_r];  
		}
		return index;
}
// delete a _size member
void mq_func(safe_del)(mq_type* const _mq, const uint16_t _size)
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

// 取消宏定义
#undef mq_func
#undef mq_type
#undef MQ_LEN
#undef MQT
#undef MQ_NINLINE
#endif // __MERA_QUEUE_H__
