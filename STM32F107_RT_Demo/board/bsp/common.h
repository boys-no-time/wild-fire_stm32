/*********************************************************************
Copyright (c) 2020-2028 by inodes. All rights reserved.
You are not allowed to copy or distribute the code without permission.
*********************************************************************/

#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdio.h>
#include "stm32f10x.h"
#include "rtthread.h"
#include "arch/sys_arch.h"
#include "lwip/err.h"
#include "lwip/sys.h"

/* miscellaneous macros and names *************************************/
#define MAX3(a,b,c) (a>b?(a>c?a:c):(b>c?b:c))

extern const char git_ver[];
extern const char git_info[];
typedef int (*PRINTFUNC) (const char *fmt, ...);
extern void inodes_print_ver(PRINTFUNC pf);

/* system ticks relative macros ***************************************/
#define tick_t                 rt_tick_t 
#define GetOSTick()            rt_tick_get()
#define OSSleep(DELAY_TICKS)   rt_thread_delay(DELAY_TICKS)
typedef int64_t ticks_size;		

/* IRQ relative macros *************************************************/
#define ENTER_IRQ() rt_interrupt_enter() 
#define EXIT_IRQ()  rt_interrupt_leave()

#ifdef __CC_ARM //using Keil
#define DisableMIRQ __disable_irq()
#define EnableMIRQ  __enable_irq()
#elif defined(__GNUC__)
#define DisableMIRQ __asm volatile ( " cpsid i " ::: "memory" )
#define EnableMIRQ  __asm volatile ( " cpsie i " ::: "memory" )
#endif

#endif

