#ifndef __WS2812B_H
#define	__WS2812B_H

#include "stm32f10x.h"
	

//#define WS2812_IN_PIN	PA8

void Timer3_1_init(void);
void Timer3_2_init(void);
void Timer3_3_init(void);

void WS2812_send(uint8_t (*color)[3], uint16_t len);

#endif 
