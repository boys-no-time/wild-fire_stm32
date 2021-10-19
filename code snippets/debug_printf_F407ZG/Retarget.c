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
#include <rt_misc.h>

/*如果使用c标准库函数，则要避免链接使用半主机模式的函数，
retarge.c文件中的#pragma import(__use_no_semihosting_swi) 
和_sys_exit函数实现就是来确保不链接半主机模式函数的。*/

#pragma import(__use_no_semihosting_swi)

#ifdef __cplusplus
extern "C" {
#endif

#ifndef UNUSED
#define UNUSED(x)	((void)(x))	/* to avoid warnings */
#endif

#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE* f)
#endif /* __GNUC__ */


extern int  sendchar(uint8_t ch);  /* in Serial_debug.c */
extern int  getkey(void);      /* in Serial_debug.c */

extern long timeval;           /* in Time.c   */


struct __FILE { int handle; /* Add whatever you need here */ };
FILE __stdout;
FILE __stdin;

#ifndef __CC_ARM //not using Keil

int __io_putchar(int ch)
{
    UNUSED(ch);
	//TODO: USART output here
	return 0;
}
#endif

PUTCHAR_PROTOTYPE 
{
  return (sendchar(ch));
}

int fgetc(FILE *f) {
  return (sendchar(getkey()));
}


int ferror(FILE *f) {
  /* Your implementation of ferror */
  return EOF;
}


void _ttywrch(int ch) {
  sendchar (ch);
}


void _sys_exit(int return_code) {
  while (1);    /* endless loop */
}

/* 下面的$Super/$Sub的意义: 如果你在这里只实现了sys_io.o中的部分symbol, 那么armlink会
** 加载sys_io.o的符号表先(but no implementation)，链接时会重复定义出错。下面方法避免. */

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


