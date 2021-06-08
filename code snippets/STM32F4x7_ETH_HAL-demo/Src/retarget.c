/*********************************************************************
Copyright (c) 2020-2028 by inodes. All rights reserved. 
You are not allowed to copy or distribute the code without permission. 
*********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>

#include "stm32f4xx.h"
#include "stm32f4xx_hal_conf.h"
#include "stm32f4xx_hal_def.h"
#include "stm32f4xx_hal.h"
#include "usart.h"


#ifndef UNUSED
#define UNUSED(x)	((void)(x))	/* to avoid warnings */
#endif

/*��ʹ��MicroLIB����Ҫ�ֶ��رհ�����ģʽ
4. �رհ�����ģʽ��ARM Compiler 5�汾����#pragma import(__use_no_semihosting_swi)
5. �رհ�����ģʽ��ARM Compiler 6�汾����__asm(".global __use_no_semihosting\n\t");
*/

#pragma import(__use_no_semihosting_swi) 

#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE* f)
#endif /* __GNUC__ */
 
#ifdef __cplusplus
extern "C" {
#endif

struct __FILE { int handle; /* Add whatever you need here */ };
FILE __stdout;
FILE __stdin;


extern	UART_HandleTypeDef huart1;

#ifdef __CC_ARM //using Keil
///�ض���c�⺯��printf������DEBUG_USART���ض�����ʹ��printf����
PUTCHAR_PROTOTYPE
{
	//TODO: USART output here
	//return 0;
    HAL_UART_Transmit(&huart1, (uint8_t *)&ch,1,HAL_MAX_DELAY);

		return ch;
}
#else
int __io_putchar(int ch)
{
    UNUSED(ch);
	//TODO: USART output here
	return 0;
}
#endif



void _ttywrch(int ch)
{
    HAL_UART_Transmit(&huart1, (uint8_t *)&ch,1,HAL_MAX_DELAY);
}

void _sys_exit(int returncode)   /* never returns */
{
    UNUSED(returncode);
		while(1);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

/* �����$Super/$Sub������: �����������ֻʵ����sys_io.o�еĲ���symbol, ��ôarmlink��
** ����sys_io.o�ķ��ű���(but no implementation)������ʱ���ظ�����������淽������. */

typedef int FILEHANDLE;
/*
 * Open a file. May return -1 if the file failed to open.
 */
extern void $Super$$_sys_open(void);
FILEHANDLE $Sub$$_sys_open(const char * name, int openmode)
{
    UNUSED(name);
    UNUSED(openmode);
	return 0;
}

