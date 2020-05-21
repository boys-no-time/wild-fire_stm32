
#include "bsp_breathing.h"


//控制输出波形的频率
__IO uint16_t period_class = 10;

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


void TIM1_UP_IRQHandler(void)
{	
	static uint16_t pwm_index = 0;			//用于PWM查表
	static uint16_t period_cnt = 0;		//用于计算周期数
	
	if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET)	//TIM_IT_Update
 	{			
			period_cnt++;		
			TIM1->CCR2 = indexWave[pwm_index];	//根据PWM表修改定时器的比较寄存器值
			TIM1->CCR3 = indexWave[pwm_index];
			TIM1->CCR4 = indexWave[pwm_index];

			//每个PWM表中的每个元素使用period_class次
			if(period_cnt > period_class)				 				
			{				
				pwm_index++;												//标志PWM表指向下一个元素			
				//若PWM表已到达结尾，重新指向表头
				if( pwm_index >=  POINT_NUM)			
				{
					pwm_index=0;								
				}			
				period_cnt=0;											//重置周期计数标志
			}		
		
		TIM_ClearITPendingBit (TIM1, TIM_IT_Update);	//必须要清除中断标志位
	}
}

 /**
  * @brief  配置TIM复用输出PWM时用到的I/O
  * @param  无
  * @retval 无
  */
static void TIMx_GPIO_Config(void) 
{ 
	GPIO_InitTypeDef GPIO_InitStructure;		
	/*  clock enable */
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOE | RCC_APB2Periph_AFIO, ENABLE );
	
			//GPIO_PinRemapConfig(GPIO_PartialRemap_TIM1, ENABLE);  
	GPIO_PinRemapConfig(GPIO_FullRemap_TIM1, ENABLE);
		
	  /* 配置呼吸灯用到的引脚 */
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_11 | GPIO_Pin_13 | GPIO_Pin_14;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	// 复用推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init( GPIOE, &GPIO_InitStructure );

}

/**
  * @brief  配置TIM输出的PWM信号的模式，如周期、极性
  */

static void TIMx_Mode_Config(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	
	/* 设置TIMCLK 时钟 */
	RCC_APB2PeriphClockCmd (RCC_APB2Periph_TIM1, ENABLE ); 
	
	/* 基本定时器配置 ,配合PWM表点数、中断服务函数中的period_cnt循环次数设置*/	
	
	/* 设置使得整个呼吸过程为3秒左右即可达到很好的效果 */	
	
	//要求：
	//TIM_Period：与PWM表中数值范围一致
	//TIM_Prescaler：越小越好，可减轻闪烁现象
	//PERIOD_CLASS：中断服务函数中控制单个点循环的次数，调整它可控制拟合曲线的周期
	//POINT_NUM：PWM表的元素，它是PWM拟合曲线的采样点数

	/*************本实验中的配置***************/	
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

  /* 基本定时器配置 */		  
  TIM_TimeBaseStructure.TIM_Period = (1000-1);      							  //当定时器从0计数到 TIM_Period+1 ，为一个定时周期
  TIM_TimeBaseStructure.TIM_Prescaler = (200-1);	    							//设置预分频(72 - 1);//
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1 ;			//设置时钟分频系数：不分频(这里用不到)
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  	//向上计数模式
	
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

//配置中断优先级	
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn ;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	
	TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);

  /* PWM模式配置 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;	    				//配置为PWM模式1
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	//使能输出
	
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable ;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;						//当定时器计数值小于CCR1_Val时为H电平
  TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low;
  TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
  TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
	
  TIM_OCInitStructure.TIM_Pulse = 0;				 						  			//设置初始PWM脉冲宽度为0	 	  

		TIM_OC2Init(TIM1, &TIM_OCInitStructure);
    TIM_OC3Init(TIM1, &TIM_OCInitStructure);
    TIM_OC4Init(TIM1, &TIM_OCInitStructure);	 									//使能通道
		
		TIM_OC2PreloadConfig (TIM1, TIM_OCPreload_Enable );						//使能预装载
		TIM_OC3PreloadConfig (TIM1, TIM_OCPreload_Enable );	
		TIM_OC4PreloadConfig (TIM1, TIM_OCPreload_Enable );	

	  TIM_CCxCmd(TIM1, TIM_Channel_2, ENABLE);
    TIM_CCxCmd(TIM1, TIM_Channel_3, ENABLE);
    TIM_CCxCmd(TIM1, TIM_Channel_4, ENABLE);

  TIM_ARRPreloadConfig(TIM1, ENABLE);			 										//使能TIM重载寄存器ARR

  TIM_Cmd(TIM1, ENABLE);                   										//使能定时器	
	
	//TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);										//使能update中断	
	TIM_CtrlPWMOutputs(TIM1, ENABLE);
																							
}

/**
  * @brief  TIM 呼吸灯初始化
  *         配置PWM模式和GPIO
  * @param  无
  * @retval 无
  */
void TIMx_Breathing_Init(void)
{
	TIMx_GPIO_Config();
	TIMx_Mode_Config();	
}

/*********************************************END OF FILE**********************/
