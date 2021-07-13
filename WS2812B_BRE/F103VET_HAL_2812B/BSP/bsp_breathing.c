
#include "stm32f1xx_it.h"
#include "bsp_breathing.h"

#define LED_ACT  	GPIO_PIN_9      //����״̬  PB9	TIM4_Channel_7
#define LED_LINK 	GPIO_PIN_8      //����״̬  PB8  TIM4_Channel_3 
#define LED_PWR  	GPIO_PIN_7     //��Դ�� 		PB7	TIM4_Channel_2

TIM_HandleTypeDef htim4;


//����������ε�Ƶ��
__IO uint16_t period_class = 13;

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

/**
  * @brief This function handles TIM4 global interrupt.
  */
void TIM4_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&htim4);

}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	static uint16_t pwm_index = 0;			//����PWM���
	static uint16_t period_cnt = 0;		//���ڼ���������
	
	period_cnt++;
		//����PWM���޸Ķ�ʱ���ıȽϼĴ���ֵ
	__HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_2,indexWave[pwm_index]);
	__HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_3,indexWave[pwm_index]);
	__HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_4,indexWave[pwm_index]);	
	
	//ÿ��PWM���е�ÿ��Ԫ��ʹ��period_class��
	if(period_cnt > period_class)
	{	
		pwm_index++;        //��־PWM��ָ����һ��Ԫ��
    if( pwm_index >=  POINT_NUM) 
		{
        pwm_index = 0;
    }
		period_cnt = 0;     //�������ڼ�����־		
	}
}
/**
  * @brief  ����TIM�����PWM�źŵ�ģʽ�������ڡ�����
	 ������ʱ������ ,���PWM��������жϷ������е�period_cntѭ����������*/	
	
	/* ����ʹ��������������Ϊ3�����Ҽ��ɴﵽ�ܺõ�Ч�� */	
	
	//Ҫ��
	//TIM_Period����PWM������ֵ��Χһ��
	//TIM_Prescaler��ԽСԽ�ã��ɼ�����˸����
	//PERIOD_CLASS���жϷ������п��Ƶ�����ѭ���Ĵ������������ɿ���������ߵ�����
	//POINT_NUM��PWM���Ԫ�أ�����PWM������ߵĲ�������	
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
void HAL_TIM4_MspPostInit(TIM_HandleTypeDef* timHandle)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
		__HAL_RCC_GPIOB_CLK_ENABLE();
    /**TIM4 GPIO Configuration
    PB6     ------> TIM4_CH1
    PB7     ------> TIM4_CH2
    PB8     ------> TIM4_CH3
    */
    GPIO_InitStruct.Pin = GPIO_PIN_7 | GPIO_PIN_8 |GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
    __HAL_RCC_TIM4_CLK_ENABLE();

    /* TIM4 interrupt Init */
    HAL_NVIC_SetPriority(TIM4_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(TIM4_IRQn);	
}


void TIM4_Mode_Config(void)
{
	TIM_ClockConfigTypeDef sClockSourceConfig;
	TIM_MasterConfigTypeDef sMasterConfig;
  TIM_OC_InitTypeDef sConfigOC;
	
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 129;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 999;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;

	HAL_TIM_Base_Init(&htim4);	
//  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
//  if (HAL_TIM_PWM_Init(&htim4) != HAL_OK)
//  {
//    Error_Handler();
//  }	
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	HAL_TIM_ConfigClockSource(&htim4,&sClockSourceConfig);
	
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig);

  sConfigOC.OCMode = TIM_OCMODE_PWM2;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;	
	
	HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_2);
	HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_3);
	HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_4);
	
	HAL_TIM4_MspPostInit(&htim4);
	HAL_TIM_Base_Start_IT(&htim4);

}

void LED_ACT_Breathing(void)
{
	
	HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_4);
}

void LED_LINK_Breathing(void)
{
	HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_3);
}

void LED_PWR_Breathing(void)
{
	HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_2);
}

void LED_ACT_OFF(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
		__HAL_RCC_GPIOB_CLK_ENABLE();
			
			/* ���ú������õ������� */
		GPIO_InitStruct.Pin =  LED_ACT;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;	
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct); 
		HAL_GPIO_WritePin(GPIOB, LED_ACT,GPIO_PIN_SET);	
}

void LED_LINK_OFF(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
		__HAL_RCC_GPIOB_CLK_ENABLE();
			
			/* ���ú������õ������� */
		GPIO_InitStruct.Pin =  LED_LINK;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;	
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct); 
		HAL_GPIO_WritePin(GPIOB, LED_LINK,GPIO_PIN_SET);	
}

void LED_PWR_OFF(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
		__HAL_RCC_GPIOB_CLK_ENABLE();
			
			/* ���ú������õ������� */
		GPIO_InitStruct.Pin =  LED_PWR;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;	
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct); 
		HAL_GPIO_WritePin(GPIOB, LED_PWR,GPIO_PIN_SET);
}
