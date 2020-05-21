 

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "stm32_eval.h"
#include <stdio.h>

#include "delay.h"
#include "bsp_breathing.h"
#include "ws2812b.h"

#define	MESSAGE1 " STM32 Connectivity Line Device !\n"

										 

/* Private function prototypes -----------------------------------------------*/
#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

/* Private functions ---------------------------------------------------------*/
void uart_print()
{
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  STM_EVAL_COMInit(COM1, &USART_InitStructure);
}

int main(void)
{
	
	delay_init();	    //延时函数初始化
	uart_print();
  printf("\n\r %s", MESSAGE1);
	
	WS2812_Led_Configuration();					//初始化灯带GPIO，PWM+DMA驱动WS2812灯带
	
//呼吸灯主函数
	printf("breath start ! \n\r");
	SIGN_LED_ACT_ON;										//开启运行状态灯

	SIGN_LED_PWR_ON;										//开启电源状态灯

	SIGN_LED_LINK_ON;										//开启网络状态灯
	
	
	while(1)
	{
		//DOOR_LED_FLASH_ON;										//灯带闪亮效果开启
		DOOR_LED_BREATH_ON;												//灯带呼吸效果开启
	}
	
}


/**
  * @brief  Retargets the C library printf function to the USART.
  */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART */
  USART_SendData(USART1, (uint8_t) ch);

  /* Loop until the end of transmission */
  while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
  {}

  return ch;
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif


