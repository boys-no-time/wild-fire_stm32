 

/* Includes ------------------------------------------------------------------*/
#include "inodes_include.h"

#define	MESSAGE1 " STM32 Connectivity Line Device !\n"										 

int main(void)
{
	
	delay_init();	    //��ʱ������ʼ��
	DBG_Usart_Init();
	
  printf("\n\r %s", MESSAGE1);
	
	//WS2812_Led_Configuration();					//��ʼ���ƴ�GPIO��PWM+DMA����WS2812�ƴ�
	
//������������
	printf("breath start ! \n\r");
	
	SIGN_LED_ACT_ON;										//��������״̬��

	SIGN_LED_PWR_ON;										//������Դ״̬��

	SIGN_LED_LINK_ON;										//��������״̬��	
	
	while(1)
	{
		//DOOR_LED_FLASH_ON;										//�ƴ�����Ч������
		DOOR_LED_BREATH_ON;												//�ƴ�����Ч������
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




