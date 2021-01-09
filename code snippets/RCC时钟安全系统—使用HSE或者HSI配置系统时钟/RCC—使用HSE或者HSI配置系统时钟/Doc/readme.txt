
void INIT_CLOCK(void)
{   
#if 1
    ErrorStatus HSEStartUpStatus;
    RCC_DeInit();  //��λRCC�Ĵ���
    RCC_HSEConfig(RCC_HSE_ON);  //�����ⲿ���پ��� ��HSE��
    HSEStartUpStatus = RCC_WaitForHSEStartUp();  //�ȴ� HSE ����
#if 1
    //�ж��ⲿ�����Ƿ�OK�����OK�����ⲿ����������У������ڲ������������ⲿ����
    if(HSEStartUpStatus == SUCCESS)
    {
        RCC_ClockSecuritySystemCmd(ENABLE); //����ʱ�Ӱ�ȫϵͳ CSS
        //�ⲿ��������
        RCC_HCLKConfig(RCC_SYSCLK_Div1);   //����ABHʱ�ӣ�HCLK����HCLK=SYSCLK
        //����PLLʱ��Դ����Ƶϵ���� PLLCLK = HSE*PLLMul = 8*8 = 64MHz ʵ���ǣ�8*8=64MHz
        RCC_PLLConfig(RCC_PLLSource_PREDIV1, RCC_PLLMul_2);
        RCC_PLLCmd(ENABLE); //ʹ��PLL

        //���ָ����RCC��־λ�������
        while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET); // Wait till PLL is ready

        RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);  //����ϵͳʱ�� (SYSCLK)

        //��������ϵͳʱ�ӵ�ʱ��Դ
        // Wait till PLL is used as system clock source
        while(RCC_GetSYSCLKSource() != 0x08);
    }
    else
#endif
    {
        //�ڲ���������
        RCC_DeInit();  //��λRCC�Ĵ���
        RCC_HSEConfig(RCC_HSE_OFF);       //�ر��ⲿ����HSE��
        RCC_HCLKConfig(RCC_SYSCLK_Div1);   //����ABHʱ�ӣ�HCLK����HCLK=SYSCLK
        //����PLLʱ��Դ����Ƶϵ���� PLLCLK = HSI/2*PLLMul = 8/2*12 = 48MHz ʵ���ǣ�48MHz
        RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_12);
        RCC_PLLCmd(ENABLE); //ʹ��PLL

        //���ָ����RCC��־λ�������
        //while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET); // Wait till PLL is ready

        RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);          //����ϵͳʱ�� (SYSCLK)

        //��������ϵͳʱ�ӵ�ʱ��Դ
        // Wait till PLL is used as system clock source
        while(RCC_GetSYSCLKSource() != 0x08);
    }    
#endif
}

void NMIException(void)
{
    if(RCC_GetITStatus(RCC_IT_CSS) != RESET)
    {
#if 0                 //�ڲ���������
        RCC_DeInit();  //��λRCC�Ĵ���
        RCC_HSEConfig(RCC_HSE_OFF);       //�ر��ⲿ����HSE��
        RCC_HCLKConfig(RCC_SYSCLK_Div1);   //����ABHʱ�ӣ�HCLK����HCLK=SYSCLK
       //����PLLʱ��Դ����Ƶϵ���� PLLCLK = HSI/2*PLLMul = 8/2*12 = 48MHz ʵ���ǣ�48MHz
        RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_12);
        RCC_PLLCmd(ENABLE); //ʹ��PLL

        //���ָ����RCC��־λ�������
        //while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);

        RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);          //����ϵͳʱ�� (SYSCLK)

        //��������ϵͳʱ�ӵ�ʱ��Դ
        // Wait till PLL is used as system clock source
        while(RCC_GetSYSCLKSource() != 0x08);
        CLKOUT_8M_INIT();    //PA8�˿�ʱ�����Ϊ24025�ṩ8MHzʱ��

        RCC_ITConfig(RCC_IT_HSIRDY,ENABLE);     //ʹ�� HSI�����ж�
        RCC_ITConfig(RCC_IT_PLLRDY,ENABLE);     //ʹ�� PLL�����ж�
#else
        NVIC_SystemReset();                     //����
        RCC_ITConfig(RCC_IT_HSERDY,ENABLE);     //ʹ�� HSE�����ж�
        RCC_ITConfig(RCC_IT_PLLRDY,ENABLE);     //ʹ�� PLL�����ж�       
#endif
        RCC_ClearITPendingBit(RCC_IT_CSS);      //���ʱ�Ӱ�ȫϵͳ�жϵĹ���λ
    }
}


void NMI_Handler(void)
{
    NMIException();
}








