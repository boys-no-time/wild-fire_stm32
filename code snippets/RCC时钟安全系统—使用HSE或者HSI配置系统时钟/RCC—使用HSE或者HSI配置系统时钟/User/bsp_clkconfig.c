#include "bsp_clkconfig.h"
#include "stm32f10x_rcc.h"

/*
 * 使用HSE时，设置系统时钟的步骤
 * 1、开启HSE ，并等待 HSE 稳定
 * 2、设置 AHB、APB2、APB1的预分频因子
 * 3、设置PLL的时钟来源，和PLL的倍频因子，设置各种频率主要就是在这里设置
 * 4、开启PLL，并等待PLL稳定
 * 5、把PLLCK切换为系统时钟SYSCLK
 * 6、读取时钟切换状态位，确保PLLCLK被选为系统时钟
 */

/* 设置 系统时钟:SYSCLK, AHB总线时钟:HCLK, APB2总线时钟:PCLK2, APB1总线时钟:PCLK1
 * PCLK2 = HCLK = SYSCLK
 * PCLK1 = HCLK/2,最高只能是36M
 * 参数说明：pllmul是PLL的倍频因子，在调用的时候可以是：RCC_PLLMul_x , x:[2,3,...16]
 * 举例：User_SetSysClock(RCC_PLLMul_9);  则设置系统时钟为：8MHZ * 9 = 72MHZ
 *       User_SetSysClock(RCC_PLLMul_16); 则设置系统时钟为：8MHZ * 16 = 128MHZ，超频慎用
 *
 * HSE作为时钟来源，经过PLL倍频作为系统时钟，这是通常的做法
 */
 
void HSE_SetSysClock(uint32_t pllmul)
{
	__IO uint32_t StartUpCounter = 0, HSEStartUpStatus = 0;

	// 把RCC外设初始化成复位状态，这句是必须的
  RCC_DeInit();

  //使能HSE，开启外部晶振，假设用的是8M
  RCC_HSEConfig(RCC_HSE_ON);

  // 等待 HSE 启动稳定
  HSEStartUpStatus = RCC_WaitForHSEStartUp();

#if 1	
	// 只有 HSE 稳定之后则继续往下执行
  if (HSEStartUpStatus == SUCCESS)
  {
		#ifdef RCC_CSS_EN 
		RCC_ClockSecuritySystemCmd(ENABLE); //启动时钟安全系统 CSS   
		#endif
		
//----------------------------------------------------------------------//
    // 使能FLASH 预存取缓冲区
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

    // SYSCLK周期与闪存访问时间的比例设置，这里统一设置成2
		// 设置成2的时候，SYSCLK低于48M也可以工作，如果设置成0或者1的时候，
		// 如果配置的SYSCLK超出了范围的话，则会进入硬件错误，程序就死了
		// 0：0 < SYSCLK <= 24M
		// 1：24< SYSCLK <= 48M
		// 2：48< SYSCLK <= 72M
    FLASH_SetLatency(FLASH_Latency_2);
//----------------------------------------------------------------------//
 
    // AHB预分频因子设置为1分频，HCLK = SYSCLK 
    RCC_HCLKConfig(RCC_SYSCLK_Div1); 
  
    // APB2预分频因子设置为1分频，PCLK2 = HCLK
    RCC_PCLK2Config(RCC_HCLK_Div1); 

    // APB1预分频因子设置为1分频，PCLK1 = HCLK/2 
    RCC_PCLK1Config(RCC_HCLK_Div2);
		
//-----------------设置各种频率主要就是在这里设置-------------------//
    // 设置PLL时钟来源为HSE，设置PLL倍频因子
		// PLLCLK = 8MHz * pllmul
		RCC_PLLConfig(RCC_PLLSource_HSE_Div1, pllmul);
//------------------------------------------------------------------//
    RCC_PLLCmd(ENABLE);    // 开启PLL 

    // 等待 PLL稳定
    while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);

    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);    // 当PLL稳定之后，把PLL时钟切换为系统时钟SYSCLK

    // 读取时钟切换状态位，确保PLLCLK被选为系统时钟
    while (RCC_GetSYSCLKSource() != 0x08);
  }
#endif	
  else
  { // 如果HSE开启失败，那么程序就会来到这里，用户可在这里添加出错的代码处理
		// 当HSE开启失败或者故障的时候，单片机会自动把HSI设置为系统时钟，
		// HSI是内部的高速时钟，8MHZ
		
        RCC_DeInit();  //复位RCC寄存器
        RCC_HSEConfig(RCC_HSE_OFF);       //关闭外部晶（HSE）
        RCC_HCLKConfig(RCC_SYSCLK_Div1);   //设置ABH时钟（HCLK）：HCLK=SYSCLK
        //设置PLL时钟源及倍频系数， PLLCLK = HSI/2*PLLMul = 8/2*12 = 48MHz 实际是：48MHz
        RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_12);
        RCC_PLLCmd(ENABLE); //使能PLL

        //检查指定的RCC标志位设置与否
        while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET); // Wait till PLL is ready

        RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);          //设置系统时钟 (SYSCLK)

        //返回作用系统时钟的时钟源
        // Wait till PLL is used as system clock source
        while(RCC_GetSYSCLKSource() != 0x08);
		
  }
}

/*
 * 使用HSI时，设置系统时钟的步骤
 * 1、开启HSI ，并等待 HSI 稳定
 * 2、设置 AHB、APB2、APB1的预分频因子
 * 3、设置PLL的时钟来源，和PLL的倍频因子，设置各种频率主要就是在这里设置
 * 4、开启PLL，并等待PLL稳定
 * 5、把PLLCK切换为系统时钟SYSCLK
 * 6、读取时钟切换状态位，确保PLLCLK被选为系统时钟
 */

/* 
 * HSI作为时钟来源，经过PLL倍频作为系统时钟，这是在HSE故障的时候才使用的方法
 * HSI会因为温度等原因会有漂移，不稳定，一般不会用HSI作为时钟来源，除非是迫不得已的情况
 * 如果HSI要作为PLL时钟的来源的话，必须二分频之后才可以，即HSI/2，而PLL倍频因子最大只能是16
 * 所以当使用HSI的时候，SYSCLK最大只能是4M*16=64M
 */

void HSI_SetSysClock(uint32_t pllmul)
{
	__IO uint32_t HSIStartUpStatus = 0;

	// 把RCC外设初始化成复位状态，这句是必须的
  RCC_DeInit();

  //使能HSI
	RCC_HSICmd(ENABLE);
	
  // 等待 HSI 就绪
	HSIStartUpStatus = RCC->CR & RCC_CR_HSIRDY;
	
	// 只有 HSI就绪之后则继续往下执行
  if (HSIStartUpStatus == RCC_CR_HSIRDY)
  {
//----------------------------------------------------------------------//
    // 使能FLASH 预存取缓冲区
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

    // SYSCLK周期与闪存访问时间的比例设置，这里统一设置成2
		// 设置成2的时候，SYSCLK低于48M也可以工作，如果设置成0或者1的时候，
		// 如果配置的SYSCLK超出了范围的话，则会进入硬件错误，程序就死了
		// 0：0 < SYSCLK <= 24M
		// 1：24< SYSCLK <= 48M
		// 2：48< SYSCLK <= 72M
    FLASH_SetLatency(FLASH_Latency_2);
//----------------------------------------------------------------------//
 
    // AHB预分频因子设置为1分频，HCLK = SYSCLK 
    RCC_HCLKConfig(RCC_SYSCLK_Div1); 
  
    // APB2预分频因子设置为1分频，PCLK2 = HCLK
    RCC_PCLK2Config(RCC_HCLK_Div1); 

    // APB1预分频因子设置为1分频，PCLK1 = HCLK/2 
    RCC_PCLK1Config(RCC_HCLK_Div2);
		
//-----------------设置各种频率主要就是在这里设置-------------------//
    // 设置PLL时钟来源为HSE，设置PLL倍频因子
		// PLLCLK = 4MHz * pllmul
		RCC_PLLConfig(RCC_PLLSource_HSI_Div2, pllmul);
//------------------------------------------------------------------//

    // 开启PLL 
    RCC_PLLCmd(ENABLE);

    // 等待 PLL稳定
    while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
    {
    }

    // 当PLL稳定之后，把PLL时钟切换为系统时钟SYSCLK
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

    // 读取时钟切换状态位，确保PLLCLK被选为系统时钟
    while (RCC_GetSYSCLKSource() != 0x08)
    {
    }
  }
  else
  { // 如果HSI开启失败，那么程序就会来到这里，用户可在这里添加出错的代码处理
		// 当HSE开启失败或者故障的时候，单片机会自动把HSI设置为系统时钟，
		// HSI是内部的高速时钟，8MHZ
    while (1)
    {
    }
  }
}



/**
  * @brief  晶振异常函数的处理函数在异常中断函数中调用此函数  
  * @note   灰色部分是内部晶振初始化的代码可以实现外部晶振“无缝”的切换到内部晶振
						(如果是用于特定的场合如：医疗设备可以使用此段代码)，
						但由于我们产品对实时性要求不是很高我还是决定重新重启设备再初始化晶振。
  * @param  None
  * @retval None
  */
void NMIException(void)
{
    if(RCC_GetITStatus(RCC_IT_CSS) != RESET)
    {
#ifdef RCC_CSS_EN                 //内部晶振配置
        RCC_DeInit();  //复位RCC寄存器
        RCC_HSEConfig(RCC_HSE_OFF);       //关闭外部晶（HSE）
        RCC_HCLKConfig(RCC_SYSCLK_Div1);   //设置ABH时钟（HCLK）：HCLK=SYSCLK
       //设置PLL时钟源及倍频系数， PLLCLK = HSI/2*PLLMul = 8/2*12 = 48MHz 实际是：48MHz
        RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_12);
        RCC_PLLCmd(ENABLE); //使能PLL

        //检查指定的RCC标志位设置与否
        //while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);

        RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);          //设置系统时钟 (SYSCLK)

        //返回作用系统时钟的时钟源
        // Wait till PLL is used as system clock source
        while(RCC_GetSYSCLKSource() != 0x08);
        CLKOUT_8M_INIT();    //PA8端口时钟输出为24025提供8MHz时钟

        RCC_ITConfig(RCC_IT_HSIRDY,ENABLE);     //使能 HSI就绪中断
        RCC_ITConfig(RCC_IT_PLLRDY,ENABLE);     //使能 PLL就绪中断
#else
        NVIC_SystemReset();                     //重启
        RCC_ITConfig(RCC_IT_HSERDY,ENABLE);     //使能 HSE就绪中断
        RCC_ITConfig(RCC_IT_PLLRDY,ENABLE);     //使能 PLL就绪中断       
#endif
        RCC_ClearITPendingBit(RCC_IT_CSS);      //清除时钟安全系统中断的挂起位
    }
}


