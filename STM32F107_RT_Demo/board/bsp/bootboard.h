
#ifndef __BOOT_BOARD_H_
#define __BOOT_BOARD_H_

#include "common.h"

extern volatile ticks_size SysTicks ;
static inline ticks_size SysGetTicks(void)
{
    return SysTicks;
}

void Usart_Init(void);
void Delay(uint32_t nCount);


#endif

