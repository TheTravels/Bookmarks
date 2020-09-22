/************************ (C) COPYLEFT 2018 Merafour *************************
* File Name          : can_link.c
* Author             : Merafour
* Last Modified Date : 09/21/2020
* Description        : �������ݵ�Ԫ����.
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
		uint32_t  lat;    // γ��,����:0.00001��,��ʮ���֮һ
		uint32_t  lon;    // ����,����:0.00001��,��ʮ���֮һ
		uint32_t  utc;    // UTC ʱ���,����
		union {
			uint32_t status;
			struct {
				uint32_t  NLat:1;    // γ�ȷ���, 1: N-��, 0��S-��
				uint32_t  ELon:1;    // ���ȷ���, 1��E-��, 0: W-��
				//uint32_t  valid:1;   // ��λ��Ч��־
				uint32_t  numSv:5;   // ������ 0-24
				uint32_t  FS:3;      // ��λ����, 0, 1, 6
				uint32_t  HDOP:5;    // ˮƽ��������
				uint32_t  msl:13;    // ���θ߶�,���� 1m
				uint32_t  rev:4;     // ����
			};
		};
};

#ifdef __cplusplus
}
#endif

#endif //_CACHE_QUEUE_CAN_DATA_H_
