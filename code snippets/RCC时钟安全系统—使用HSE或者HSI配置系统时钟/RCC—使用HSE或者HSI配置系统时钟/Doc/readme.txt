
void INIT_CLOCK(void)
{   
#if 1
    ErrorStatus HSEStartUpStatus;
    RCC_DeInit();  //复位RCC寄存器
    RCC_HSEConfig(RCC_HSE_ON);  //设置外部高速晶振 （HSE）
    HSEStartUpStatus = RCC_WaitForHSEStartUp();  //等待 HSE 起振
#if 1
    //判断外部晶振是否OK，如果OK就用外部晶振，如果不行，就用内部晶振，优先用外部晶振。
    if(HSEStartUpStatus == SUCCESS)
    {
        RCC_ClockSecuritySystemCmd(ENABLE); //启动时钟安全系统 CSS
        //外部晶振配置
        RCC_HCLKConfig(RCC_SYSCLK_Div1);   //设置ABH时钟（HCLK）：HCLK=SYSCLK
        //设置PLL时钟源及倍频系数， PLLCLK = HSE*PLLMul = 8*8 = 64MHz 实际是：8*8=64MHz
        RCC_PLLConfig(RCC_PLLSource_PREDIV1, RCC_PLLMul_2);
        RCC_PLLCmd(ENABLE); //使能PLL

        //检查指定的RCC标志位设置与否
        while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET); // Wait till PLL is ready

        RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);  //设置系统时钟 (SYSCLK)

        //返回作用系统时钟的时钟源
        // Wait till PLL is used as system clock source
        while(RCC_GetSYSCLKSource() != 0x08);
    }
    else
#endif
    {
        //内部晶振配置
        RCC_DeInit();  //复位RCC寄存器
        RCC_HSEConfig(RCC_HSE_OFF);       //关闭外部晶（HSE）
        RCC_HCLKConfig(RCC_SYSCLK_Div1);   //设置ABH时钟（HCLK）：HCLK=SYSCLK
        //设置PLL时钟源及倍频系数， PLLCLK = HSI/2*PLLMul = 8/2*12 = 48MHz 实际是：48MHz
        RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_12);
        RCC_PLLCmd(ENABLE); //使能PLL

        //检查指定的RCC标志位设置与否
        //while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET); // Wait till PLL is ready

        RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);          //设置系统时钟 (SYSCLK)

        //返回作用系统时钟的时钟源
        // Wait till PLL is used as system clock source
        while(RCC_GetSYSCLKSource() != 0x08);
    }    
#endif
}

void NMIException(void)
{
    if(RCC_GetITStatus(RCC_IT_CSS) != RESET)
    {
#if 0                 //内部晶振配置
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


void NMI_Handler(void)
{
    NMIException();
}








