/*********************************************************************
Copyright (c) 2020-2028 by inodes. All rights reserved. 
You are not allowed to copy or distribute the code without permission. 
*********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>

#include "stm32f1xx.h"
#include "stm32f1xx_hal_conf.h"
#include "stm32f1xx_hal_def.h"
#include "stm32f1xx_hal.h"
#include "usart.h"

//extern	UART_HandleTypeDef huart3;

int __io_putchar(int ch);

#ifdef __CC_ARM //using Keil
int fputc(int ch, FILE *fp)
{
    //UNUSED(ch);
    UNUSED(fp);
	//TODO: USART output here
	//return 0;
    HAL_UART_Transmit(&huart4, (uint8_t *)&ch,1,HAL_MAX_DELAY);

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



void retarget_init()
{
    // Initialize UART
}

int _write(int fd, char *ptr, int len)
{
    UNUSED(fd);
    int i = 0;
    while (*ptr && (i < len)) {
        __io_putchar(*ptr);
        if (*ptr == '\n') {
            __io_putchar('\r');
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

void _ttywrch(int ch)
{
    __io_putchar(ch);
    return;
}

void _sys_exit(int returncode)   /* never returns */
{
    UNUSED(returncode);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

/* 下面�?$Super/$Sub的意�?: 如果你在这里只实现了sys_io.o中的部分symbol, 那么armlink�?
** 加载sys_io.o的符号表�?(but no implementation)，链接时会重复定义出错�?�下面方法避�?. */

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

#if 0
/*
 * Close a file. Should return 0 on success or a negative value on error.
 */
int _sys_close(FILEHANDLE fh)
{
	return 0;
}

/*
 * Write to a file. Returns 0 on success, negative on error, and
 * the number of characters _not_ written on partial success.
 * `mode' exists for historical reasons and must be ignored.
 */
int _sys_write(FILEHANDLE fh, const unsigned char *buf, unsigned len, int mode)
{
	return 0;
}

/*
 * Read from a file. Can return:
 *  - zero if the read was completely successful
 *  - the number of bytes _not_ read, if the read was partially successful
 *  - the number of bytes not read, plus the top bit set (0x80000000), if
 *    the read was partially successful due to end of file
 *  - -1 if some error other than EOF occurred
 *
 * It is also legal to signal EOF by returning no data but
 * signalling no error (i.e. the top-bit-set mechanism need never
 * be used).
 *
 * So if (for example) the user is trying to read 8 bytes at a time
 * from a file in which only 5 remain, this routine can do three
 * equally valid things:
 *
 *  - it can return 0x80000003 (3 bytes not read due to EOF)
 *  - OR it can return 3 (3 bytes not read), and then return
 *    0x80000008 (8 bytes not read due to EOF) on the next attempt
 *  - OR it can return 3 (3 bytes not read), and then return
 *    8 (8 bytes not read, meaning 0 read, meaning EOF) on the next
 *    attempt
 *
 * `mode' exists for historical reasons and must be ignored.
 */
int _sys_read(FILEHANDLE fh, unsigned char *buf, unsigned len, int mode)
{
	return 0;
}

/*
 * Write a character to the output channel. This function is used
 * for last-resort error message output.
 */
extern void _ttywrch(int /*ch*/);

/*
 * Return non-zero if the argument file is connected to a terminal.
 */
int _sys_istty(FILEHANDLE fh)
{
	return 0;
}

/*
 * Move the file position to a given offset from the file start.
 * Returns >=0 on success, <0 on failure.
 */
int _sys_seek(FILEHANDLE fh, long pos)
{
	return 0;
}

/*
 * Flush any OS buffers associated with fh, ensuring that the file
 * is up to date on disk. Result is >=0 if OK, negative for an
 * error.
 */
int _sys_ensure(FILEHANDLE fh)
{
	return 0;
}

/*
 * Return the current length of a file, or <0 if an error occurred.
 * _sys_flen is allowed to reposition the file pointer (so Unix can
 * implement it with a single lseek, for example), since it is only
 * called when processing SEEK_END relative fseeks, and therefore a
 * call to _sys_flen is always followed by a call to _sys_seek.
 */
long _sys_flen(FILEHANDLE fh)
{
	return 0;
}

/*
 * Return the name for temporary file number sig in the buffer
 * name. Returns 0 on failure. maxlen is the maximum name length
 * allowed.
 */
int _sys_tmpnam(char * name, int sig, unsigned maxlen)
{
	return 0;
}

/*
 * Terminate the program, passing a return code back to the user.
 * This function may not return.
 */
void _sys_exit(int returncode)   /* never returns */
{
}

/*
 * Return a pointer to the command line used to invoke the program.
 * The supplied buffer may be used to store the string, but need
 * not be.
 */
char *_sys_command_string(char * cmd, int len)
{
	return NULL;
}

#endif
