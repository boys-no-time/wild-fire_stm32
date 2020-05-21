#ifndef __PWM_BREATHING_H
#define	__PWM_BREATHING_H

#include "stm32f10x.h"

/*PWM表中的点数*/
extern uint16_t  POINT_NUM	;
//控制输出波形的频率
extern __IO uint16_t period_class ;
	

void      TIMx_Breathing_Init          (void);



#endif /* __PWM_BREATHING_H */
