#include "bsp_clkconfig.h"
#include "stm32f10x_rcc.h"

/*
 * ʹ��HSEʱ������ϵͳʱ�ӵĲ���
 * 1������HSE �����ȴ� HSE �ȶ�
 * 2������ AHB��APB2��APB1��Ԥ��Ƶ����
 * 3������PLL��ʱ����Դ����PLL�ı�Ƶ���ӣ����ø���Ƶ����Ҫ��������������
 * 4������PLL�����ȴ�PLL�ȶ�
 * 5����PLLCK�л�Ϊϵͳʱ��SYSCLK
 * 6����ȡʱ���л�״̬λ��ȷ��PLLCLK��ѡΪϵͳʱ��
 */

/* ���� ϵͳʱ��:SYSCLK, AHB����ʱ��:HCLK, APB2����ʱ��:PCLK2, APB1����ʱ��:PCLK1
 * PCLK2 = HCLK = SYSCLK
 * PCLK1 = HCLK/2,���ֻ����36M
 * ����˵����pllmul��PLL�ı�Ƶ���ӣ��ڵ��õ�ʱ������ǣ�RCC_PLLMul_x , x:[2,3,...16]
 * ������User_SetSysClock(RCC_PLLMul_9);  ������ϵͳʱ��Ϊ��8MHZ * 9 = 72MHZ
 *       User_SetSysClock(RCC_PLLMul_16); ������ϵͳʱ��Ϊ��8MHZ * 16 = 128MHZ����Ƶ����
 *
 * HSE��Ϊʱ����Դ������PLL��Ƶ��Ϊϵͳʱ�ӣ�����ͨ��������
 */
 
void HSE_SetSysClock(uint32_t pllmul)
{
	__IO uint32_t StartUpCounter = 0, HSEStartUpStatus = 0;

	// ��RCC�����ʼ���ɸ�λ״̬������Ǳ����
  RCC_DeInit();

  //ʹ��HSE�������ⲿ���񣬼����õ���8M
  RCC_HSEConfig(RCC_HSE_ON);

  // �ȴ� HSE �����ȶ�
  HSEStartUpStatus = RCC_WaitForHSEStartUp();

#if 1	
	// ֻ�� HSE �ȶ�֮�����������ִ��
  if (HSEStartUpStatus == SUCCESS)
  {
		#ifdef RCC_CSS_EN 
		RCC_ClockSecuritySystemCmd(ENABLE); //����ʱ�Ӱ�ȫϵͳ CSS   
		#endif
		
//----------------------------------------------------------------------//
    // ʹ��FLASH Ԥ��ȡ������
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

    // SYSCLK�������������ʱ��ı������ã�����ͳһ���ó�2
		// ���ó�2��ʱ��SYSCLK����48MҲ���Թ�����������ó�0����1��ʱ��
		// ������õ�SYSCLK�����˷�Χ�Ļ���������Ӳ�����󣬳��������
		// 0��0 < SYSCLK <= 24M
		// 1��24< SYSCLK <= 48M
		// 2��48< SYSCLK <= 72M
    FLASH_SetLatency(FLASH_Latency_2);
//----------------------------------------------------------------------//
 
    // AHBԤ��Ƶ��������Ϊ1��Ƶ��HCLK = SYSCLK 
    RCC_HCLKConfig(RCC_SYSCLK_Div1); 
  
    // APB2Ԥ��Ƶ��������Ϊ1��Ƶ��PCLK2 = HCLK
    RCC_PCLK2Config(RCC_HCLK_Div1); 

    // APB1Ԥ��Ƶ��������Ϊ1��Ƶ��PCLK1 = HCLK/2 
    RCC_PCLK1Config(RCC_HCLK_Div2);
		
//-----------------���ø���Ƶ����Ҫ��������������-------------------//
    // ����PLLʱ����ԴΪHSE������PLL��Ƶ����
		// PLLCLK = 8MHz * pllmul
		RCC_PLLConfig(RCC_PLLSource_HSE_Div1, pllmul);
//------------------------------------------------------------------//
    RCC_PLLCmd(ENABLE);    // ����PLL 

    // �ȴ� PLL�ȶ�
    while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);

    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);    // ��PLL�ȶ�֮�󣬰�PLLʱ���л�Ϊϵͳʱ��SYSCLK

    // ��ȡʱ���л�״̬λ��ȷ��PLLCLK��ѡΪϵͳʱ��
    while (RCC_GetSYSCLKSource() != 0x08);
  }
#endif	
  else
  { // ���HSE����ʧ�ܣ���ô����ͻ���������û�����������ӳ���Ĵ��봦��
		// ��HSE����ʧ�ܻ��߹��ϵ�ʱ�򣬵�Ƭ�����Զ���HSI����Ϊϵͳʱ�ӣ�
		// HSI���ڲ��ĸ���ʱ�ӣ�8MHZ
		
        RCC_DeInit();  //��λRCC�Ĵ���
        RCC_HSEConfig(RCC_HSE_OFF);       //�ر��ⲿ����HSE��
        RCC_HCLKConfig(RCC_SYSCLK_Div1);   //����ABHʱ�ӣ�HCLK����HCLK=SYSCLK
        //����PLLʱ��Դ����Ƶϵ���� PLLCLK = HSI/2*PLLMul = 8/2*12 = 48MHz ʵ���ǣ�48MHz
        RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_12);
        RCC_PLLCmd(ENABLE); //ʹ��PLL

        //���ָ����RCC��־λ�������
        while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET); // Wait till PLL is ready

        RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);          //����ϵͳʱ�� (SYSCLK)

        //��������ϵͳʱ�ӵ�ʱ��Դ
        // Wait till PLL is used as system clock source
        while(RCC_GetSYSCLKSource() != 0x08);
		
  }
}

/*
 * ʹ��HSIʱ������ϵͳʱ�ӵĲ���
 * 1������HSI �����ȴ� HSI �ȶ�
 * 2������ AHB��APB2��APB1��Ԥ��Ƶ����
 * 3������PLL��ʱ����Դ����PLL�ı�Ƶ���ӣ����ø���Ƶ����Ҫ��������������
 * 4������PLL�����ȴ�PLL�ȶ�
 * 5����PLLCK�л�Ϊϵͳʱ��SYSCLK
 * 6����ȡʱ���л�״̬λ��ȷ��PLLCLK��ѡΪϵͳʱ��
 */

/* 
 * HSI��Ϊʱ����Դ������PLL��Ƶ��Ϊϵͳʱ�ӣ�������HSE���ϵ�ʱ���ʹ�õķ���
 * HSI����Ϊ�¶ȵ�ԭ�����Ư�ƣ����ȶ���һ�㲻����HSI��Ϊʱ����Դ���������Ȳ����ѵ����
 * ���HSIҪ��ΪPLLʱ�ӵ���Դ�Ļ����������Ƶ֮��ſ��ԣ���HSI/2����PLL��Ƶ�������ֻ����16
 * ���Ե�ʹ��HSI��ʱ��SYSCLK���ֻ����4M*16=64M
 */

void HSI_SetSysClock(uint32_t pllmul)
{
	__IO uint32_t HSIStartUpStatus = 0;

	// ��RCC�����ʼ���ɸ�λ״̬������Ǳ����
  RCC_DeInit();

  //ʹ��HSI
	RCC_HSICmd(ENABLE);
	
  // �ȴ� HSI ����
	HSIStartUpStatus = RCC->CR & RCC_CR_HSIRDY;
	
	// ֻ�� HSI����֮�����������ִ��
  if (HSIStartUpStatus == RCC_CR_HSIRDY)
  {
//----------------------------------------------------------------------//
    // ʹ��FLASH Ԥ��ȡ������
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

    // SYSCLK�������������ʱ��ı������ã�����ͳһ���ó�2
		// ���ó�2��ʱ��SYSCLK����48MҲ���Թ�����������ó�0����1��ʱ��
		// ������õ�SYSCLK�����˷�Χ�Ļ���������Ӳ�����󣬳��������
		// 0��0 < SYSCLK <= 24M
		// 1��24< SYSCLK <= 48M
		// 2��48< SYSCLK <= 72M
    FLASH_SetLatency(FLASH_Latency_2);
//----------------------------------------------------------------------//
 
    // AHBԤ��Ƶ��������Ϊ1��Ƶ��HCLK = SYSCLK 
    RCC_HCLKConfig(RCC_SYSCLK_Div1); 
  
    // APB2Ԥ��Ƶ��������Ϊ1��Ƶ��PCLK2 = HCLK
    RCC_PCLK2Config(RCC_HCLK_Div1); 

    // APB1Ԥ��Ƶ��������Ϊ1��Ƶ��PCLK1 = HCLK/2 
    RCC_PCLK1Config(RCC_HCLK_Div2);
		
//-----------------���ø���Ƶ����Ҫ��������������-------------------//
    // ����PLLʱ����ԴΪHSE������PLL��Ƶ����
		// PLLCLK = 4MHz * pllmul
		RCC_PLLConfig(RCC_PLLSource_HSI_Div2, pllmul);
//------------------------------------------------------------------//

    // ����PLL 
    RCC_PLLCmd(ENABLE);

    // �ȴ� PLL�ȶ�
    while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
    {
    }

    // ��PLL�ȶ�֮�󣬰�PLLʱ���л�Ϊϵͳʱ��SYSCLK
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

    // ��ȡʱ���л�״̬λ��ȷ��PLLCLK��ѡΪϵͳʱ��
    while (RCC_GetSYSCLKSource() != 0x08)
    {
    }
  }
  else
  { // ���HSI����ʧ�ܣ���ô����ͻ���������û�����������ӳ���Ĵ��봦��
		// ��HSE����ʧ�ܻ��߹��ϵ�ʱ�򣬵�Ƭ�����Զ���HSI����Ϊϵͳʱ�ӣ�
		// HSI���ڲ��ĸ���ʱ�ӣ�8MHZ
    while (1)
    {
    }
  }
}



/**
  * @brief  �����쳣�����Ĵ��������쳣�жϺ����е��ô˺���  
  * @note   ��ɫ�������ڲ������ʼ���Ĵ������ʵ���ⲿ�����޷족���л����ڲ�����
						(����������ض��ĳ����磺ҽ���豸����ʹ�ô˶δ���)��
						���������ǲ�Ʒ��ʵʱ��Ҫ���Ǻܸ��һ��Ǿ������������豸�ٳ�ʼ������
  * @param  None
  * @retval None
  */
void NMIException(void)
{
    if(RCC_GetITStatus(RCC_IT_CSS) != RESET)
    {
#ifdef RCC_CSS_EN                 //�ڲ���������
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


