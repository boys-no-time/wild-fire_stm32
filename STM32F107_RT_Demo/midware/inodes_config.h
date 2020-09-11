/*********************************************************************
Copyright (c) 2020-2028 by inodes. All rights reserved. 
You are not allowed to copy or distribute the code without permission. 
*********************************************************************/

#ifndef __INODES_CONFIG_H__
#define __INODES_CONFIG_H__

/*********************************************************************
	本文件只设置基本信息和配置功能模块，严禁添加其他信息！！
        PROJ_XXX作为项目的唯一标志，只能在Keil的"Options for Target..."
	的选项卡"C/C++"中定义,或在在线编译的CFLAGS中定义,不可在代码里写死! 
*********************************************************************/

/* 微库("Options..." => "Target" => "Use MicroLIB")除了代码紧凑外，有很多缺陷，所以
** 内存充足的项目都要避免使用它(亦即：CFLAGS不要添加-D__MICROLIB)。
** 这时，Keil默认使用半主机功能，导致嵌入式设备上使用出现问题(Semihosting code can
** cause a HardFault on systems that are unable to handle semihosting code)，所以也
** 要使用下面的directive显式指定不使用半主机模式. 为此有几个函数(链接时提示找不到的
** 函数，如_sys_exit(), fputc(), etc)需要在inodes_sdk.c中实现对应的功能或者做一个空
** 函数(stub; 仅使链接通过而不实现功能). 
** 注：使用__use_no_semihosting_swi跟__use_no_semihosting效果是一样的。 */
#if defined(__ARMCC_VERSION) //see Compiler's User Guide: Predefined macros
	/* ARMCC版本号: Mmmbbbb, M-Major mm-minor, bbbb-build */
	#if (__ARMCC_VERSION < 6000000) //ARMCC 5.x
	#pragma import(__use_no_semihosting)
	#else //ARMCC 6不支持pragma import(__use_no_semihosting)
	__asm(".global __use_no_semihosting\n\t");
	#endif
	#pragma import(__ARM_use_no_argv)	//指定main()没有参数(与MicroLIB不兼容)
#elif defined(__GNUC__)
	/* use respective options while linking:
	** If you need semihosting, linking like:
		$ arm-none-eabi-gcc --specs=rdimon.specs $(OTHER_LINK_OPTIONS)
	** you are using non-semihosting/retarget, linking like:
		$ arm-none-eabi-gcc --specs=nosys.specs $(OTHER_LINK_OPTIONS)
	*/
#endif

/* Keil消除一些不必要的警告
	#177-D: function "xxx" was declared but never referenced
	#223-D: function "yyy" declared implicitly
#ifdef __CC_ARM
#pragma diag_suppress 177, 223
#endif
*/

/* 通用设置 *********************************************************/
//使用24位MCU-ID
#define INODES_HAVE_24_ID

/*===========================================================================
新电源控制器项目: 支持网卡和蓝牙 
===========================================================================*/
#if defined(PROJ_NEWPWRCTRL) //新钥匙箱项目标志宏

/* 基本信息 *********************************************************/

/* 1. software relative -------------------- */
//硬件版本
#define HARDWARE_VERSION "00-00-04"
//软件版
#define SOFTWARE_VERSION "1-00-00"
//协议版本号
#define PROCOTOL_VERSION "00-00-02"
//产品型号
#define PRODUCT_MODEL "INODES_NEWKEYBOX"
//建造时间
#define INODES_BUILD_TIME "2020-08-25"

/* 2. hardware relative -------------------- */
/* stm32f10x.h中有SRAM_BASE/FLASH_BASE的定义。(0x20000000~0x200000BF是复制中断
   向量表的区域(192字节); SystemCoreClock放在0x200000C0,用4个字节) */

/* 请仔细读data sheet来正确设置下面的几个值, 否则可能后果严重 */
//系统SRAM起始地址
#define SRAM_BASE_ADDR		(0x20000000)
//系统SRAM size(注意0xC4之前APP不可用)
#define SRAM_TOTAL_SIZE		(0x10000)               //0x20000000 - 0x2000FFFF
//系统FLASH起始地址
#define FLASH_BASE_ADDR 	(0x08000000)
//系统FLASH总size
#define FLASH_TOTAL_SIZE  	(0x40000) 					//256K        0x08000000 - 0x0803FFFF
//FLASH页大小(擦、写的最小粒度)
#define FLASH_PAGE_SIZE 	(2048)								//128 pages of 2Kbytes eash
//FLASH sector大小(写保护的最小粒度)
#define FLASH_SECTOR_SIZE	(4096)								//2pages for hign density and connectivity lines  4pages for low and medium density

/* 注: 其他详细信息(如COM，FLASH, etc)在各自实现的头文件中设置. */

/* 功能模块 *********************************************************/
/* 1. 默认打开/支持的功能模块(需要指定关闭):
		DEBUG	调试打印
		FLASH	读写FLASH
		MQTT	MQTT协议
		RTC		RTC时钟日历
		HW_RTC	硬件RTC(去掉则软件模拟; 如果RTC关闭则无效)
** 去掉默认功能模块请使用INODES_NO_YYY(YYY取上面的某值) */
#define INODES_NO_HW_RTC

/* 2. 需要显式指定才支持的功能模块 */
//IAP(Inter-Application Programming)中断向量表预留(支持在线升级)
#define IAP_SLAVE
#define INODES_HAVE_LED_CTRL
#define INODES_HAVE_BEEP_CTRL


//支持LAN/BT(网卡和蓝牙)
//#define INODES_HAVE_ETH_BT

#else //project identity missed!!! ==========================================
#error "你必须定义正确的项目标志宏!!!"

#endif

#endif //__INODES_CONFIG_H__

