 

/* Includes ------------------------------------------------------------------*/
#include "inodes_include.h"

#define	MESSAGE1 " STM32 Connectivity Line Device !\n"										 

int main(void)
{
	
	delay_init();	    //延时函数初始化
	DBG_Usart_Init();
	
  printf("\n\r %s", MESSAGE1);
	
	//WS2812_Led_Configuration();					//初始化灯带GPIO，PWM+DMA驱动WS2812灯带
	
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




