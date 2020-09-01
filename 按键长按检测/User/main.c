
#include "stm32f10x.h"
#include "bsp_led.h"  
#include "bsp_key.h" 
#include "bsp_usart.h"

volatile uint32_t key_press_time = 0; // ms ��ʱ���� 

int main(void)
{	
	/* LED�˿ڳ�ʼ�� */
	LED_GPIO_Config();
	Key_GPIO_Config();
	USART_Config();
	
	BASIC_TIM_Init();
	LED2_ON;
	
	printf("SYSTERM INIT ! \n\n");
  while(1)
	{
	if( key_press_time >= 5000 ) /* 5000 * 1 ms = 5s ʱ�䵽 */
  {
      key_press_time = 0;
			LED2_OFF;
			LED1_ON;
			printf("��⵽����\n\n\n\n");
  }
}

}
/*********************************************END OF FILE**********************/
