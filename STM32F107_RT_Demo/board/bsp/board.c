/*
 * File      : board.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006 - 2013 RT-Thread Develop Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2009-01-05     Bernard      first implementation
 */

#include "inodes_include.h"


/*******************************************************************************
 * Function Name  : NVIC_ConfigurationB
 * Description    : Configures Vector Table base location.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void NVIC_ConfigurationB(void)
{
    uint32_t offset;

#ifdef IAP_SLAVE //online upgrade supported
     offset = 0x4000; 
#else    
     offset = 0;
#endif    

#ifdef  VECT_TAB_RAM
    /* Set the Vector Table base location at 0x20000000 */
    NVIC_SetVectorTable(NVIC_VectTab_RAM, offset);
#else  /* VECT_TAB_FLASH  */
    /* Set the Vector Table base location at 0x08000000 */
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, offset);
#endif
}

/**
 * This is the timer interrupt service routine.
 *
 */
void SysTick_Handler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();
#ifdef HAVE_MS_TICK
    inodes_tick_increase();
#else
    rt_tick_increase();
#endif
    /* leave interrupt */
    rt_interrupt_leave();
}

#ifdef INODES_HAVE_ETH_BT
#error "do something here"

#endif
/*获取硬件版本号输入引脚与网络类型选择按键引脚初始化*/
void board_set_gpio(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD , ENABLE);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD ;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz ;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
	
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
		GPIO_Init(GPIOC, &GPIO_InitStructure);	
}

void nextai_dataout_config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
	
/* Configure the F030 NRST(PB5) level is LOW RESET MCU */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE);
/*设置引脚模式为通用推挽输出*/
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 	
	
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;	
		GPIO_Init(GPIOB, &GPIO_InitStructure);
		GPIO_SetBits(GPIOB,GPIO_Pin_5);
}



/**
 * This function will initial STM32 board.
 */
void rt_hw_board_init(void)
{
    /* NVIC Configuration */
    NVIC_ConfigurationB();

    /* Configure the SysTick */
#ifdef HAVE_MS_TICK
    SysTick_Config(SystemCoreClock / 1000);
#else
    SysTick_Config(SystemCoreClock / RT_TICK_PER_SECOND);
#endif
		board_set_gpio();
    nextai_dataout_config();
	
#ifdef INODES_HAVE_ETH_BT
	#error "init your devices here"
    //
    //HalCom_Init();
#endif

}

void  delay(uint32_t nCount)
{
    do {
        __asm volatile("nop");
    } while (nCount --);
}

static char board_ver[12] = "1.0";
char *board_getver(void)
{
    uint8_t sw;

    if (GPIOD->IDR & GPIO_Pin_11) sw |= 0x01;
    if (GPIOD->IDR & GPIO_Pin_10) sw |= 0x02;
    if (GPIOD->IDR & GPIO_Pin_9) sw |= 0x04;
    if (GPIOD->IDR & GPIO_Pin_8) sw |= 0x08;


    if (sw == 0x00) {
        strcpy(board_ver, "1.0");
    } else if (sw == 0x01) {
        strcpy(board_ver, "1.1");
    } else if (sw == 0x02) {
        strcpy(board_ver, "1.2");
    } else if (sw == 0x03) {
        strcpy(board_ver, "1.3");
    } else if (sw == 0x04) {
        strcpy(board_ver, "1.4");
    }
    return (char *) board_ver;
}

