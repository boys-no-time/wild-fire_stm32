/* RCC CSS 时钟安全系统
	当外部时钟HSE故障时，产生CSS中断，在中断里面采取补救措施，
	重新设置系统时钟使用内部时钟，使其能在HSE故障时自动切换到HSI时钟
 * 1、开启CSS功能
HAL_RCC_EnableCSS();	// SystemClock_Config()函数中，

 * 2、NMI_Handler() 中断函数中调用 :
									HAL_RCC_NMI_IRQHandler()  
此中断处理函数中调用回调函数：
									HAL_RCC_CSSCallback();

 * 3、实现stm32f4xx_hal_rcc.c 中的回调函数 HAL_RCC_CSSCallback();
 在里面重新设置时钟使用HSI时钟

*/