/*********************************************************************
Copyright (c) 2020-2028 by inodes. All rights reserved. 
You are not allowed to copy or distribute the code without permission. 
*********************************************************************/
#include "inodes_include.h"

/* 
		1、在合适的地方使用 Hal_Led_Init 初始化
		LED_STATUS		LED_NET		LED_WIFI		LED_BLE
		2、LED_STATUS(ON) 开启		LED_STATUS(OFF) 关闭
*/

#ifdef INODES_HAVE_LED_CTRL
void Hal_Led_Init(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
		/*开启LED相关的GPIO外设时钟*/
		RCC_APB2PeriphClockCmd( LED_STATUS_CLK | LED_WIFI_CLK, ENABLE);
		GPIO_InitStructure.GPIO_Pin = LED_STATUS_PIN | LED_NET_PIN;	

		/*设置引脚模式为通用推挽输出*/
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
		GPIO_Init(LED_STATUS_PORT, &GPIO_InitStructure);
		
		GPIO_InitStructure.GPIO_Pin = LED_WIFI_PIN | LED_BLE_PIN;	
		GPIO_Init(LED_WIFI_PORT, &GPIO_InitStructure);
	
		GPIO_SetBits(LED_STATUS_PORT, LED_STATUS_PIN | LED_NET_PIN);
		GPIO_SetBits(LED_WIFI_PORT, LED_WIFI_PIN | LED_BLE_PIN);
	
}

#endif //INODES_HAVE_LED_CTRL

#ifdef INODES_HAVE_BEEP_CTRL
void Hal_Beep_Init(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;

		RCC_APB2PeriphClockCmd( BEEP_CLK, ENABLE);
		GPIO_InitStructure.GPIO_Pin = BEEP_PIN;	

		/*设置引脚模式为通用推挽输出*/
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
		GPIO_Init(BEEP_PORT, &GPIO_InitStructure);
		
		GPIO_ResetBits(BEEP_PORT, BEEP_PIN);
	
}

#endif //INODES_HAVE_BEEP_CTRL

