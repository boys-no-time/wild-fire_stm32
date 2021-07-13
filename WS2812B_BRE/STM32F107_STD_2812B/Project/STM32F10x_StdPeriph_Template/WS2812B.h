
#ifndef __WS2812B_H
#define	__WS2812B_H

#include "stm32f10x.h"


#define DOOR_LED_CTRL_ON    GPIO_SetBits(GPIOD, GPIO_Pin_0)
#define DOOR_LED_CTRL_OFF   GPIO_ResetBits(GPIOD, GPIO_Pin_0)

#define DOOR_LED_FLASH_ON   WS2812_Led_Flash(47)				//灯带闪亮效果开启，输入LED数量，预设47
#define DOOR_LED_FLASH_OFF  DOOR_LED_CTRL_OFF						//灯带闪亮效果关闭---->关闭电源

#define DOOR_LED_BREATH_ON   WS2812_Led_Breathing(47)		//灯带呼吸效果开启，输入LED数量，预设47
#define DOOR_LED_BREATH_OFF  DOOR_LED_CTRL_OFF					//灯带呼吸效果关闭---->关闭电源

//#define WS2812_IN_PIN	PC8  TIM3_CH3
void Timer3_1_init(void);

void WS2812_Led_Configuration(void);

void WS2812_send(uint8_t (*color)[3], uint16_t len);

void WS2812_Led_Flash(uint8_t led);

void WS2812_Led_Breathing(uint8_t led);

#endif



