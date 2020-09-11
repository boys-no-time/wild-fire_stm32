/*********************************************************************
Copyright (c) 2020-2028 by inodes. All rights reserved. 
You are not allowed to copy or distribute the code without permission. 
*********************************************************************/

#ifndef __INODES_CONFIG_H__
#define __INODES_CONFIG_H__

/*********************************************************************
	���ļ�ֻ���û�����Ϣ�����ù���ģ�飬�Ͻ����������Ϣ����
        PROJ_XXX��Ϊ��Ŀ��Ψһ��־��ֻ����Keil��"Options for Target..."
	��ѡ�"C/C++"�ж���,�������߱����CFLAGS�ж���,�����ڴ�����д��! 
*********************************************************************/

/* ΢��("Options..." => "Target" => "Use MicroLIB")���˴�������⣬�кܶ�ȱ�ݣ�����
** �ڴ�������Ŀ��Ҫ����ʹ����(�༴��CFLAGS��Ҫ���-D__MICROLIB)��
** ��ʱ��KeilĬ��ʹ�ð��������ܣ�����Ƕ��ʽ�豸��ʹ�ó�������(Semihosting code can
** cause a HardFault on systems that are unable to handle semihosting code)������Ҳ
** Ҫʹ�������directive��ʽָ����ʹ�ð�����ģʽ. Ϊ���м�������(����ʱ��ʾ�Ҳ�����
** ��������_sys_exit(), fputc(), etc)��Ҫ��inodes_sdk.c��ʵ�ֶ�Ӧ�Ĺ��ܻ�����һ����
** ����(stub; ��ʹ����ͨ������ʵ�ֹ���). 
** ע��ʹ��__use_no_semihosting_swi��__use_no_semihostingЧ����һ���ġ� */
#if defined(__ARMCC_VERSION) //see Compiler's User Guide: Predefined macros
	/* ARMCC�汾��: Mmmbbbb, M-Major mm-minor, bbbb-build */
	#if (__ARMCC_VERSION < 6000000) //ARMCC 5.x
	#pragma import(__use_no_semihosting)
	#else //ARMCC 6��֧��pragma import(__use_no_semihosting)
	__asm(".global __use_no_semihosting\n\t");
	#endif
	#pragma import(__ARM_use_no_argv)	//ָ��main()û�в���(��MicroLIB������)
#elif defined(__GNUC__)
	/* use respective options while linking:
	** If you need semihosting, linking like:
		$ arm-none-eabi-gcc --specs=rdimon.specs $(OTHER_LINK_OPTIONS)
	** you are using non-semihosting/retarget, linking like:
		$ arm-none-eabi-gcc --specs=nosys.specs $(OTHER_LINK_OPTIONS)
	*/
#endif

/* Keil����һЩ����Ҫ�ľ���
	#177-D: function "xxx" was declared but never referenced
	#223-D: function "yyy" declared implicitly
#ifdef __CC_ARM
#pragma diag_suppress 177, 223
#endif
*/

/* ͨ������ *********************************************************/
//ʹ��24λMCU-ID
#define INODES_HAVE_24_ID

/*===========================================================================
�µ�Դ��������Ŀ: ֧������������ 
===========================================================================*/
#if defined(PROJ_NEWPWRCTRL) //��Կ������Ŀ��־��

/* ������Ϣ *********************************************************/

/* 1. software relative -------------------- */
//Ӳ���汾
#define HARDWARE_VERSION "00-00-04"
//�����
#define SOFTWARE_VERSION "1-00-00"
//Э��汾��
#define PROCOTOL_VERSION "00-00-02"
//��Ʒ�ͺ�
#define PRODUCT_MODEL "INODES_NEWKEYBOX"
//����ʱ��
#define INODES_BUILD_TIME "2020-08-25"

/* 2. hardware relative -------------------- */
/* stm32f10x.h����SRAM_BASE/FLASH_BASE�Ķ��塣(0x20000000~0x200000BF�Ǹ����ж�
   �����������(192�ֽ�); SystemCoreClock����0x200000C0,��4���ֽ�) */

/* ����ϸ��data sheet����ȷ��������ļ���ֵ, ������ܺ������ */
//ϵͳSRAM��ʼ��ַ
#define SRAM_BASE_ADDR		(0x20000000)
//ϵͳSRAM size(ע��0xC4֮ǰAPP������)
#define SRAM_TOTAL_SIZE		(0x10000)               //0x20000000 - 0x2000FFFF
//ϵͳFLASH��ʼ��ַ
#define FLASH_BASE_ADDR 	(0x08000000)
//ϵͳFLASH��size
#define FLASH_TOTAL_SIZE  	(0x40000) 					//256K        0x08000000 - 0x0803FFFF
//FLASHҳ��С(����д����С����)
#define FLASH_PAGE_SIZE 	(2048)								//128 pages of 2Kbytes eash
//FLASH sector��С(д��������С����)
#define FLASH_SECTOR_SIZE	(4096)								//2pages for hign density and connectivity lines  4pages for low and medium density

/* ע: ������ϸ��Ϣ(��COM��FLASH, etc)�ڸ���ʵ�ֵ�ͷ�ļ�������. */

/* ����ģ�� *********************************************************/
/* 1. Ĭ�ϴ�/֧�ֵĹ���ģ��(��Ҫָ���ر�):
		DEBUG	���Դ�ӡ
		FLASH	��дFLASH
		MQTT	MQTTЭ��
		RTC		RTCʱ������
		HW_RTC	Ӳ��RTC(ȥ�������ģ��; ���RTC�ر�����Ч)
** ȥ��Ĭ�Ϲ���ģ����ʹ��INODES_NO_YYY(YYYȡ�����ĳֵ) */
#define INODES_NO_HW_RTC

/* 2. ��Ҫ��ʽָ����֧�ֵĹ���ģ�� */
//IAP(Inter-Application Programming)�ж�������Ԥ��(֧����������)
#define IAP_SLAVE
#define INODES_HAVE_LED_CTRL
#define INODES_HAVE_BEEP_CTRL


//֧��LAN/BT(����������)
//#define INODES_HAVE_ETH_BT

#else //project identity missed!!! ==========================================
#error "����붨����ȷ����Ŀ��־��!!!"

#endif

#endif //__INODES_CONFIG_H__

