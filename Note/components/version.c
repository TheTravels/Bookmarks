/******************** (C) COPYRIGHT 2015 merafour ********************
* File Name          : version.c
* Author             : 冷月追风 && Merafour
* Version            : V1.0.0
* Last Modified Date : 07/04/2020
* Description        : 版本信息.
********************************************************************************
* https://merafour.blog.163.com
* merafour@163.com
* https://github.com/merafour
*******************************************************************************/
#include "version.h"

/********************************** 更新记录 **********************************
* V2.1.4             : ---- 2020.07.22
*                    : 1. 更改 EC20初始化；
*                    : 2. 更改 EC20默认波特率为 115200,模块上电后临时更改为 460800；
*                    : 3. 更改多帧处理为在缓存中检索数据帧；
*                    : 4. 更改参数为参数表,并分块管理；
*                    : 5. 合并中断中处理心跳包；
*                    : 6. IP 端口可配置；
*                    : 7. 断网重连；
* V2.2.0             : ---- 2020.07.23
*                    : 1. 在串口中断中检测断网消息；
*                    : 2. 使用多任务处理心跳包；
* V2.2.1             : ---- 2020.07.23
*                    : 3. 在重启模块之前检测有无网络；
*                    : 4. 检测质检标志,区分生产和发行；
* V2.13.0            : 
*                    : 1. 4G 模块接收改用循环缓存；
*                    : 2. 发现 "StrToHex(RxcpBuffer, H4G_RxBuffer, H4G_RxLength);" 参数 H4G_RxLength错误,正确应为:(H4G_RxLength/2)；
* V2.14.0            : 
*                    : 1. 增加 SD卡模块和调试模块；
* V2.15.x            : 
*                    : 0. 增加调试功能；
*                    : 1. 更新 EC20驱动代码,并增加 LED绿色提示；
*                    : 2. 更新串口1发送部分；
*                    : 3. 优化数据解析部分；
*                    : 4. 优化数据处理部分；
*                    : 5. 改用循环队列接收数据；
*                    : 6. 合并廖工的康明斯协议代码；
*                    : 7. 改断网重启为断网重连；
*                    : 8. 增加 USB虚拟串口调试；
*                    : 9. 增加 USB作为通信口；
*                    : 10. USB可进入主循环后选择作为调试或通信口；
*                    : 11. USB可作为唯一的通信口用于本地开发调试；
*                    : 12. 第二次合并廖工康明斯代码,并更改 4G接收支持小写；
*                    : 13. 限制单次上传的数据亮最大不超过 (1024-100)；
*                    : 14. GPS定位功能已确认；
*                    : 15. 自动帧测波特率,支持:115200,230400,460800,921600,9600,19200,38400,57600；
*                    : 16. 更改USB通信为 ASCII方式,USB上电自动拔插,更新can模块延时；
*                    : 17. 优化4G超时重连,无SIM卡时输出调试信息提示；
*                    : 18. 排查无广播数据,第三次合并廖工代码,CAN使用循环队列接收,并优化数据上报；
*                    : 19. 优化CAN部分代码；
*                    : 20. 分离CAN通信的硬件驱动和应用层；
*                    : 21. 重构CAN硬件驱动代码；
*                    : 22. 重新核对数据解析；
*                    : 23. 经排查从17次修订开始不能刷写,现基于第17次修订重新修订,将CAN缓存数组改为结构数组；
*                    : 24. 数组改队列,测试可刷写；
*                    : 25. 优化队列操作,CAN1刷写已测试；
*                    : 26. 刷写和广播已测试；
*                    : 27. 移除不使用的 DataSendBuff 缓存；
*                    : 28. 分离CAN2驱动；
*                    : 29. 分离标准CAN协议；
*                    : 30. 分离康明斯协议；
*                    : 31. 分离CAN1驱动；
*                    : 32. 设备休眠于唤醒功能测试,ADC测试；
*                    : 33. 增加低压休眠功能；
*                    : 34. 更新CDC通信切换；
*                    : 35. 消除代码编译警告；
*                    : 36. 广播和刷写已测试；
*                    : 37. 同步第22修订的CAN部分代码；
*                    : 38. 重构标准CAN数据发送；
*                    : 39. 根据 UDS 协议优化 CAN1 否定应答处理；
*                    : 40. 改 App 参数为参数表形式,并删除部分被注释代码(发行版)；
* V2.16.x            : 
*                    : 0. 增加多任务模型；
*                    : 1. 将main函数中的部分功能改用多任务；
*                    : 2. 增加多帧解码接口,简化主循环；
*                    : 3. 使用多任务处理锁机；
*                    : 4. 增加调试信息观察 CAN 发送失败出错信息；
*                    : 5. 提取 CAN模块公共代码；
*                    : 6. 分离 CAN发送接收代码使之独立成为模块；
*                    : 7. CAN1 CH6 CL14, CAN2 CH3 CL11接收广播测试；
*                    : 8. CAN1 CH6 CL14, CAN2 CH3 CL11收发测试；
*                    : 9. CAN引脚由参数指定；
*                    : 10. 删除测试用广播接收,USB 未连接超时关闭；
*                    : 11. 删除部分被注释的代码,bin并设置CDC为调试口(发行版)；
* V2.17.x            : 
*                    : 0. 增加内存管理模块；
*                    : 1. CCM使用动态内存管理；
*                    : 2. 康明斯部分改用新的CAN通讯接口；
*                    : 3. 更新调试信息和增加康明斯单帧标志,并修复康明斯部分直接对CAN1接口的调用改为指针形参,增加刷写boot测试代码；
*                    : 4. 增加刷写 Boot的版本校验；
*                    : 5. 统一4G和CDC数据解析,b并去掉 CAN数据发送中的注释测试刷写；
*                    : 6. 增加更新Boot参数表接口；
*                    : 7. 修正历史参数表索引(发行版)；
* V2.18.x            : 
*                    : 0. 使用 INCBIN 指令测试编译 boot二进制文件,并重新编写boot更新接口；
*                    : 1. 独立看门狗测试,并修改boot参数表更新接口支持修改SN号；
*                    : 2. 使用 debug 控制调试接口是否初始化,并重新整理主函数对命令字的处理,修复参数引用错误导致的二级boot重复刷写；
*                    : 3. 删除 can1_drv_read_new_safe 接口的直接引用；
*                    : 4. 用结构封装 CAN接口；
*                    : 5. CAN 接收数据改用接口操作 CAN；
*                    : 6. 改CAN初始化为初始化完成再使能CAN收发器；
*                    : 7. 分离更新boot接口,系统延时函数单独提取称为模块；
*                    : 8. 检测 SD卡；
*                    : 9. 文件系统测试；
*                    : 10. 增加日志功能；
*                    : 11. GPS模块测试；
*                    : 12. RTC测试；
*                    : 13. 将GPS数据保存为日志；
*                    : 14. 优化 GPS 初始化；
*                    : 15. 康明斯测试；
*                    : 16. GPS模块自动检测波特率；
*                    : 17. CAN刷写重测,解决GPS速率过高导致的4G接收大数据不完整,并清理部分已注释代码；
*                    : 18. 简化main函数,经测试SDIO中断将导致接收 4G模块大数据不完整,此处关闭 GPS日志功能,17版本由于GPS被误关闭导致日志功能关闭故能正常刷写；
*                    : 19. 优化解码,自动侦测 A5A5包头(注:若非必要,数据部分不要出现A5A5和55等数据,会干扰帧自动检测)；
*                    : 20. SDIO使用 DMA传输,重新开启 GPS日志功能,优化GPS数据解析,提高GPS通信波特率；
*                    : 21. 合并廖工代码 App_V2.18.18_LZS_1.0.3；
*                    : 22. GPS使用大缓存；
*                    : 23. 本地解 GPS 数据；
*                    : 24. 合并 App??_V2.18.19_he 代码；
*                    : 24A. 合并 Appxx_V2.18.24B(标准 CAN在主循环处理接收)；
*                    : 25. 合并 Appxx_V2.18.25B(重新解析标准CAN数据)；
*                    : 26. 合并 Appxx_V2.18.26B(更新标准 CAN数据解码)；
*                    : 27. 合并 Appxx_V2.18.27B(将刷写时的心跳包放在delay函数处理),28B(处理 GPS 波特率检测和数据处理的长度错误);
*                    : 28. 使用 C++类模板编写队列(旨在减少不同数据队列的重复编码), GPS部分使用 C++ 模板队列;
*                    : 28A. 串口使用模板队列,增加循环队列模板的 C实现;
*                    : 28B. CAN 使用模板队列 C实现;
*                    : 28D. 增加队列的非内联函数支持;
*******************************************************************************/

// define version data section
//#define _version __attribute__((unused, section(".version")))
#define _version __attribute__((unused, section(".version")))


/*
 * Flash map
 * bin       |-----------------| start
 * uint32_t 0| __initial_sp    | 000H
 *          1| Reset_Handler   | 004H
 *          2| Flash KB        | 008H  | swd| erase| flashH| flashL|
 *           |-----------------|
 *          3|                 | 00CH
 *        ...| mtext           | ...H
 *         10|                 | {8*4}B
 *           |-----------------|
 *         11| empty           |
 *           |-----------------|
 *         12| KEY_GPIO        | 030H
 *         13| KEY_Pin         |       | Low| Pin|
 *         14| LED1_GPIO       |
 *         15| LED1_Pin        |       | Low| Pin|
 *         16| LED2_GPIO       |
 *         17| LED2_Pin        |       | Low| Pin|
 *           |-----------------|
 *         18|                 | 48H
 */
//static int led_low=0;
//static int key_low=0;
//static uint32_t swd=1;
const struct Emb_EPS_418_Version _version __attribute__ ((aligned (512))) EPS_418_Version __attribute__((section(".ARM.__at_0x08010200"))) = {
		.Emb = {
			.version = "2.18.28D-hw1.0-HSH.Gen",     // Boot Version, eg:"2.1.10-hw2.0-HSH.Gen"
			.model = "52",                       // Device Model,eg: "0A0"
			.author = "Merafour",                // Developers
			.hardware = "A108",                  // Board name
			.mtext = {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF},      // boot signature Key
			.signData = {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF},   // boot signature
			.signApp = {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF},    // firmware signature
			.crc = 0xFFFFFFFF,
			//.swd = 0,                          // Debug enable
			.erase = 0,                          //
			.flash = 256/2,                      // Flash map addr, 2K
			.mount = 256/2,                      // Flash mount size, 4K
			.vbus = 0xA9,                        // VBUS Port && Pin, PA9
			.led  = 0xDD,                        // LED Port && Pin,  PD13
			.cfg = {
					.swd = 1,
					.vbus = 0,
					.led = 1,
					//.debug = EMB_DEBUG_UART3,    // Debug out UART3
					//.debug = EMB_DEBUG_NONE,     // 无调试信息
					.debug = EMB_DEBUG_UART2,//EMB_DEBUG_UART2,//EMB_DEBUG_CDC,    // Debug out CDC
			},
			.reserve1 = 0x55,
			.reserve2 = 0x55,
	},
#if 1
	.host = "47.107.60.248",
	.port = 20006,
#else
	.host = "120.78.134.213", //KMS SEVER FOR LZS
	.port = 20020,
#endif
	//.host = "120.78.134.213",
	//.port = 20015,
	.ftp_host = "39.108.51.99",
	.ftp_port = 21,
};


/************************ (C) COPYRIGHT Merafour *****END OF FILE****/

