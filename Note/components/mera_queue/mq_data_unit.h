/************************ (C) COPYLEFT 2018 Merafour *************************
* File Name          : can_link.c
* Author             : Merafour
* Last Modified Date : 09/21/2020
* Description        : 队列数据单元定义.
********************************************************************************
* https://merafour.blog.163.com
* merafour@163.com
* https://github.com/merafour
******************************************************************************/
#ifndef _CACHE_QUEUE_CAN_DATA_H_
#define _CACHE_QUEUE_CAN_DATA_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*_____________________________________________________________ CAN ________________________________________________________________*/
struct cache_can_rxbuf{
		uint32_t id;
		uint32_t rx_time;
		uint8_t data[8];
		uint8_t DLC : 4;
		uint8_t IDE : 1;
		uint8_t RTR : 1;
		uint8_t recv : 2;
		//uint8_t flag;
};
/*_____________________________________________________________ GPS ________________________________________________________________*/
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

#ifdef __cplusplus
}
#endif

#endif //_CACHE_QUEUE_CAN_DATA_H_
