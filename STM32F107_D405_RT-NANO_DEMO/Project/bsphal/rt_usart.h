#ifndef __RT_USART_H__
#define __RT_USART_H__

#include "inodes_include.h"

#define UART_ENABLE_IRQ(n)            NVIC_EnableIRQ((n))
#define UART_DISABLE_IRQ(n)           NVIC_DisableIRQ((n))

extern void rt_hw_usart_init(void);



#endif
