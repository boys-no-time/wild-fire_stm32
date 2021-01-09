#ifndef __CLKCONFIG_H
#define	__CLKCONFIG_H

//#define RCC_CSS_EN 

#include "stm32f10x.h"

void HSE_SetSysClock(uint32_t pllmul);
void HSI_SetSysClock(uint32_t pllmul);

void NMIException(void);

#endif /* __CLKCONFIG_H */
