

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"

/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}


void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}


void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}


void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}


void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}


void SVC_Handler(void)
{
}

void DebugMon_Handler(void)
{
}

void PendSV_Handler(void)
{
}

void SysTick_Handler(void)
{
}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/


#include "bsp_breathing.h"
/* �������жϷ����� */
//extern uint16_t  POINT_NUM;
extern uint16_t indexWave[];

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


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
