#ifndef __PWM_BREATHING_H
#define	__PWM_BREATHING_H

#include "stm32f10x.h"

/*PWM���еĵ���*/
extern uint16_t  POINT_NUM	;
//����������ε�Ƶ��
extern __IO uint16_t period_class ;

#define RED_LIGHT 		1
#define GREEN_LIGHT 	2
#define BLUE_LIGHT		3

/*Ҫʹ��ʲô��ɫ�ĺ����ƣ���ѡRED_LIGHT��GREEN_LIGHT��BLUE_LIGHT*/
#define LIGHT_COLOR 	RED_LIGHT

/********************��ʱ��ͨ��**************************/

/************���***************/
//	#define   BRE_TIMx                      TIM1

//	#define   BRE_TIM_APBxClock_FUN        RCC_APB2PeriphClockCmd
//	#define   BRE_TIM_CLK                   RCC_APB2Periph_TIM1
//	#define   BRE_TIM_GPIO_APBxClock_FUN   RCC_APB2PeriphClockCmd
//	#define   BRE_TIM_GPIO_CLK              (RCC_APB2Periph_GPIOE | RCC_APB2Periph_AFIO)

//	//��Ƶ�������Ҫ��ӳ��
//	#define   BRE_GPIO_REMAP_FUN()						GPIO_PinRemapConfig(GPIO_FullRemap_TIM1, ENABLE); 				

//	#define  BRE_TIM_LED_PORT               GPIOE
//	#define  BRE_TIM_LED_PIN                GPIO_Pin_11

//	#define  BRE_TIM_OCxInit                TIM_OC2Init            //ͨ��ѡ��1~4
//	#define  BRE_TIM_OCxPreloadConfig       TIM_OC2PreloadConfig 
//	#define  BRE_CCRx                       CCR2

//	#define   BRE_TIMx_IRQn                TIM1_UP_IRQn              //�ж�
//	#define   BRE_TIMx_IRQHandler          TIM1_UP_IRQHandler
	

void      TIMx_Breathing_Init          (void);



#endif /* __PWM_BREATHING_H */
