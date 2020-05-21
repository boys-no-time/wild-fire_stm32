
/* Buffer that holds one complete DMA transmission
 * 
 * Ensure that this buffer is big enough to hold
 * all data bytes that need to be sent
 * 
 * The buffer size can be calculated as follows:
 * number of LEDs * 24 bytes + 42 bytes
 * 
 * This leaves us with a maximum string length of
 * (2^16 bytes per DMA stream - 42 bytes)/24 bytes per LED = 2728 LEDs
 */
 
#include "WS2812B.h" 
#include "delay.h"

#define TIM3_CCR1_Address 0x40000434	// physical memory address of Timer 3 CCR1 register
#define TIM3_CCR2_Address 0x40000438
#define TIM3_CCR3_Address 0x4000043c 	// physical memory address of Timer 3 CCR1 register
#define TIM3_CCR4_Address 0x40000440


#define TIMING_ONE  50
#define TIMING_ZERO 25
uint16_t LED_BYTE_Buffer[800];

u16 i;

uint8_t rgb0[][3] = {0,0,0};
//RED_LED数据
uint8_t rgb_r[53][3] = {{0,0,0},{5,0,0},{10,0,0},{15,0,0},{20,0,0},{25,0,0},{30,0,0},{35,0,0},{40,0,0},{45,0,0},
											 {50,0,0},{55,0,0},{60,0,0},{65,0,0},{70,0,0},{75,0,0},{80,0,0},{85,0,0},{90,0,0},{95,0,0},
											 {100,0,0},{105,0,0},{110,0,0},{115,0,0},{120,0,0},{125,0,0},{130,0,0},{125,0,0},{120,0,0},{115,0,0},
											 {110,0,0},{110,0,0},{100,0,0},{95,0,0},{90,0,0},{85,0,0},{80,0,0},{75,0,0},{70,0,0},{65,0,0},
											 {60,0,0},{55,0,0},{50,0,0},{45,0,0},{40,0,0},{35,0,0},{30,0,0},{25,0,0},{20,0,0},{15,0,0},
											 {10,0,0},{5,0,0},{0,0,0}};

//GREEN_LED数据											 
uint8_t rgb_g[53][3] = {{0,0,0},{0,5,0},{0,10,0},{0,15,0},{0,20,0},{0,25,0},{0,30,0},{0,35,0},{0,40,0},{0,45,0},
											 {0,50,0},{0,55,0},{0,60,0},{0,65,0},{0,70,0},{0,75,0},{0,80,0},{0,85,0},{0,90,0},{0,95,0},
											 {0,100,0},{0,105,0},{0,110,0},{0,115,0},{0,120,0},{0,122,0},{0,120,0},{0,120,0},{0,120,0},{0,115,0},
											 {0,110,0},{0,110,0},{0,100,0},{0,95,0},{0,90,0},{0,85,0},{0,80,0},{0,75,0},{0,70,0},{0,65,0},
											 {0,60,0},{0,55,0},{0,50,0},{0,45,0},{0,40,0},{0,35,0},{0,30,0},{0,25,0},{0,20,0},{0,15,0},
											 {0,10,0},{0,5,0},{0,0,0}};

//BLUE_LED数据	
uint8_t rgb_b[53][3] = {{0,0,0},{0,0,5},{0,0,10},{0,0,15},{0,0,20},{0,0,25},{0,0,30},{0,0,35},{0,0,40},{0,0,45},
											 {0,0,50},{0,0,55},{0,0,60},{0,0,65},{0,0,70},{0,0,75},{0,0,80},{0,0,85},{0,0,90},{0,0,95},
											 {0,0,100},{0,0,105},{0,0,110},{0,0,115},{0,0,120},{0,0,125},{0,0,130},{0,0,125},{0,0,120},{0,0,115},
											 {0,0,110},{0,0,110},{0,0,100},{0,0,95},{0,0,90},{0,0,85},{0,0,80},{0,0,75},{0,0,70},{0,0,65},
											 {0,0,60},{0,0,55},{0,0,50},{0,0,45},{0,0,40},{0,0,35},{0,0,30},{0,0,25},{0,0,20},{0,0,15},
											 {0,0,10},{0,0,5},{0,0,0}};

uint8_t green[53][3] = {{0,65,0},{0,65,0},{0,65,0},{0,65,0},{0,65,0},{0,65,0},{0,65,0},{0,65,0},{0,65,0},{0,65,0},
								 {0,65,0},{0,65,0},{0,65,0},{0,65,0},{0,65,0},{0,65,0},{0,65,0},{0,65,0},{0,65,0},{0,65,0},
								 {0,65,0},{0,65,0},{0,65,0},{0,65,0},{0,65,0},{0,65,0},{0,65,0},{0,65,0},{0,65,0},{0,65,0},
								 {0,65,0},{0,65,0},{0,65,0},{0,65,0},{0,65,0},{0,65,0},{0,65,0},{0,65,0},{0,65,0},{0,65,0},
								 {0,65,0},{0,65,0},{0,65,0},{0,65,0},{0,65,0},{0,65,0},{0,65,0},{0,65,0},{0,65,0},{0,65,0},
								 {0,65,0},{0,65,0},{0,65,0}};	 

 
void WS2812_Led_Configuration(void)
{
		//PD0 灯带电源开关，高电平开启，低电平关闭
		GPIO_InitTypeDef GPIO_InitStructure;
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD , ENABLE);	
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0 ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
		GPIO_ResetBits(GPIOD, GPIO_Pin_0);				//默认关闭
				
		Timer3_1_init();					//初始化TIM3，PWM+DMA驱动WS2812灯带
}	
 

//---------------------------------------------------------------//
/********************************************************
Get PWM2 signal used TIM3_CH1 by GPIO PC6,
Used DMA1_CH3 translate data for TIM3_CCR1 with buffer.
********************************************************/
void Timer3_1_init(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_OCInitTypeDef  TIM_OCInitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
  DMA_InitTypeDef DMA_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_FullRemap_TIM3, ENABLE);
	
	/* PC6-8 Configuration: TIM3 Channel 3 as alternate function push-pull */
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	
	/* Compute the prescaler value */
	//PrescalerValue = (uint16_t) (SystemCoreClock / 24000000) - 1;
	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = 90-1; // 800kHz 
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	
	TIM_OC3Init(TIM3, &TIM_OCInitStructure);
	
	/* configure DMA */
	/* DMA clock enable */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	
	/* DMA1 Channel2 Config */
	DMA_DeInit(DMA1_Channel3);

	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)TIM3_CCR3_Address;	// physical address of Timer 3 CCR1
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)LED_BYTE_Buffer;		// this is the buffer memory 
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;						// data shifted from memory to peripheral
	DMA_InitStructure.DMA_BufferSize = 42;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;					// automatically increase buffer index
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;							// stop DMA feed after buffer size is reached
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	
	DMA_Init(DMA1_Channel3, &DMA_InitStructure);

		/* TIM1 CC1 DMA Request enable */
	TIM_DMACmd(TIM3, TIM_DMA_Update, ENABLE);
}

/* This function sends data bytes out to a string of WS2812s
 * The first argument is a pointer to the first RGB triplet to be sent
 * The seconds argument is the number of LEDs in the chain
 * 
 * This will result in the RGB triplet passed by argument 1 being sent to 
 * the LED that is the furthest away from the controller (the point where
 * data is injected into the chain)
 */
void WS2812_send(uint8_t (*color)[3], uint16_t len)
{
	uint8_t i;
	uint16_t memaddr;
	uint16_t buffersize;
	buffersize = (len*24)+43;	// number of bytes needed is #LEDs * 24 bytes + 42 trailing bytes
	memaddr = 0;				// reset buffer memory index

	while (len)
	{	
				for(i=0; i<8; i++) // GREEN data
			{
					LED_BYTE_Buffer[memaddr] = ((color[0][1]<<i) & 0x0080) ? TIMING_ONE:TIMING_ZERO;
					memaddr++;
			}
			for(i=0; i<8; i++) // RED
			{
					LED_BYTE_Buffer[memaddr] = ((color[0][0]<<i) & 0x0080) ? TIMING_ONE:TIMING_ZERO;
					memaddr++;
			}
			for(i=0; i<8; i++) // BLUE
			{
					LED_BYTE_Buffer[memaddr] = ((color[0][2]<<i) & 0x0080) ? TIMING_ONE:TIMING_ZERO;
					memaddr++;
			}
			
		  len--;
	}
//===================================================================//	
//bug：最后一个周期波形不知道为什么全是高电平，故增加一个波形
  	LED_BYTE_Buffer[memaddr] = ((color[0][2]<<8) & 0x0080) ? TIMING_ONE:TIMING_ZERO;
//===================================================================//	
	  memaddr++;	
		while(memaddr < buffersize)
		{
			LED_BYTE_Buffer[memaddr] = 0;
			memaddr++;
		}

		DMA_SetCurrDataCounter(DMA1_Channel3, buffersize); 	// load number of bytes to be transferred
		DMA_Cmd(DMA1_Channel3, ENABLE); 			// enable DMA channel 6
		
		//TIM_SetCounter(TIM1,0x0000);
		
		TIM_Cmd(TIM3, ENABLE); 						// enable Timer 3
		while(!DMA_GetFlagStatus(DMA1_FLAG_TC3)) ; 	// wait until transfer complete
		TIM_Cmd(TIM3, DISABLE); 	// disable Timer 3
		
		DMA_Cmd(DMA1_Channel3, DISABLE); 			// disable DMA channel 6
		DMA_ClearFlag(DMA1_FLAG_TC3); 				// clear DMA1 Channel 6 transfer complete flag

}

void WS2812_Led_Flash(uint8_t led)
{

	DOOR_LED_CTRL_ON;
									

	WS2812_send(rgb0,led);	
	
	delay_ms(3000);
			
	for(i=0;i<53;i++)
			{
				WS2812_send(&green[i],led);
				//delay_ms(60);
			}
	delay_ms(3000);
			
	WS2812_send(green,led);													 											 																				 
																																						 
}


void WS2812_Led_Breathing(uint8_t led)		//灯带执行函数，红绿蓝三色呼吸点亮，共47颗灯
{
		DOOR_LED_CTRL_ON;
	
		WS2812_send(rgb0,led);
	
		for(i=0;i<53;i++)
		{
			WS2812_send(&rgb_r[i],led);
			delay_ms(70);
		}
		for(i=0;i<53;i++)
		{
			WS2812_send(&rgb_g[i],led);
			delay_ms(70);
		}
		for(i=0;i<53;i++)
		{
			WS2812_send(&rgb_b[i],led);
			delay_ms(70);
		}
}

