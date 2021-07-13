
#include "stm32f1xx_it.h"
#include "bsp_breathing.h"

#define LED_ACT  	GPIO_PIN_9      //运行状态  PB9	TIM4_Channel_7
#define LED_LINK 	GPIO_PIN_8      //网络状态  PB8  TIM4_Channel_3 
#define LED_PWR  	GPIO_PIN_7     //电源灯 		PB7	TIM4_Channel_2

TIM_HandleTypeDef htim4;


//控制输出波形的频率
__IO uint16_t period_class = 13;

/* LED亮度等级 PWM表,指数曲线 ，此表使用工程目录下的python脚本index_wave.py生成*/
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

//计算PWM表有多少个元素
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
	static uint16_t pwm_index = 0;			//用于PWM查表
	static uint16_t period_cnt = 0;		//用于计算周期数
	
	period_cnt++;
		//根据PWM表修改定时器的比较寄存器值
	__HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_2,indexWave[pwm_index]);
	__HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_3,indexWave[pwm_index]);
	__HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_4,indexWave[pwm_index]);	
	
	//每个PWM表中的每个元素使用period_class次
	if(period_cnt > period_class)
	{	
		pwm_index++;        //标志PWM表指向下一个元素
    if( pwm_index >=  POINT_NUM) 
		{
        pwm_index = 0;
    }
		period_cnt = 0;     //重置周期计数标志		
	}
}
/**
  * @brief  配置TIM输出的PWM信号的模式，如周期、极性
	 基本定时器配置 ,配合PWM表点数、中断服务函数中的period_cnt循环次数设置*/	
	
	/* 设置使得整个呼吸过程为3秒左右即可达到很好的效果 */	
	
	//要求：
	//TIM_Period：与PWM表中数值范围一致
	//TIM_Prescaler：越小越好，可减轻闪烁现象
	//PERIOD_CLASS：中断服务函数中控制单个点循环的次数，调整它可控制拟合曲线的周期
	//POINT_NUM：PWM表的元素，它是PWM拟合曲线的采样点数	
	/***********************************************
	#python计算脚本	count.py
	#PWM点数
	POINT_NUM = 110

	#周期倍数
	PERIOD_CLASS = 10

	#定时器定时周期
	TIMER_TIM_Period = 2**10
	#定时器分频
	TIMER_TIM_Prescaler = 200

	#STM32系统时钟频率和周期
	f_pclk = 72000000
	t_pclk = 1/f_pclk

	#定时器update事件周期
	t_timer = t_pclk*TIMER_TIM_Prescaler*TIMER_TIM_Period

	#每个PWM点的时间
	T_Point = t_timer * PERIOD_CLASS

	#整个呼吸周期
	T_Up_Down_Cycle = T_Point * POINT_NUM

	print ("呼吸周期：",T_Up_Down_Cycle)
	
	#运行结果：
	
	呼吸周期：3.12888
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
			
			/* 配置呼吸灯用到的引脚 */
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
			
			/* 配置呼吸灯用到的引脚 */
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
			
			/* 配置呼吸灯用到的引脚 */
		GPIO_InitStruct.Pin =  LED_PWR;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;	
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct); 
		HAL_GPIO_WritePin(GPIOB, LED_PWR,GPIO_PIN_SET);
}
