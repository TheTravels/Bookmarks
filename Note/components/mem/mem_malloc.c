/************************ (C) COPYLEFT 2018 Merafour *************************

* File Name          : mem_malloc.c
* Author             : Merafour
* Last Modified Date : 01/10/2019
* Description        : dynamic memory allocation library.
********************************************************************************
* https://merafour.blog.163.com
* merafour@163.com
* https://github.com/merafour
******************************************************************************/
#include "mem_malloc.h"
#include <string.h>
#include "Periphs/uart.h"

#ifdef USE_MEM_STATIC
/**
 * 内存块宽度, 内存块大小(2^MEM_BLOCK_WIDTH), 单位字节.用于加快内存分配时的运算速度
 */
#define MEM_BLOCK_WIDTH			4
/**
 * 最大管理内存 10K
 */
#define MEM_MAX_SIZE			(40*1024)

//#define MEM_BLOCK_SIZE			(0x1<<MEM_BLOCK_WIDTH)                  /* 内存块大小(2^MEM_BLOCK_WIDTH), 单位字节 */
/*#define MEM_ALLOC_TABLE_SIZE	MEM_MAX_SIZE/MEM_BLOCK_SIZE 	         内存表大小 */
#define MEM_ALLOC_TABLE_SIZE	(((MEM_MAX_SIZE)>>MEM_BLOCK_WIDTH)/*+1*/) 	/* 内存表大小 */

/* mem align 4 byte */
/*__align(4) static uint8_t mem_ram[MEM_ALLOC_TABLE_SIZE<<MEM_BLOCK_WIDTH]={0};			   */ /* SRAM内存池 */
//static uint8_t __attribute__ ((aligned (16))) mem_ram[MEM_ALLOC_TABLE_SIZE<<MEM_BLOCK_WIDTH]= {0};			   /* SRAM内存池 */
static uint8_t mem_ram[MEM_ALLOC_TABLE_SIZE<<MEM_BLOCK_WIDTH] __attribute__((at(0x10006000)));;			   /* CCM内存池 */
/* SRAM内存表 */
// 1bit is one block
static uint8_t mem_map[(MEM_ALLOC_TABLE_SIZE/8)+1]= {0};
/* 内存管理参数 */
static const uint32_t mem_map_size=MEM_ALLOC_TABLE_SIZE;		    /* 内存表大小 */
static uint8_t ready = 0;

/* 内存管理初始化 */
static void init(void)
{
	memset(mem_map, 0,sizeof(mem_map));
	memset(mem_ram, 0, sizeof(mem_ram));
	ready = 1;
}
// 检查块是否为空
static __inline int mem_is_empty(const uint32_t _block_num)
{
		const uint32_t index = _block_num>>3;
		const uint8_t pos = _block_num&((0x1<<3)-1);
		const uint8_t map = mem_map[index];
		//app_debug("[%s--%d] _block_num:%d map:0x%08X index:%d pos:%d (0==((map>>pos)&0x01)):%d \r\n", __func__, __LINE__, _block_num, map, index, pos, (0==((map>>pos)&0x01)));
		return (0==((map>>pos)&0x01));
}
// 设置块被使用
static __inline void mem_in_use(const uint32_t _block_num)
{
		const uint32_t index = _block_num>>3;
		const uint8_t pos = _block_num&((0x1<<3)-1);
		uint8_t map = mem_map[index];
		map |= (0x01<<pos);
		mem_map[index] = map;
}
// 设置块为空闲
static __inline void mem_in_empty(const uint32_t _block_num)
{
		const uint32_t index = _block_num>>3;
		const uint8_t pos = _block_num&((0x1<<3)-1);
		uint8_t map = mem_map[index];
		map &= (~(0x01<<pos));
		mem_map[index] = map;
}
/**
 * 获取内存使用率
 * 返回值:使用率(0~100)
*/
uint8_t mem_perused(void)
{
	uint32_t used=0;
	uint32_t i;
	//app_debug("[%s--%d] \r\n", __func__, __LINE__);
	for(i=0; i<mem_map_size; i++)
	{
		//if(mem_map[i])used+=100;
		if(0==mem_is_empty(i)) used+=100;
	}
	//app_debug("[%s--%d] used:%d mem_map_size:%d \r\n", __func__, __LINE__, used, mem_map_size);
	return (uint8_t)((used)/(mem_map_size));
}
void* mem_malloc(const uint32_t _size)
{
	uint32_t offset=0;
	uint32_t block_size;
	uint32_t block_addr;
	uint16_t block_cnt=0;
	uint32_t i;
	const uint32_t size = _size+16; // 多申请的字节用于保存内存大小信息
	if(!ready) init();
	//app_debug("[%s--%d] mem_map_size:%d MEM_MAX_SIZE:%d size:%d \r\n", __func__, __LINE__, mem_map_size, MEM_MAX_SIZE, size);
	if(size==0) return NULL;

	block_size=size>>MEM_BLOCK_WIDTH;
	if(size&((0x1<<MEM_BLOCK_WIDTH)-1)) block_size++;
	//app_debug("[%s--%d] block_size:%d \r\n", __func__, __LINE__, block_size);
	for(offset=0; offset<(mem_map_size-1); offset++)//for(offset=mem_map_size-1; ; offset--) // search empty mem
	{
		//app_debug("[%s--%d] offset:%d \r\n", __func__, __LINE__, offset);
		if((offset+block_size)>=mem_map_size) break;
		for(i=0, block_cnt=0; i<block_size; i++)
		{
				if(mem_is_empty(offset+i))block_cnt++;
				else break;
		}
		if(block_cnt==block_size)	       // search ok
		{
			// mark using
			for(i=0; i<block_size; i++) mem_in_use(offset+i);
			block_addr = offset<<MEM_BLOCK_WIDTH;
			//app_debug("[%s--%d] block_cnt:%d offset:%d \r\n", __func__, __LINE__, block_cnt, offset);
			memcpy(&mem_ram[block_addr], &block_size, 4);
			return &mem_ram[block_addr+4];
		}
	}
	return NULL;
}
static inline void _free(const uint32_t block_offset)
{
	uint32_t i;
	//uint32_t block_size=mem_map[block_offset];
	uint32_t block_size;
	memcpy(&block_size, &mem_ram[block_offset<<MEM_BLOCK_WIDTH], 4);
	for(i=0; i<block_size; i++)
	{
		// mark empty mem
		//mem_map[block_offset+i]=0;
		mem_in_empty(block_offset+i);
	}
}
void mem_free(void *ptr)
{
	uint32_t offset;
	if(ptr==NULL) return ;
	offset=(uint32_t)((uint8_t*)ptr-(uint8_t*)mem_ram);
	if(!ready)
	{
		init();
		return ;
	}
	if(offset<sizeof(mem_ram))
	{
		// free, block addr: offset>>MEM_BLOCK_WIDTH
		_free(offset>>MEM_BLOCK_WIDTH);
	}
}

void* mem_realloc(void *ptr, const uint32_t size)
{
	void* _mem=NULL;
	_mem = mem_malloc(size);
	if(NULL==_mem)
	{
		return NULL;
	}
	else
	{
		memcpy(_mem,ptr,size);	// copy data
		mem_free(ptr);  		// free old mem
		return _mem;  			// new mem
	}
}

void* smem_malloc(size_t size)				            /* 内存分配 */
{
	return mem_malloc((uint32_t)size);
}
void* smem_realloc(void *ptr, size_t size)	            /* 重新分配内存 */
{
	return mem_realloc(ptr, (uint32_t)size);
}
#if 0
void mem_test(void)
{
	char *buf1 = NULL;
	char *buf2 = NULL;
//	app_debug("[%s--%d] mem:0x%08X \r\n", __func__, __LINE__, mem_malloc(4096));
//	app_debug("[%s--%d] perused:%d\r\n", __func__, __LINE__, mem_perused());
	buf1 = mem_malloc(8096);
	buf2 = mem_malloc(4096);
	app_debug("[%s--%d] buf1:0x%08X buf2:0x%08X \r\n", __func__, __LINE__, buf1, buf2);
	app_debug("[%s--%d] perused:%d\r\n", __func__, __LINE__, mem_perused());
	mem_free(buf1);
	app_debug("[%s--%d] perused:%d\r\n", __func__, __LINE__, mem_perused());
	buf1 = mem_malloc(1024*5);
	app_debug("[%s--%d] buf1:0x%08X buf2:0x%08X \r\n", __func__, __LINE__, buf1, buf2);
	app_debug("[%s--%d] perused:%d\r\n", __func__, __LINE__, mem_perused());
	mem_free(buf2);
	app_debug("[%s--%d] perused:%d\r\n", __func__, __LINE__, mem_perused());
}
#endif
#endif
/**************************************** 内存池处理 **********************************************/
#ifdef USE_MEM_POOL
void mem_pool_init(struct mem_pool*const _pool, uint8_t ram[], const uint32_t _rsize, const uint8_t _mbw)
{
		uint32_t map_size = _rsize>>_mbw; // 块数量
		uint32_t map_size_max = sizeof(_pool->map)<<3;
		if(map_size>map_size_max) map_size = map_size_max;
		memset(_pool, 0, sizeof(struct mem_pool));
		memset(ram, 0, _rsize);
		_pool->mbw = _mbw;
		_pool->map_size = map_size;
		_pool->ram_size = map_size<<_mbw;
		_pool->ram = &ram[0];
}
// 检查块是否为空
static __inline int mem_pool_is_empty(const struct mem_pool*const _pool, const uint32_t _block_num)
{
		const uint32_t index = _block_num>>3;
		const uint8_t pos = _block_num&((0x1<<3)-1);
		const uint8_t map = _pool->map[index];
		//app_debug("[%s--%d] _block_num:%d map:0x%08X index:%d pos:%d (0==((map>>pos)&0x01)):%d \r\n", __func__, __LINE__, _block_num, map, index, pos, (0==((map>>pos)&0x01)));
		return (0==((map>>pos)&0x01));
}
// 设置块被使用
static __inline void mem_pool_in_use(struct mem_pool*const _pool, const uint32_t _block_num)
{
		const uint32_t index = _block_num>>3;
		const uint8_t pos = _block_num&((0x1<<3)-1);
		uint8_t map = _pool->map[index];
		//app_debug("[%s--%d] _block_num:%d 0x%08X index:%d pos:%d\r\n", __func__, __LINE__, _block_num, _block_num, index, pos);
		map |= (0x01<<pos);
		_pool->map[index] = map;
		//app_debug("[%s--%d] _block_num:%d map:0x%08X \r\n", __func__, __LINE__, _block_num, map);
}
// 设置块为空闲
static __inline void mem_pool_in_empty(struct mem_pool*const _pool, const uint32_t _block_num)
{
		const uint32_t index = _block_num>>3;
		const uint8_t pos = _block_num&((0x1<<3)-1);
		uint8_t map = _pool->map[index];
		map &= (~(0x01<<pos));
		_pool->map[index] = map;
		//app_debug("[%s--%d] _block_num:%d map:0x%08X \r\n", __func__, __LINE__, _block_num, map);
}
/**
 * 获取内存使用率
 * 返回值:使用率(0~100)
*/
uint8_t mem_pool_perused(const struct mem_pool*const _pool)
{
	uint32_t used=0;
	uint32_t i;
	//app_debug("[%s--%d] \r\n", __func__, __LINE__);
	for(i=0; i<_pool->map_size; i++)
	{
		if(0==mem_pool_is_empty(_pool, i)) used+=100;
	}
	//app_debug("[%s--%d] used:%d map_size:%d \r\n", __func__, __LINE__, used, _pool->map_size);
	return (uint8_t)((used)/(_pool->map_size));
}
void* mem_pool_malloc(struct mem_pool*const _pool, const uint32_t size)
{
	uint32_t offset=0;
	uint32_t block_size;
	uint32_t block_addr;
	uint16_t block_cnt=0;
	uint32_t i;
	//app_debug("[%s--%d] map_size:%d ram_size:%d size:%d \r\n", __func__, __LINE__, _pool->map_size, _pool->ram_size, size);
	if(size==0) return NULL;

	block_size=size>>_pool->mbw;
	if(size&((0x1<<_pool->mbw)-1)) block_size++;
	//app_debug("[%s--%d] block_size:%d \r\n", __func__, __LINE__, block_size);
	for(offset=0; offset<(_pool->map_size-1); offset++)//for(offset=mem_map_size-1; ; offset--) // search empty mem
	{
		//app_debug("[%s--%d] offset:%d \r\n", __func__, __LINE__, offset);
		if((offset+block_size)>=_pool->map_size) break;
		for(i=0, block_cnt=0; i<block_size; i++)
		{
				if(mem_pool_is_empty(_pool, offset+i))block_cnt++;
				else break;
		}
		if(block_cnt==block_size)	       // search ok
		{
				// mark using
				for(i=0; i<block_size; i++) mem_pool_in_use(_pool, offset+i);
				block_addr = offset<<_pool->mbw;
				//app_debug("[%s--%d] block_cnt:%d offset:%d \r\n", __func__, __LINE__, block_cnt, offset);
				memcpy(&_pool->ram[block_addr], &block_size, 4);
				return &_pool->ram[block_addr+4];
		}
	}
	return NULL;
}
static inline void _pool_free(struct mem_pool*const _pool, const uint32_t block_offset)
{
	uint32_t i;
	//uint32_t block_size=mem_map[block_offset];
	uint32_t block_size;
	memcpy(&block_size, &_pool->ram[block_offset<<_pool->mbw], 4);
	for(i=0; i<block_size; i++)
	{
		// mark empty mem
		mem_pool_in_empty(_pool, block_offset+i);
	}
}
void mem_pool_free(struct mem_pool*const _pool, void *ptr)
{
	uint32_t offset;
	if(ptr==NULL) return ;
	offset=(uint32_t)((uint8_t*)ptr-(uint8_t*)_pool->ram);
	if(offset<_pool->ram_size)
	{
		// free, block addr: offset>>MEM_BLOCK_WIDTH
		_pool_free(_pool, offset>>_pool->mbw);
	}
}

void* mem_pool_realloc(struct mem_pool*const _pool, void *ptr, const uint32_t size)
{
		void* _mem=NULL;
		_mem = mem_pool_malloc(_pool, size);
		if(NULL==_mem) return NULL;
		memcpy(_mem, ptr, size);	      // copy data
		mem_pool_free(_pool, ptr);  		// free old mem
		return _mem;  			            // new mem
}
#if 0
void mem_pool_test(uint8_t ram[], const uint32_t _rsize)
{
	struct mem_pool _mem_pool;
	char *buf1 = NULL;
	char *buf2 = NULL;
	mem_pool_init(&_mem_pool, ram, _rsize-2048, 6);
	//mem_pool_init(&_mem_pool, RxcpBuffer, 4096, 6);
//	app_debug("[%s--%d] mem:0x%08X \r\n", __func__, __LINE__, mem_malloc(4096));
//	app_debug("[%s--%d] perused:%d\r\n", __func__, __LINE__, mem_perused());
	buf1 = mem_pool_malloc(&_mem_pool, 8096);
	buf2 = mem_pool_malloc(&_mem_pool, 4096);
	app_debug("[%s--%d] buf1:0x%08X buf2:0x%08X \r\n", __func__, __LINE__, buf1, buf2);
	app_debug("[%s--%d] perused:%d\r\n", __func__, __LINE__, mem_pool_perused(&_mem_pool));
	mem_pool_free(&_mem_pool, buf1);
	app_debug("[%s--%d] perused:%d\r\n", __func__, __LINE__, mem_pool_perused(&_mem_pool));
	buf1 = mem_pool_malloc(&_mem_pool, 1024*5);
	app_debug("[%s--%d] buf1:0x%08X buf2:0x%08X \r\n", __func__, __LINE__, buf1, buf2);
	app_debug("[%s--%d] perused:%d\r\n", __func__, __LINE__, mem_pool_perused(&_mem_pool));
	mem_pool_free(&_mem_pool, buf2);
	app_debug("[%s--%d] perused:%d\r\n", __func__, __LINE__, mem_pool_perused(&_mem_pool));
	mem_pool_free(&_mem_pool, buf1);
	app_debug("[%s--%d] perused:%d\r\n", __func__, __LINE__, mem_pool_perused(&_mem_pool));
}
#endif
#endif
