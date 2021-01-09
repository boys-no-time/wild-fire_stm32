参考HAL固件包中的 Examples RCC示例代码，(参考F107VC硬件，25M晶振)
需要开启时钟安全系统

1、实现SystemClockHSE_Config

2、实现SystemClockHSI_Config

3、实现SwitchSystemClock，在进入中断后循环切换HSI和HSE时钟

4、重写HAL_RCC_CSSCallback();
该函数在RCC的CSS中断处理中被调用（HAL_RCC_NMI_IRQHandler）
这样当时钟发生故障，在HAL_RCC_CSSCallback回调函数中运行切换时钟函数，进行切换时钟。

/**
  * @brief This function handles the RCC CSS interrupt request.
  * @note This API should be called under the NMI_Handler().
  * @retval None
  */
void HAL_RCC_NMI_IRQHandler(void)
{
  /* Check RCC CSSF flag  */
  if(__HAL_RCC_GET_IT(RCC_IT_CSS))
  {
    /* RCC Clock Security System interrupt user callback */
    HAL_RCC_CSSCallback();

    /* Clear RCC CSS pending bit */
    __HAL_RCC_CLEAR_IT(RCC_IT_CSS);
  }
}
  /* Initialize Key push-button, will be used to trigger an interrupt each time it's pressed.
       In the ISR the PLL source will be changed from HSI to HSE circularly */

/**
    * @brief  Switch the PLL source from HSI to HSE , and select the PLL as SYSCLK
  *         source.
  *         The system Clock is configured as follow :
  *            System Clock source            = PLL (HSE )
  *            SYSCLK(Hz)                     = 72000000
  *            HCLK(Hz)                       = 72000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 2
  *            APB2 Prescaler                 = 1
  *            HSE Frequency(Hz)              = 25000000
  *            HSE PREDIV1                    = 5
  *            HSE PREDIV2                    = 5
  *            PLLMUL                         = 9
  *            PLL2MUL                        = 8
  *            Flash Latency(WS)              = 2
  * @param  None
  * @retval None
  */
static void SystemClockHSE_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};

  /* -1- Select HSI as system clock source to allow modification of the PLL configuration */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }
  
  /* -2- Enable HSE  Oscillator, select it as PLL source and finally activate the PLL */
  RCC_OscInitStruct.OscillatorType        = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState              = RCC_HSE_ON;
  
  RCC_OscInitStruct.PLL.PLLSource         = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLState          = RCC_PLL_ON;
  RCC_OscInitStruct.Prediv1Source         = RCC_PREDIV1_SOURCE_PLL2;
  RCC_OscInitStruct.HSEPredivValue        = RCC_HSE_PREDIV_DIV5;
  RCC_OscInitStruct.PLL.PLLMUL            = RCC_PLL_MUL9;
  RCC_OscInitStruct.PLL2.PLL2State        = RCC_PLL2_ON;
  RCC_OscInitStruct.PLL2.PLL2MUL          = RCC_PLL2_MUL8;
  RCC_OscInitStruct.PLL2.HSEPrediv2Value  = RCC_HSE_PREDIV2_DIV5;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  /* -3- Select the PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 clocks dividers */
  RCC_ClkInitStruct.ClockType       = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource    = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider   = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider  = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider  = RCC_HCLK_DIV1;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  /* -4- Optional: Disable HSI Oscillator (if the HSI is no more needed by the application)*/
  RCC_OscInitStruct.OscillatorType  = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState        = RCC_HSI_OFF;
  RCC_OscInitStruct.PLL.PLLState    = RCC_PLL_NONE;
  RCC_OscInitStruct.PLL2.PLL2State  = RCC_PLL2_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }
}

/**
  * @brief  Switch the PLL source from HSE  to HSI, and select the PLL as SYSCLK
  *         source.
  *         The system Clock is configured as follow :
  *            System Clock source            = PLL (HSI)
  *            SYSCLK(Hz)                     = 36000000
  *            HCLK(Hz)                       = 36000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 2
  *            APB2 Prescaler                 = 1
  *            HSI Frequency(Hz) /2           = 4000000
  *            PLLMUL                         = 9
  *            Flash Latency(WS)              = 2
  * @param  None
  * @retval None
  */
static void SystemClockHSI_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};

  /* -1- Select HSE  as system clock source to allow modification of the PLL configuration */
  RCC_ClkInitStruct.ClockType       = RCC_CLOCKTYPE_SYSCLK;
  RCC_ClkInitStruct.SYSCLKSource    = RCC_SYSCLKSOURCE_HSE;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  /* -2- Enable HSI Oscillator, select it as PLL source and finally activate the PLL */
  RCC_OscInitStruct.OscillatorType    = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState          = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue  = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState      = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource     = RCC_PLLSOURCE_HSI_DIV2;
  RCC_OscInitStruct.PLL.PLLMUL        = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  /* -3- Select the PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 clocks dividers */
  RCC_ClkInitStruct.ClockType       = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource    = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider   = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider  = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider  = RCC_HCLK_DIV1;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  /* -4- Optional: Disable HSE  Oscillator (if the HSE  is no more needed by the application) */
  RCC_OscInitStruct.OscillatorType  = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState        = RCC_HSE_OFF;
  RCC_OscInitStruct.PLL.PLLState    = RCC_PLL_NONE;
  RCC_OscInitStruct.PLL2.PLL2State  = RCC_PLL2_OFF;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }
}

/**
  * @brief  switch in system clock out of ISR context.
  * @retval None
  */
static void SwitchSystemClock(void)
{
  if (__HAL_RCC_GET_PLL_OSCSOURCE() == RCC_PLLSOURCE_HSI_DIV2)
  {
    /* PLL source is HSI oscillator */
    /* Set SYSCLK frequency to 72000000 Hz, coming from the PLL which is clocked by HSE  */
    SystemClockHSE_Config();
  }
  else if (__HAL_RCC_GET_PLL_OSCSOURCE() == RCC_PLLSOURCE_HSE)
  {
    /* PLL source is HSE  oscillator */
    /* Set SYSCLK frequency to 36000000 Hz, coming from the PLL which is clocked by HSI */
    SystemClockHSI_Config();
  }

  /* reset global variable */
  SwitchClock = RESET;
}

















