
#ifndef __HAL_LED_H__
#define __HAL_LED_H__

#include "inodes_include.h"


// 工作指示灯-红色
#define LED_STATUS_PORT    	GPIOE			              
#define LED_STATUS_CLK 	    RCC_APB2Periph_GPIOE		
#define LED_STATUS_PIN		  GPIO_Pin_0			        

// 网络指示灯-绿色
#define LED_NET_PORT    		GPIOE			              
#define LED_NET_CLK 	    	RCC_APB2Periph_GPIOE		
#define LED_NET_PIN		    	GPIO_Pin_1

// WIFI指示灯-绿色
#define LED_WIFI_PORT    		GPIOB			              
#define LED_WIFI_CLK 	    	RCC_APB2Periph_GPIOB		
#define LED_WIFI_PIN		  	GPIO_Pin_8

// 蓝牙指示灯-蓝色
#define LED_BLE_PORT    		GPIOB			              
#define LED_BLE_CLK 	    	RCC_APB2Periph_GPIOB		
#define LED_BLE_PIN		  		GPIO_Pin_9

#define BEEP_PORT    				GPIOD			              
#define BEEP_CLK 	    			RCC_APB2Periph_GPIOD		
#define BEEP_PIN		  			GPIO_Pin_1

#define ON  0
#define OFF 1

#define LED_STATUS(a)	if (a)	\
					GPIO_SetBits(LED_STATUS_PORT,LED_STATUS_PIN);\
					else		\
					GPIO_ResetBits(LED_STATUS_PORT,LED_STATUS_PIN);

#define LED_NET(a)	if (a)	\
					GPIO_SetBits(LED_NET_PORT,LED_NET_PIN);\
					else		\
					GPIO_ResetBits(LED_NET_PORT,LED_NET_PIN);

#define LED_WIFI(a)	if (a)	\
					GPIO_SetBits(LED_WIFI_PORT,LED_WIFI_PIN);\
					else		\
					GPIO_ResetBits(LED_WIFI_PORT,LED_WIFI_PIN);

#define LED_BLE(a)	if (a)	\
					GPIO_SetBits(LED_BLE_PORT,LED_BLE_PIN	);\
					else		\
					GPIO_ResetBits(LED_BLE_PORT,LED_BLE_PIN	);					

#define BEEP(a)	if (a)	\
					GPIO_ResetBits(BEEP_PORT,BEEP_PIN	);\
					else		\
					GPIO_SetBits(BEEP_PORT,BEEP_PIN	);	
					
void Hal_Led_Init(void);
void Hal_Beep_Init(void);

#endif

