 

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "stm32_eval.h"
#include <stdio.h>

#include "delay.h"
#include "bsp_breathing.h"
#include "ws2812b.h"

#define	MESSAGE1 " STM32 Connectivity Line Device !\n"

u16 i,j;

uint8_t rgb0[][3] = {0,0,0};
uint8_t rgb1[53][3] = {{0,0,0},{10,0,0},{20,0,0},{30,0,0},{40,0,0},{50,0,0},{60,0,0},{70,0,0},{80,0,0},{90,0,0},
											 {100,0,0},{110,0,0},{120,0,0},{130,0,0},{140,0,0},{150,0,0},{160,0,0},{170,0,0},{180,0,0},{190,0,0},
											 {200,0,0},{210,0,0},{220,0,0},{230,0,0},{240,0,0},{250,0,0},{255,0,0},{250,0,0},{240,0,0},{230,0,0},
											 {220,0,0},{210,0,0},{200,0,0},{190,0,0},{180,0,0},{170,0,0},{160,0,0},{150,0,0},{140,0,0},{130,0,0},
											 {120,0,0},{110,0,0},{100,0,0},{90,0,0},{80,0,0},{70,0,0},{60,0,0},{50,0,0},{40,0,0},{30,0,0},
											 {20,0,0},{10,0,0},{0,0,0}};
uint8_t rgb2[53][3] = {{0,0,0},{0,10,0},{0,20,0},{0,30,0},{0,40,0},{0,50,0},{0,60,0},{0,70,0},{0,80,0},{0,90,0},
											 {0,100,0},{0,110,0},{0,120,0},{0,130,0},{0,140,0},{0,150,0},{0,160,0},{0,170,0},{0,180,0},{0,190,0},
											 {0,200,0},{0,210,0},{0,220,0},{0,230,0},{0,240,0},{0,250,0},{0,255,0},{0,250,0},{0,240,0},{0,230,0},
											 {0,220,0},{0,210,0},{0,200,0},{0,190,0},{0,180,0},{0,170,0},{0,160,0},{0,150,0},{0,140,0},{0,130,0},
											 {0,120,0},{0,110,0},{0,100,0},{0,90,0},{0,80,0},{0,70,0},{0,60,0},{0,50,0},{0,40,0},{0,30,0},
											 {0,20,0},{0,10,0},{0,0,0}};
uint8_t rgb3[53][3] = {{0,0,0},{0,0,10},{0,0,20},{0,0,30},{0,0,40},{0,0,50},{0,0,60},{0,0,70},{0,0,80},{0,0,90},
											 {0,0,100},{0,0,110},{0,0,120},{0,0,130},{0,0,140},{0,0,150},{0,0,160},{0,0,170},{0,0,180},{0,0,190},
											 {0,0,200},{0,0,210},{0,0,220},{0,0,230},{0,0,240},{0,0,250},{0,0,255},{0,0,250},{0,0,240},{0,0,230},
											 {0,0,220},{0,0,210},{0,0,200},{0,0,190},{0,0,180},{0,0,170},{0,0,160},{0,0,150},{0,0,140},{0,0,130},
											 {0,0,120},{0,0,110},{0,0,100},{0,0,90},{0,0,80},{0,0,70},{0,0,60},{0,0,50},{0,0,40},{0,0,30},
											 {0,0,20},{0,0,10},{0,0,0}};

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
	
	TIMx_Breathing_Init();
	printf("breath start ! \n\r");
	
	Timer3_1_init();
	//Timer3_init();
	//Timer4_init();
	printf("DMA start ! \n\r");
  /* Infinite loop */
	while(1)
	{
		WS2812_send(rgb0,8);
		for(i=0;i<53;i++)
		{
			WS2812_send(&rgb1[i],8);
			delay_ms(50);
		}
		for(i=0;i<53;i++)
		{
			WS2812_send(&rgb2[i],8);
			delay_ms(50);
		}
		for(i=0;i<53;i++)
		{
			WS2812_send(&rgb3[i],8);
			delay_ms(50);
		}
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


