/**
  ******************************************************************************
  * File Name          : cache_queue_can.h
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#ifndef _CACHE_QUEUE_CAN_DATA_H_
#define _CACHE_QUEUE_CAN_DATA_H_

#include <stdint.h>

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

#endif //_CACHE_QUEUE_CAN_DATA_H_
