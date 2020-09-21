 
/* Includes ------------------------------------------------------------------*/
#include "inodes_include.h"

#define	MESSAGE1 " STM32 Connectivity Line Device !\n"										 

static rt_thread_t led_thread = RT_NULL;

static void led_thread_entry(void *parameter);

int main(void)
{
	
  printf("\n\r %s", MESSAGE1);
	
	
	LED_ACT_OFFON(LED_STATUS,ON);
	
	led_thread = rt_thread_create("led",led_thread_entry,
															RT_NULL,512,3,20);
	if(led_thread != RT_NULL)
			rt_thread_startup(led_thread);
	else
			return -1;	
}



void led_thread_entry(void *parameter)
{
    UNUSED(parameter);
		
	while(1)
	{
		//SIGN_BEEP_ACT_ON;
		
		LED_ACT_OFFON(LED_NET,ON);
		LED_ACT_OFFON(LED_WIFI,ON);
		LED_ACT_OFFON(LED_BL,ON);
		
		rt_thread_mdelay(200);
		
		//SIGN_BEEP_ACT_OFF;
		
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




