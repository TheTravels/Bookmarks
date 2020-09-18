/******************** (C) COPYRIGHT 2015 merafour ********************
* File Name          : mera_queue_cxx.h
* Author             : 冷月追风 && Merafour
* Version            : V1.0.0
* Last Modified Date : 09/18/2020
* Description        : 循环队列,C++实现(mera_queue C++版本),使用类模板方式,减少不同数据成员的重复编码.
* Reference          : https://github.com/ArduPilot/ardupilot/blob/master/libraries/AP_Param/AP_Param.h.
* Reference          : http://c.biancheng.net/view/320.html.
********************************************************************************
* https://merafour.blog.163.com
* merafour@163.com
* https://github.com/merafour
*******************************************************************************/

#include <stddef.h>
#include <string.h>
#include <stdint.h>
//#include <cmath>
//#include<cstring>

#ifndef __MERA_QUEUE_CXX_H__
#define __MERA_QUEUE_CXX_H__

/// Template class for array variables.
///
/// Objects created using this template behave like arrays of the type T,
/// but are stored like single variables.
///
/// @tparam MQ_T           The scalar type of the variable
/// @tparam MQ_LEN           number of elements
/// @tparam PT          the AP_PARAM_* type
///
template<typename MQT, uint16_t MQ_LEN>
class mera_queue_cxx
{
protected:
		uint16_t Index_r;
		uint16_t Index_w;
		uint16_t Index_safe;
		MQT _value[MQ_LEN];
public:
/*_____________________________________________________________ API ________________________________________________________________*/
void init(void)
{ 
		memset(_value, 0, sizeof(_value));
		Index_w = 0; 
		Index_r = MQ_LEN-1;
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
uint16_t isempty(void)
{
	uint16_t r_tmp;
	r_tmp = Index_r+1;
	if(r_tmp>=MQ_LEN) r_tmp=0;
	if(r_tmp == Index_w) return 1;  /* empty */
	return 0;
}
// 获取缓存中数据长度
uint16_t size(void)
{
	uint16_t r_tmp;
	uint16_t index_w = Index_w;
	r_tmp = Index_r+1;
	if(r_tmp>=MQ_LEN) r_tmp=0;
	if(index_w>=r_tmp) return (index_w-r_tmp); // 数据长度
	// 循环队列, index_w<r_tmp
	else
	{
		return (index_w+MQ_LEN-r_tmp);
	}
}
/*_____________________________________________________________ API write ________________________________________________________________*/
int write(const MQT _byte)  
{ 
			if(Index_w != Index_r) memcpy(&_value[Index_w++], &_byte, sizeof(MQT)); 
			if(Index_w>=MQ_LEN) Index_w=0; 
			return 1;
}
int write(const MQT* const _byte)  
{ 
			if(Index_w != Index_r) memcpy(&_value[Index_w++], _byte, sizeof(MQT)); 
			if(Index_w>=MQ_LEN) Index_w=0; 
			return 1;
}
int write_cover(const MQT _byte) 
{ 
		uint16_t index_w = Index_w;
		uint16_t index_r = Index_r;
		if(index_w != index_r) memcpy(&_value[index_w++], &_byte, sizeof(MQT)); 
		else 
		{
				index_r++; 
				if(index_r>=MQ_LEN) index_r=0; 
				memcpy(&_value[index_w++], &_byte, sizeof(MQT)); 
		} 
		if(index_w>=MQ_LEN) index_w=0;
		Index_r = index_r;		
		Index_w = index_w; 
		return 1;
} 
int write_cover(const MQT* const _byte) 
{ 
		uint16_t index_w = Index_w;
		uint16_t index_r = Index_r;
		if(index_w != index_r) memcpy(&_value[index_w++], _byte, sizeof(MQT)); 
		else 
		{
				index_r++; 
				if(index_r>=MQ_LEN) index_r=0; 
				memcpy(&_value[index_w++], _byte, sizeof(MQT)); 
		} 
		if(index_w>=MQ_LEN) index_w=0;
		Index_r = index_r;		
		Index_w = index_w; 
		return 1;
} 

int writes(const MQT _byte[], const uint16_t _size)  
{ 
		uint16_t len;
		uint16_t index_w = Index_w;
		/*volatile*/ const uint16_t index_r = Index_r;
		//index_w = Index_w;
		for(len=0; len<_size; len++)
		{
				if(index_r == index_w) break;
				memcpy(&_value[index_w++], &_byte[len], sizeof(MQT)); 
				if(index_w>=MQ_LEN) index_w=0; 
		}		
		Index_w = index_w;
		return len;
}
int writes_cover(const MQT _byte[], const uint16_t _size)  
{ 
		uint16_t len;
		uint16_t index_w = Index_w;
		uint16_t index_r = Index_r;
		//index_w = Index_w;
		for(len=0; len<_size; len++)
		{
				if(index_r != index_w) memcpy(&_value[index_w++], &_byte[len], sizeof(MQT)); 
				else 
				{
					index_r++; 
					if(index_r>=MQ_LEN) index_r=0; 
					memcpy(&_value[index_w++], &_byte[len], sizeof(MQT)); 
				}
				if(index_w>=MQ_LEN) index_w=0; 
		}
		Index_r = index_r;		
		Index_w = index_w;
		return len;
}
/*_____________________________________________________________ API read ________________________________________________________________*/
int read(MQT* const _byte)
{
		int ret=-1;
	  uint16_t r_tmp;
		//__asm("CPSID  I");
		r_tmp = Index_r+1;
		if(r_tmp>=MQ_LEN) r_tmp=0;
		if(r_tmp != Index_w)
		{
				memcpy(_byte, &_value[r_tmp], sizeof(MQT)); //*_byte = _queue->buf[r_tmp];
				Index_r = r_tmp;
				ret = 0;
		}
		//__asm("CPSIE  I");
		return ret;
}
uint16_t reads(MQT buf[], const uint16_t buf_size)
{
		uint16_t index=0;
	  uint16_t index_r = Index_r;
		const uint16_t index_w = Index_w;
		memset(buf, 0, buf_size*sizeof(MQT));
		for(index=0; index<buf_size; index++)
		{
				index_r++;
				if(index_r>=MQ_LEN) index_r=0;
				if(index_w == index_r) break;  // empty
				Index_r = index_r;
				memcpy(&buf[index], &_value[index_r], sizeof(MQT));//buf[index] = _queue->buf[index_r++];
		}
		return index;
}
/*_____________________________________________________________ API safe ________________________________________________________________*/
void init_safe(void)
{ 
	Index_safe = Index_r;
}
uint16_t read_safe(MQT* const _byte)
{ 
	uint16_t index_r; 
	index_r = Index_safe+1; 
	if(index_r>=MQ_LEN) index_r=0; 
	if(index_r == Index_w) return 0;  /* empty */
	memcpy(_byte, &_value[index_r], sizeof(MQT)); 
	Index_safe = index_r; 
	return 1;
}
uint16_t reads_safe(MQT buf[], const uint16_t _size)  
{
		uint16_t index;
		uint16_t index_r = Index_safe;
		const uint16_t index_w = Index_w;
		//index_r = Index_safe;
		//memset(buf, 0, buf_size*sizeof(T)); 
		for(index=0; index<_size; index++) 
		{ 
				index_r++;
				if(index_r>=MQ_LEN) index_r=0; 
				if(index_w == index_r) break;  /* empty */ 
				Index_safe = index_r;
				memcpy(&buf[index], &_value[index_r], sizeof(MQT));//buf[index] = _queue->buf[index_r];  
		}
		return index;
}
// delete a _size member
void safe_del(const uint16_t _size)
{
	uint16_t index;
	uint16_t index_r = Index_r;
	const uint16_t index_w = Index_w;
	for(index=0; index<_size; index++)
	{
		index_r++;
		if(index_r>=MQ_LEN) index_r=0;
		if(index_w == index_r) break;  /* empty */
		Index_r = index_r;
	}
}
    //static const uint16_t MQ_LEN = N;
/*_____________________________________________________________ Operator overloading ________________________________________________________________*/
    /// Array operator accesses members.
    ///
    /// @note It would be nice to range-check i here, but then what would we return?
    ///
    const MQT & operator[](uint16_t i) {
        return _value[i];
    }

    const MQT & operator[](int16_t i) {
        return _value[(uint16_t)i];
    }

    /// Value getter
    ///
    /// @note   Returns zero for index values out of range.
    ///
    MQT get(uint16_t i) const {
        if (i < MQ_LEN) {
            return _value[i];
        } else {
            return (T)0;
        }
    }

    /// Value setter
    ///
    /// @note   Attempts to set an index out of range are discarded.
    ///
    void  set(uint8_t i, const MQT &v) {
        if (i < MQ_LEN) {
            _value[i] = v;
        }
    }

protected:
    //MQT _value[MQ_LEN];
};


/// Convenience macro for defining instances of the AP_ParamT template.
///
// declare a scalar type
// _t is the base type
// _suffix is the suffix on the AP_* type name
// _pt is the eMQ_LEN ap_var_type type
//#define AP_PARAMDEF(_t, _suffix, _pt)   typedef AP_ParamT<_t, _pt> AP_ ## _suffix;
//AP_PARAMDEF(float, Float, AP_PARAM_FLOAT);    // defines AP_Float
//AP_PARAMDEF(int8_t, Int8, AP_PARAM_INT8);     // defines AP_Int8
//AP_PARAMDEF(int16_t, Int16, AP_PARAM_INT16);  // defines AP_Int16
//AP_PARAMDEF(int32_t, Int32, AP_PARAM_INT32);  // defines AP_Int32

typedef mera_queue_cxx<uint8_t, 1024*24> MQ_24K;
typedef mera_queue_cxx<uint8_t, 4096+128> MQ_4096;
typedef mera_queue_cxx<uint8_t, 512+128> MQ_512;


#endif // __MERA_QUEUE_CXX_H__

