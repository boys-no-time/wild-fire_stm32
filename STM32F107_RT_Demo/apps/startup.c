/*
 * File      : startup.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006 - 2013, RT-Thread Develop Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://openlab.rt-thread.com/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2006-08-31     Bernard      first implementation
 * 2011-06-05     Bernard      modify for STM32F107 version
 */

#include "inodes_include.h"

/**
 * @addtogroup STM32
 */

/*@{*/

extern int  rt_application_init(void);

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

static void inodes_print_clocks(void)
{
	RCC_ClocksTypeDef RCC_ClocksStructure;
	RCC_GetClocksFreq(&RCC_ClocksStructure);
	DPRINTF(("\tSystemCoreClock: %d\n", SystemCoreClock));
	DPRINTF(("\t SYSCLK: %d\n", RCC_ClocksStructure.SYSCLK_Frequency));
	DPRINTF(("\t   HCLK: %d\n", RCC_ClocksStructure.HCLK_Frequency));
	DPRINTF(("\t   PCLK: %d\n", RCC_ClocksStructure.PCLK1_Frequency));
	DPRINTF(("\t   PCLK: %d\n", RCC_ClocksStructure.PCLK2_Frequency));
	DPRINTF(("\t ADCCLK: %d\n", RCC_ClocksStructure.ADCCLK_Frequency));
}

/**
 * This function will startup RT-Thread RTOS.
 */
void rtthread_startup(void)
{
    /* init board */
    rt_hw_board_init();

    /* init tick */
    rt_system_tick_init();

    /* init kernel object */
    rt_system_object_init();

    /* init timer system */
    rt_system_timer_init();

#ifdef RT_USING_HEAP
#ifdef __CC_ARM
    rt_system_heap_init((void *)STM32_SRAM_BEGIN, (void *)STM32_SRAM_END);
#else //__GNUC__
    rt_system_heap_init((void *)heap_mem, (void *)(((int) heap_mem) + HEAP_SIZE));
#endif
#endif

    rt_hw_usart_init();

    rt_console_set_device(RT_CONSOLE_DEVICE_NAME);

    rt_kprintf("\r\nMain booting ....\r\n");
		inodes_print_ver((PRINTFUNC)rt_kprintf);
    /* show version */
    rt_show_version();

	/* print clocks */
	inodes_print_clocks();

#ifdef INODES_HAVE_LED_CTRL
    Hal_Led_Init();
//    SIGN_LED_PWR_ON;
#endif

    /* init scheduler system */
    rt_system_scheduler_init();

#ifdef INODES_USING_SWDOG
    swdog_thread_create();
#endif

    /* init application */
    rt_application_init();

    /* init timer thread */
    rt_system_timer_thread_init();

    /* init idle thread */
    rt_thread_idle_init();

    /* start scheduler */
    rt_system_scheduler_start();

    /* never reach here */
    return ;
}

int main(void)
{
    /* disable interrupt first */
    rt_hw_interrupt_disable();

    /* startup RT-Thread RTOS */
    rtthread_startup();

    return 0;
}

/*******************************************************************************
* Function Name  : assert_failed
* Description    : Reports the name of the source file and the source line number
*                  where the assert error has occurred.
* Input          : - file: pointer to the source file name
*                  - line: assert error line source number
* Output         : None
* Return         : None
*******************************************************************************/
void assert_failed(u8 *file, u32 line)
{
    rt_kprintf("\n\r Wrong parameter value detected on\r\n");
    rt_kprintf("       file  %s\r\n", file);
    rt_kprintf("       line  %d\r\n", line);

    while (1) ;
}

/*@}*/

