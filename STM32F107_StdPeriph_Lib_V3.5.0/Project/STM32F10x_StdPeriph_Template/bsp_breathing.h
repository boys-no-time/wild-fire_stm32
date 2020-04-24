#ifndef __PWM_BREATHING_H
#define	__PWM_BREATHING_H

#include "stm32f10x.h"

/*PWM表中的点数*/
extern uint16_t  POINT_NUM	;
//控制输出波形的频率
extern __IO uint16_t period_class ;

#define RED_LIGHT 		1
#define GREEN_LIGHT 	2
#define BLUE_LIGHT		3

/*要使用什么颜色的呼吸灯，可选RED_LIGHT、GREEN_LIGHT、BLUE_LIGHT*/
#define LIGHT_COLOR 	RED_LIGHT

/********************定时器通道**************************/

/************红灯***************/
//	#define   BRE_TIMx                      TIM1

//	#define   BRE_TIM_APBxClock_FUN        RCC_APB2PeriphClockCmd
//	#define   BRE_TIM_CLK                   RCC_APB2Periph_TIM1
//	#define   BRE_TIM_GPIO_APBxClock_FUN   RCC_APB2PeriphClockCmd
//	#define   BRE_TIM_GPIO_CLK              (RCC_APB2Periph_GPIOE | RCC_APB2Periph_AFIO)

//	//红灯的引脚需要重映射
//	#define   BRE_GPIO_REMAP_FUN()						GPIO_PinRemapConfig(GPIO_FullRemap_TIM1, ENABLE); 				

//	#define  BRE_TIM_LED_PORT               GPIOE
//	#define  BRE_TIM_LED_PIN                GPIO_Pin_11

//	#define  BRE_TIM_OCxInit                TIM_OC2Init            //通道选择，1~4
//	#define  BRE_TIM_OCxPreloadConfig       TIM_OC2PreloadConfig 
//	#define  BRE_CCRx                       CCR2

//	#define   BRE_TIMx_IRQn                TIM1_UP_IRQn              //中断
//	#define   BRE_TIMx_IRQHandler          TIM1_UP_IRQHandler
	

void      TIMx_Breathing_Init          (void);



#endif /* __PWM_BREATHING_H */
