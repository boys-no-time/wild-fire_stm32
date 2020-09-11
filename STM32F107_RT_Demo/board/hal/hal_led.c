/*********************************************************************
Copyright (c) 2020-2028 by inodes. All rights reserved. 
You are not allowed to copy or distribute the code without permission. 
*********************************************************************/
#include "inodes_include.h"

/* 
		1���ں��ʵĵط�ʹ�� Hal_Led_Init ��ʼ��
		LED_STATUS		LED_NET		LED_WIFI		LED_BLE
		2��LED_STATUS(ON) ����		LED_STATUS(OFF) �ر�
*/

#ifdef INODES_HAVE_LED_CTRL
void Hal_Led_Init(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
		/*����LED��ص�GPIO����ʱ��*/
		RCC_APB2PeriphClockCmd( LED_STATUS_CLK | LED_WIFI_CLK, ENABLE);
		GPIO_InitStructure.GPIO_Pin = LED_STATUS_PIN | LED_NET_PIN;	

		/*��������ģʽΪͨ���������*/
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

		/*��������ģʽΪͨ���������*/
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
		GPIO_Init(BEEP_PORT, &GPIO_InitStructure);
		
		GPIO_ResetBits(BEEP_PORT, BEEP_PIN);
	
}

#endif //INODES_HAVE_BEEP_CTRL

