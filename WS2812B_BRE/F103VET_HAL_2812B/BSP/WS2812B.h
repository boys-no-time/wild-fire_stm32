
#ifndef __WS2812B_H
#define	__WS2812B_H

#include "main.h"

#define DOOR_LED_CTRL_ON    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5,GPIO_PIN_SET);
#define DOOR_LED_CTRL_OFF   HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5,GPIO_PIN_RESET);

#define DOOR_LED_FLASH_ON   WS2812_Led_Flash(47)				//灯带闪亮效果开启，输入LED数量，预设47
#define DOOR_LED_FLASH_OFF  DOOR_LED_CTRL_OFF						//灯带闪亮效果关闭---->关闭电源

#define DOOR_LED_BREATH_ON   WS2812_Led_Breathing(47)		//灯带呼吸效果开启，输入LED数量，预设47
#define DOOR_LED_BREATH_OFF  DOOR_LED_CTRL_OFF					//灯带呼吸效果关闭---->关闭电源

//#define WS2812_IN_PIN	PC8  TIM3_CH3
void TIM4_2812_init(void);

void WS2812_Led_Configuration(void);

void WS2812_send(uint8_t (*color)[3], uint16_t len);

void WS2812_Led_Flash(uint8_t led);

void WS2812_Led_Breathing(uint8_t led);


#endif



