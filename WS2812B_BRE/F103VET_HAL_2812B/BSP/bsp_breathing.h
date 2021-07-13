#ifndef __PWM_BREATHING_H
#define	__PWM_BREATHING_H

#include "main.h"

#define SIGN_LED_ACT_ON   	LED_ACT_Breathing()
#define SIGN_LED_ACT_OFF   	LED_ACT_OFF()

#define SIGN_LED_LINK_ON  	LED_LINK_Breathing()
#define SIGN_LED_LINK_OFF 	LED_LINK_OFF()

#define SIGN_LED_PWR_ON   	LED_PWR_Breathing()
#define SIGN_LED_PWR_OFF  	LED_PWR_OFF()

/*PWM���еĵ���*/
extern uint16_t  POINT_NUM;
//����������ε�Ƶ��
extern __IO uint16_t period_class ;

void TIM4_Mode_Config(void);

void Breath_GPIO_Config(void);
void LED_Breathing_Init(void);

void LED_ACT_Breathing(void);
void LED_LINK_Breathing(void);
void LED_PWR_Breathing(void);
void LED_ACT_OFF(void);
void LED_LINK_OFF(void);
void LED_PWR_OFF(void);

#endif /* __PWM_BREATHING_H */


