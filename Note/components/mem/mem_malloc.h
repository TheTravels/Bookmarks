/************************ (C) COPYLEFT 2018 Merafour *************************

* File Name          : mem_malloc.h
* Author             : Merafour
* Last Modified Date : 01/10/2019
* Description        : dynamic memory allocation library.
********************************************************************************
* https://merafour.blog.163.com
* merafour@163.com
* https://github.com/merafour
******************************************************************************/
#ifndef _MEM_MALLOC_H_
#define _MEM_MALLOC_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stddef.h>
#ifndef NULL
#define NULL 0
#endif

// ʹ�þ�̬�ڴ�
#define USE_MEM_STATIC   1
//#undef  USE_MEM_STATIC
// ʹ���ڴ��
#define USE_MEM_POOL   1
#undef  USE_MEM_POOL

#ifdef USE_MEM_STATIC
/**
 * ���η�������ڴ�����
 * 8bit: 2^8-1=255; 16bit: 2^16-1=65535; 32bit: 2^32-1, MCUû����ô���ڴ�,���ø�����
 * ����: 8bit ��� 16bit �ڴ��ɽ�ʡһ��,���ڴ��������������� 255ʱ�ɽ�ʡ�ڴ�,����ڵ�Ƭ��������
 * �ڴ����������: size >> MEM_BLOCK_WIDTH
 * ͬʱ��Ҫע�� MEM_BLOCK_WIDTH Խ�󼴵����ڴ��Խ��(2^MEM_BLOCK_WIDTH), �ڶ�̬�ڴ����������˷�Խ��,
 * ��Ϊ������ÿ�ζ�����ĸոպ�, �����ڵ�Ƭ����Ϊ�˼����ڴ��˷��������Ҫ���������С�ڴ����� MAX_BLOCK_SIZE �� MEM_BLOCK_WIDTH.
 * ͬʱ����ͨ�� mem_perused �����۲��ڴ�ʹ��������������ֵ.
 */
//#define MAX_BLOCK_SIZE_8bit     0x01
//#define MAX_BLOCK_SIZE_16bit    0x02
//#define MAX_BLOCK_SIZE_32bit    0x04
//#define MAX_BLOCK_SIZE   MAX_BLOCK_SIZE_16bit

extern uint8_t mem_perused(void);					            /* �ڴ�ʹ���� */
extern void  mem_free(void *ptr);  				                /* �ڴ��ͷ� */
extern void* mem_malloc(const uint32_t size);				    /* �ڴ���� */
extern void* mem_realloc(void *ptr, const uint32_t size);	    /* ���·����ڴ� */
/**    size_t    **/
extern void* smem_malloc(size_t size);				            /* �ڴ���� */
extern void* smem_realloc(void *ptr, size_t size);	            /* ���·����ڴ� */
extern void mem_test(void);
#endif

/**************************************** �ڴ�ش��� **********************************************/
#ifdef USE_MEM_POOL

// �ڴ�ض���
struct mem_pool{
		uint8_t map[256];
		uint8_t *ram;
		uint32_t map_size;
		uint32_t ram_size;
		uint8_t mbw; // mem_block_width;  // �ڴ����
};

extern void mem_pool_init(struct mem_pool*const _pool, uint8_t ram[], const uint32_t _rsize, const uint8_t _mbw);
extern uint8_t mem_pool_perused(const struct mem_pool*const _pool);					            /* �ڴ�ʹ���� */
extern void  mem_pool_free(struct mem_pool*const _pool, void *ptr);  				                /* �ڴ��ͷ� */
extern void* mem_pool_malloc(struct mem_pool*const _pool, const uint32_t size);				    /* �ڴ���� */
extern void* mem_pool_realloc(struct mem_pool*const _pool, void *ptr, const uint32_t size);	    /* ���·����ڴ� */
extern void mem_pool_test(uint8_t ram[], const uint32_t _rsize);
#endif

#ifdef __cplusplus
}
#endif

#endif /* _MEM_MALLOC_H_ */

