 
/* Includes ------------------------------------------------------------------*/
#include "inodes_include.h"

#define	MESSAGE1 " STM32 Connectivity Line Device !\n"										 

int main(void)
{
	
	delay_init();	    //延时函数初始化
	DBG_Usart_Init();
	
  printf("\n\r %s", MESSAGE1);
	
	printf("main start ! \n\r");
	
	LED_ACT_OFFON(LED_STATUS,ON);
	
	while(1)
	{
		SIGN_BEEP_ACT_ON;
		
		LED_ACT_OFFON(LED_NET,ON);
		LED_ACT_OFFON(LED_WIFI,ON);
		LED_ACT_OFFON(LED_BL,ON);
		
		rt_thread_mdelay(200);
		
		SIGN_BEEP_ACT_OFF;
		
		LED_ACT_OFFON(LED_NET,OFF);
		LED_ACT_OFFON(LED_WIFI,OFF);
		LED_ACT_OFFON(LED_BL,OFF);
		
		rt_thread_mdelay(200);
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




