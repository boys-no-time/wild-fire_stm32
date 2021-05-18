/******************************************************************************/
/* RETARGET.C: 'Retarget' layer for target-dependent low level functions      */
/******************************************************************************/
/* This file is part of the uVision/ARM development tools.                    */
/* Copyright (c) 2005 Keil Software. All rights reserved.                     */
/* This software may only be used under the terms of a valid, current,        */
/* end user licence from KEIL for a compatible version of KEIL software       */
/* development tools. Nothing else gives you the right to use this software.  */
/******************************************************************************/

#include <stdio.h>
#include <time.h>

#include "stm32f4xx.h"
#include "stm32F4xg_atk.h"

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

#ifndef __CC_ARM //using Keil

int __io_putchar(int ch)
{
    UNUSED(ch);
	//TODO: USART output here
	return 0;
}
#endif


///�ض���c�⺯��printf������DEBUG_USART���ض�����ʹ��printf����
PUTCHAR_PROTOTYPE
{
		/* ����һ���ֽ����ݵ�����DEBUG_USART */
		USART_SendData(DEBUG_USART, (uint8_t) ch);
		
		/* �ȴ�������� */
		while (USART_GetFlagStatus(DEBUG_USART, USART_FLAG_TXE) == RESET);		
	
		return (ch);
}

///�ض���c�⺯��scanf������DEBUG_USART����д����ʹ��scanf��getchar�Ⱥ���
int fgetc(FILE *f)
{
		/* �ȴ������������� */
		while (USART_GetFlagStatus(DEBUG_USART, USART_FLAG_RXNE) == RESET);

		return (int)USART_ReceiveData(DEBUG_USART);
}

int ferror(FILE *f) {
  /* Your implementation of ferror */
  return EOF;
}

int _write(int fd, char *ptr, int len)
{
    int i = 0;
		FILE *f;
		UNUSED(fd);
    
    while (*ptr && (i < len)) {
        fputc(*ptr,f);
        if (*ptr == '\n') {
            fputc('\r',f);
        }
        i++;
        ptr++;
    }
    return i;
}

int _read(int fd, char *ptr, int len)
{
    UNUSED(fd);
    UNUSED(ptr);

    /* Read "len" of char to "ptr" from file id "fd"
     * Return number of char read.
     * Need implementing with UART here. */
    return len;
}

void _sys_exit(int x)
{
    x = x;
}
 
void _ttywrch(int x)
{
    x = x;
}

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









