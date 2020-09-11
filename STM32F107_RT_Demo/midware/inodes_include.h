/*********************************************************************
Copyright (c) 2020-2028 by inodes. All rights reserved. 
You are not allowed to copy or distribute the code without permission. 
*********************************************************************/

/**********************************************************
			!!! header��ʹ�ù��� !!!
	1. ����APP/porting����ֻ��include���header;
	2. ��Ҫ����������header��include header��
***********************************************************/

#ifndef __INODES_INCLUDE_H__
#define __INODES_INCLUDE_H__

/* 1. �豸header��Ҳ������overrideʲô��Ҫ����������Ҫ����. */
#include "stm32f10x.h"
#include "stm32f10x_it.h"
#include "stm32f10x_conf.h" // it includes almost all driver headers

/* 2. ANSI header�������Σ���Ҫ�������û�header֮ǰ. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <math.h>
#include <time.h>
#include <limits.h>
#include <float.h>

/* 3. �û�header��������󡣲�Ҫ������Ĵ���;����������include. */
#include "inodes_config.h"  	//����/������ص����ã�һ������ǰ��!!!
#include <rtthread.h>			//����е����⣬����������Ҫ���䶨��
#include <rtdevice.h>			//����е����⣬����������Ҫ���䶨��
#include "inodes_defines.h" 	//һЩ��Ҫ���Զ���
//#include "inodes_porting_inc.h" //�ײ�porting�Ĺ���
//#include "inodes_dprintf.h"		//DEBUG��ӡ���
//#include "inodes_sdk.h"			//һЩ�����ԵĶ���
//#include "inodes_mem.h"			//�ڴ�(SRAM)���
//#include "inodes_ctrl.h"		//�ܿ�ģ��
//#include "inodes_network.h" 	//���紫�����
//#include "inodes_mqtt_pack.h"	//MQTT������
//#include "inodes_queue.h"		//���ݶ��д���
//#include "inodes_mqtt_client.h"	//MQTT client
//#include "inodes_datacntr.h"	//���ݴ�������
//#include "inodes_settings.h"	//ϵͳ����
/* ����Ϊ�ȽϹ̶���header����������Ŀ���е�header(������ɾ;����ע��). */
//#include "inodes_vmem.h"			//�豸���ݼ����ռ�
//#include "inodes_mobile.h"			//֧��4Gģ��
//#include "inodes_wifi.h"			//֧��WIFIģ��
//#include "inodes_datacntr.h"		//ָ�����ݴ�������
//#include "inodes_packet.h"			//ָ�����ݰ����
//#include "inodes_device.h"			//�ɼ������
//#include "inodes_meter.h"			//���ܵ�����
#include "inodes_port.h"
#include "inodes_thread.h"
#include "common.h" //in "board/bsp"
#include "version.h"
#include "mqtt_client.h"
#include "nextai_mqtt.h"
#include "nextai_defined.h"
#include "nextai_setting.h"
#include "nextai_flash.h"
#include "nextai_com.h"
#include "board.h"
#include "cJSON.h"
#include "exboard.h"
#include "swdog.h"
#include "stm32_eth.h"
#include "hal_led.h"
#include "fw_upgrade.h"
#include "com_master.h"
#include "hal_led.h"
#include "hal_com.h"
#include "usart.h"
#include "msd.h"
#include "crc8.h"
#include "bootstart.h"
#include "bootboard.h"
#include "boardtest.h"
#include "platform.h"
#include "ds1302.h"
#include "hal_key.h" 

#endif //__INODES_INCLUDE_H__


