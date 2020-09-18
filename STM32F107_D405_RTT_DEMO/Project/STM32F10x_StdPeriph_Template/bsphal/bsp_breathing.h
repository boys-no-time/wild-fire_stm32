#ifndef __PWM_BREATHING_H
#define	__PWM_BREATHING_H

#include "stm32f10x.h"

#define SIGN_LED_ACT_ON   	LED_ACT_Breathing()
#define SIGN_LED_ACT_OFF   	LED_ACT_OFF()

#define SIGN_LED_LINK_ON  	LED_LINK_Breathing()
#define SIGN_LED_LINK_OFF 	LED_LINK_OFF()

#define SIGN_LED_PWR_ON   	LED_PWR_Breathing()
#define SIGN_LED_PWR_OFF  	LED_PWR_OFF()

/*PWM表中的点数*/
extern uint16_t  POINT_NUM;
//控制输出波形的频率
extern __IO uint16_t period_class ;

void Breath_GPIO_Config(void);
void LED_Breathing_Init(void);
void  Breathing_LED_Init(uint16_t TIM_Channel);

void LED_ACT_Breathing(void);
void LED_LINK_Breathing(void);
void LED_PWR_Breathing(void);
void LED_ACT_OFF(void);
void LED_LINK_OFF(void);
void LED_PWR_OFF(void);

#endif /* __PWM_BREATHING_H */


