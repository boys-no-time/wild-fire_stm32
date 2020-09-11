/*********************************************************************
Copyright (c) 2020-2028 by inodes. All rights reserved. 
You are not allowed to copy or distribute the code without permission. 
*********************************************************************/

/**********************************************************
			!!! header的使用规则 !!!
	1. 所有APP/porting程序只许include这个header;
	2. 不要随意在其他header里include header。
***********************************************************/

#ifndef __INODES_INCLUDE_H__
#define __INODES_INCLUDE_H__

/* 1. 设备header：也许它们override什么重要东西，所以要最先. */
#include "stm32f10x.h"
#include "stm32f10x_it.h"
#include "stm32f10x_conf.h" // it includes almost all driver headers

/* 2. ANSI header：这个其次，但要在所有用户header之前. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <math.h>
#include <time.h>
#include <limits.h>
#include <float.h>

/* 3. 用户header：放在最后。不要随意更改次序;在最后面添加include. */
#include "inodes_config.h"  	//功能/特征相关的设置，一定在最前面!!!
#include <rtthread.h>			//这货有点特殊，可能我们需要对其定制
#include <rtdevice.h>			//这货有点特殊，可能我们需要对其定制
#include "inodes_defines.h" 	//一些重要的自定义
//#include "inodes_porting_inc.h" //底层porting的功能
//#include "inodes_dprintf.h"		//DEBUG打印相关
//#include "inodes_sdk.h"			//一些基础性的东西
//#include "inodes_mem.h"			//内存(SRAM)相关
//#include "inodes_ctrl.h"		//总控模块
//#include "inodes_network.h" 	//网络传输管理
//#include "inodes_mqtt_pack.h"	//MQTT封包解包
//#include "inodes_queue.h"		//数据队列处理
//#include "inodes_mqtt_client.h"	//MQTT client
//#include "inodes_datacntr.h"	//数据处理中心
//#include "inodes_settings.h"	//系统设置
/* 以上为比较固定的header；下面是项目特有的header(酌情增删;必须注释). */
//#include "inodes_vmem.h"			//设备数据集中收集
//#include "inodes_mobile.h"			//支持4G模组
//#include "inodes_wifi.h"			//支持WIFI模组
//#include "inodes_datacntr.h"		//指令数据处理中心
//#include "inodes_packet.h"			//指令数据包相关
//#include "inodes_device.h"			//采集板相关
//#include "inodes_meter.h"			//智能电表相关
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


