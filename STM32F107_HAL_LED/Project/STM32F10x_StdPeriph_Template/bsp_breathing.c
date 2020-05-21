
#include "bsp_breathing.h"


//����������ε�Ƶ��
__IO uint16_t period_class = 10;

/* LED���ȵȼ� PWM��,ָ������ ���˱�ʹ�ù���Ŀ¼�µ�python�ű�index_wave.py����*/
uint16_t indexWave[] = {
1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 4,
4, 5, 5, 6, 7, 8, 9, 10, 11, 13,
15, 17, 19, 22, 25, 28, 32, 36,
41, 47, 53, 61, 69, 79, 89, 102,
116, 131, 149, 170, 193, 219, 250,
284, 323, 367, 417, 474, 539, 613,
697, 792, 901, 1024, 1024, 901, 792,
697, 613, 539, 474, 417, 367, 323,
284, 250, 219, 193, 170, 149, 131, 
116, 102, 89, 79, 69, 61, 53, 47, 41,
36, 32, 28, 25, 22, 19, 17, 15, 13, 
11, 10, 9, 8, 7, 6, 5, 5, 4, 4, 3, 3,
2, 2, 2, 2, 1, 1, 1, 1
	
};

//����PWM���ж��ٸ�Ԫ��
uint16_t POINT_NUM = sizeof(indexWave)/sizeof(indexWave[0]); 


void TIM1_UP_IRQHandler(void)
{	
	static uint16_t pwm_index = 0;			//����PWM���
	static uint16_t period_cnt = 0;		//���ڼ���������
	
	if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET)	//TIM_IT_Update
 	{			
			period_cnt++;		
			TIM1->CCR2 = indexWave[pwm_index];	//����PWM���޸Ķ�ʱ���ıȽϼĴ���ֵ
			TIM1->CCR3 = indexWave[pwm_index];
			TIM1->CCR4 = indexWave[pwm_index];

			//ÿ��PWM���е�ÿ��Ԫ��ʹ��period_class��
			if(period_cnt > period_class)				 				
			{				
				pwm_index++;												//��־PWM��ָ����һ��Ԫ��			
				//��PWM���ѵ����β������ָ���ͷ
				if( pwm_index >=  POINT_NUM)			
				{
					pwm_index=0;								
				}			
				period_cnt=0;											//�������ڼ�����־
			}		
		
		TIM_ClearITPendingBit (TIM1, TIM_IT_Update);	//����Ҫ����жϱ�־λ
	}
}

 /**
  * @brief  ����TIM�������PWMʱ�õ���I/O
  * @param  ��
  * @retval ��
  */
static void TIMx_GPIO_Config(void) 
{ 
	GPIO_InitTypeDef GPIO_InitStructure;		
	/*  clock enable */
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOE | RCC_APB2Periph_AFIO, ENABLE );
	
			//GPIO_PinRemapConfig(GPIO_PartialRemap_TIM1, ENABLE);  
	GPIO_PinRemapConfig(GPIO_FullRemap_TIM1, ENABLE);
		
	  /* ���ú������õ������� */
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_11 | GPIO_Pin_13 | GPIO_Pin_14;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	// �����������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init( GPIOE, &GPIO_InitStructure );

}

/**
  * @brief  ����TIM�����PWM�źŵ�ģʽ�������ڡ�����
  */

static void TIMx_Mode_Config(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	
	/* ����TIMCLK ʱ�� */
	RCC_APB2PeriphClockCmd (RCC_APB2Periph_TIM1, ENABLE ); 
	
	/* ������ʱ������ ,���PWM��������жϷ������е�period_cntѭ����������*/	
	
	/* ����ʹ��������������Ϊ3�����Ҽ��ɴﵽ�ܺõ�Ч�� */	
	
	//Ҫ��
	//TIM_Period����PWM������ֵ��Χһ��
	//TIM_Prescaler��ԽСԽ�ã��ɼ�����˸����
	//PERIOD_CLASS���жϷ������п��Ƶ�����ѭ���Ĵ������������ɿ���������ߵ�����
	//POINT_NUM��PWM���Ԫ�أ�����PWM������ߵĲ�������

	/*************��ʵ���е�����***************/	
	/***********************************************
	#python����ű�	count.py
	#PWM����
	POINT_NUM = 110

	#���ڱ���
	PERIOD_CLASS = 10

	#��ʱ����ʱ����
	TIMER_TIM_Period = 2**10
	#��ʱ����Ƶ
	TIMER_TIM_Prescaler = 200

	#STM32ϵͳʱ��Ƶ�ʺ�����
	f_pclk = 72000000
	t_pclk = 1/f_pclk

	#��ʱ��update�¼�����
	t_timer = t_pclk*TIMER_TIM_Prescaler*TIMER_TIM_Period

	#ÿ��PWM���ʱ��
	T_Point = t_timer * PERIOD_CLASS

	#������������
	T_Up_Down_Cycle = T_Point * POINT_NUM

	print ("�������ڣ�",T_Up_Down_Cycle)
	
	#���н����
	
	�������ڣ�3.12888
	************************************************************/

  /* ������ʱ������ */		  
  TIM_TimeBaseStructure.TIM_Period = (1000-1);      							  //����ʱ����0������ TIM_Period+1 ��Ϊһ����ʱ����
  TIM_TimeBaseStructure.TIM_Prescaler = (200-1);	    							//����Ԥ��Ƶ(72 - 1);//
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1 ;			//����ʱ�ӷ�Ƶϵ��������Ƶ(�����ò���)
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  	//���ϼ���ģʽ
	
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

//�����ж����ȼ�	
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn ;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	
	TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);

  /* PWMģʽ���� */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;	    				//����ΪPWMģʽ1
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	//ʹ�����
	
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable ;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;						//����ʱ������ֵС��CCR1_ValʱΪH��ƽ
  TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low;
  TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
  TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
	
  TIM_OCInitStructure.TIM_Pulse = 0;				 						  			//���ó�ʼPWM������Ϊ0	 	  

		TIM_OC2Init(TIM1, &TIM_OCInitStructure);
    TIM_OC3Init(TIM1, &TIM_OCInitStructure);
    TIM_OC4Init(TIM1, &TIM_OCInitStructure);	 									//ʹ��ͨ��
		
		TIM_OC2PreloadConfig (TIM1, TIM_OCPreload_Enable );						//ʹ��Ԥװ��
		TIM_OC3PreloadConfig (TIM1, TIM_OCPreload_Enable );	
		TIM_OC4PreloadConfig (TIM1, TIM_OCPreload_Enable );	

	  TIM_CCxCmd(TIM1, TIM_Channel_2, ENABLE);
    TIM_CCxCmd(TIM1, TIM_Channel_3, ENABLE);
    TIM_CCxCmd(TIM1, TIM_Channel_4, ENABLE);

  TIM_ARRPreloadConfig(TIM1, ENABLE);			 										//ʹ��TIM���ؼĴ���ARR

  TIM_Cmd(TIM1, ENABLE);                   										//ʹ�ܶ�ʱ��	
	
	//TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);										//ʹ��update�ж�	
	TIM_CtrlPWMOutputs(TIM1, ENABLE);
																							
}

/**
  * @brief  TIM �����Ƴ�ʼ��
  *         ����PWMģʽ��GPIO
  * @param  ��
  * @retval ��
  */
void TIMx_Breathing_Init(void)
{
	TIMx_GPIO_Config();
	TIMx_Mode_Config();	
}

/*********************************************END OF FILE**********************/
