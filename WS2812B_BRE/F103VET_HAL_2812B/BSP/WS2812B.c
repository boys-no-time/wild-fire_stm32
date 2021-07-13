
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

TIM_HandleTypeDef htim3;
DMA_HandleTypeDef hdma_tim3_ch1_trig;

#define TIMING_ONE  50
#define TIMING_ZERO 25

#define TIM3_CCR1_Address 0x40000434	// physical memory address of Timer 3 CCR1 register

uint16_t LED_BYTE_Buffer[800];
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

				
		TIM4_2812_init();					//初始化TIM3，PWM+DMA驱动WS2812灯带
}	

/********************************************************
Get PWM2 signal used TIM3_CH1 by GPIO PA6,
Used DMA1_CH6 translate data for TIM3_CCR1 with buffer.
********************************************************/
void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();
  /* DMA interrupt init */
  /* DMA1_Channel6_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel6_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel6_IRQn);

}
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef* tim_pwmHandle)
{

  if(tim_pwmHandle->Instance==TIM3)
  {
  /* USER CODE BEGIN TIM3_MspInit 0 */

  /* USER CODE END TIM3_MspInit 0 */
    /* TIM3 clock enable */
    __HAL_RCC_TIM3_CLK_ENABLE();

    /* TIM3 DMA Init */
    /* TIM3_CH1_TRIG Init */
    hdma_tim3_ch1_trig.Instance = DMA1_Channel6;
    hdma_tim3_ch1_trig.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_tim3_ch1_trig.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_tim3_ch1_trig.Init.MemInc = DMA_MINC_ENABLE;
    hdma_tim3_ch1_trig.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hdma_tim3_ch1_trig.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    hdma_tim3_ch1_trig.Init.Mode = DMA_NORMAL;
    hdma_tim3_ch1_trig.Init.Priority = DMA_PRIORITY_HIGH;
    if (HAL_DMA_Init(&hdma_tim3_ch1_trig) != HAL_OK)
    {
      Error_Handler();
    }

    /* Several peripheral DMA handle pointers point to the same DMA handle.
     Be aware that there is only one channel to perform all the requested DMAs. */
    __HAL_LINKDMA(tim_pwmHandle,hdma[TIM_DMA_ID_CC1],hdma_tim3_ch1_trig);
    __HAL_LINKDMA(tim_pwmHandle,hdma[TIM_DMA_ID_TRIGGER],hdma_tim3_ch1_trig);

  }
}
void HAL_TIM_PWM_MspDeInit(TIM_HandleTypeDef* tim_pwmHandle)
{

  if(tim_pwmHandle->Instance==TIM3)
  {
  /* USER CODE BEGIN TIM3_MspDeInit 0 */

  /* USER CODE END TIM3_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_TIM3_CLK_DISABLE();

    /* TIM3 DMA DeInit */
    HAL_DMA_DeInit(tim_pwmHandle->hdma[TIM_DMA_ID_CC1]);
    HAL_DMA_DeInit(tim_pwmHandle->hdma[TIM_DMA_ID_TRIGGER]);
  /* USER CODE BEGIN TIM3_MspDeInit 1 */

  /* USER CODE END TIM3_MspDeInit 1 */
  }
}

/**
  * @brief This function handles DMA1 channel6 global interrupt.
  */
void DMA1_Channel6_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel6_IRQn 0 */

  /* USER CODE END DMA1_Channel6_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_tim3_ch1_trig);
  /* USER CODE BEGIN DMA1_Channel6_IRQn 1 */

  /* USER CODE END DMA1_Channel6_IRQn 1 */
}
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == TIM3)
		{
        HAL_TIM_PWM_Stop_DMA(&htim3, TIM_CHANNEL_1);
    }
}

void TIM4_2812_init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 0;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 89;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
	
	MX_DMA_Init();
	
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }	
	
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig);

  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	
  HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1);

		__HAL_RCC_GPIOA_CLK_ENABLE();	
    /**TIM3 GPIO Configuration    PA6     ------> TIM3_CH1    */
    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	
}
/* This function sends data bytes out to a string of WS2812s
 * The first argument is a pointer to the first RGB triplet to be sent
 * The seconds argument is the number of LEDs in the chain
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
		HAL_TIM_PWM_Start_DMA(&htim3, TIM_CHANNEL_1, (uint32_t*)&LED_BYTE_Buffer, buffersize);
//		HAL_DMA_Start_IT(&hdma_tim3_ch1_trig, (uint32_t)&LED_BYTE_Buffer,(uint32_t)TIM3_CCR1_Address, buffersize);
		
//		DMA_SetCurrDataCounter(DMA1_Channel6, buffersize); 	// load number of bytes to be transferred
//		DMA_Cmd(DMA1_Channel6, ENABLE); 			// enable DMA channel 6
		
		//TIM_SetCounter(TIM1,0x0000);
		
//		TIM_Cmd(TIM3, ENABLE); 						// enable Timer 3
//		while(!DMA_GetFlagStatus(DMA1_FLAG_TC6)) ; 	// wait until transfer complete
//		TIM_Cmd(TIM3, DISABLE); 	// disable Timer 3
//		
//		DMA_Cmd(DMA1_Channel6, DISABLE); 			// disable DMA channel 6
//		DMA_ClearFlag(DMA1_FLAG_TC6); 				// clear DMA1 Channel 6 transfer complete flag

}

void WS2812_Led_Flash(uint8_t led)
{
	uint8_t i;
	DOOR_LED_CTRL_ON;
									
	WS2812_send(rgb0,led);	
	
	HAL_Delay(3000);
			
	for(i=0;i<53;i++)
			{
				WS2812_send(&green[i],led);
				//HAL_Delay(60);
			}
	HAL_Delay(3000);
			
	WS2812_send(green,led);													 											 																				 
																																						 
}


void WS2812_Led_Breathing(uint8_t led)		//灯带执行函数，红绿蓝三色呼吸点亮，共47颗灯
{
		uint8_t i;
		DOOR_LED_CTRL_ON;
	
		WS2812_send(rgb0,led);
	
		for(i=0;i<53;i++)
		{
			WS2812_send(&rgb_r[i],led);
			HAL_Delay(70);
		}
		for(i=0;i<53;i++)
		{
			WS2812_send(&rgb_g[i],led);
			HAL_Delay(70);
		}
		for(i=0;i<53;i++)
		{
			WS2812_send(&rgb_b[i],led);
			HAL_Delay(70);
		}
}

