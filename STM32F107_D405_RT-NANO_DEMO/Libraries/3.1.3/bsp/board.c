/*
 * Copyright (c) 2006-2019, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2017-07-24     Tanek        the first version
 * 2018-11-12     Ernest Chen  modify copyright
 */
#include "inodes_include.h"

#ifdef __CC_ARM
	extern int Image$$RW_IRAM1$$ZI$$Limit;
	#define STM32_SRAM_BEGIN    (&Image$$RW_IRAM1$$ZI$$Limit)
#elif __ICCARM__
	#pragma section="HEAP"
	#define STM32_SRAM_BEGIN    (__segment_end("HEAP"))
#else
	extern int __bss_end;
	#define STM32_SRAM_BEGIN    (&__bss_end)
#endif

#ifdef __GNUC__
#define HEAP_SIZE  (1024*22)
char heap_mem[HEAP_SIZE] __attribute__((aligned(4)));
#endif

// Updates the variable SystemCoreClock and must be called 
// whenever the core clock is changed during program execution.
extern void SystemCoreClockUpdate(void);

//extern uint32_t SystemCoreClock;

void Hal_Led_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
	
/* Configure the F030 NRST(PB5) level is LOW RESET MCU */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE, ENABLE);

		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 	
	
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;	
		GPIO_Init(GPIOB, &GPIO_InitStructure);
		GPIO_SetBits(GPIOB,GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_5 );
	
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;	
		GPIO_Init(GPIOE, &GPIO_InitStructure);
		GPIO_SetBits(GPIOE,GPIO_Pin_0 | GPIO_Pin_1);

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;	
		GPIO_Init(GPIOD, &GPIO_InitStructure);
		GPIO_ResetBits(GPIOD,GPIO_Pin_1);
		
}

void LED_ACT_OFFON(uint8_t cmd,uint8_t on)
{
		switch(cmd)
		{
			case LED_STATUS:
					if(on)
					{
							GPIO_ResetBits(GPIOE, GPIO_Pin_0);
					}
					else
					{
							GPIO_SetBits(GPIOE, GPIO_Pin_0);
					}
					break;
			case LED_NET:
					if(on)
					{
							GPIO_ResetBits(GPIOE, GPIO_Pin_1);
					}
					else
					{
							GPIO_SetBits(GPIOE, GPIO_Pin_1);
					}
					break;				
			case LED_WIFI:
					if(on)
					{
							GPIO_ResetBits(GPIOB, GPIO_Pin_8);
					}
					else
					{
							GPIO_SetBits(GPIOB, GPIO_Pin_8);
					}
					break;	
			case LED_BL:
					if(on)
					{
							GPIO_ResetBits(GPIOB, GPIO_Pin_9);
					}
					else
					{
							GPIO_SetBits(GPIOB, GPIO_Pin_9);
					}
					break;
					
		}

}

void rt_hw_console_output(const char *str)
{
    rt_enter_critical();
		while(*str!='\0')
		{
			if(*str=='\n')
			{
					USART_SendData(DBG_UART, '\r');
					while (USART_GetFlagStatus(DBG_UART, USART_FLAG_TC) == RESET);
			}
			USART_SendData(DBG_UART, *str++);
			while (USART_GetFlagStatus(DBG_UART, USART_FLAG_TC) == RESET);
		}
}

void rt_hw_board_init()
{
    /* System Clock Update */
    SystemCoreClockUpdate();
    
    /* System Tick Configuration */
#ifdef HAVE_MS_TICK
    SysTick_Config(SystemCoreClock / 1000);
#else
    SysTick_Config(SystemCoreClock / RT_TICK_PER_SECOND);
#endif

#if defined(RT_USING_USER_MAIN) && defined(RT_USING_HEAP)
#ifdef __CC_ARM
    rt_system_heap_init((void *)STM32_SRAM_BEGIN, (void *)STM32_SRAM_END);
#else //__GNUC__
    rt_system_heap_init((void *)heap_mem, (void *)(((int) heap_mem) + HEAP_SIZE));
#endif
#endif	
	
    /* Call components board initial (use INIT_BOARD_EXPORT()) */
#ifdef RT_USING_COMPONENTS_INIT
    rt_components_board_init();
#endif
//	rt_hw_usart_init();
//  
		DBG_Usart_Init();
//		rt_console_set_device(RT_CONSOLE_DEVICE_NAME);
		rt_kprintf("\r\nMain booting ....\r\n");
	
		Hal_Led_Init();
		
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
