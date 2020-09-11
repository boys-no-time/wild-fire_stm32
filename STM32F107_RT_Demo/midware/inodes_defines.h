/*********************************************************************
Copyright (c) 2020-2028 by inodes. All rights reserved.
You are not allowed to copy or distribute the code without permission.
*********************************************************************/

#ifndef __INODES_DEFINES_H__
#define __INODES_DEFINES_H__

#ifndef UNUSED
#define	UNUSED(x) (void)x
#endif

/* basic data types *********************/
#ifdef __CC_ARM //gcc认为重复定义是个错误
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
#endif

/* logging and debugging macros ***************************************/
#ifndef INODES_NO_DEBUG
#define DPRINTF(a) rt_kprintf a
#define EPRINTF(a) rt_kprintf a
#define FPRINTF(a) rt_kprintf a

#define assert(ex)  do {                                               		\
						if (!(ex)) {                                 		\
							rt_kprintf("Assertion failed: file \"%s\",	\
									line %d\n", __FILE__, __LINE__);   		\
						}                                            		\
                    } while (0)
#else
#define DPRINTF(a)
#define EPRINTF(a)
#define FPRINTF(a) rt_kprintf a

#define assert(ex)
#endif

typedef enum {FALSE = 0, TRUE} BOOL;

#define INODES_TRUE		(1)
#define INODES_FALSE	(0)

/* error codes: return -ERROR_XXX *******/
enum {
	INODES_EOK,
	INODES_ENET,
	INODES_EIO,
};

/* event and sub-events ******************/
enum {
	INODES_EVENT_TYPE_TIMER,
	INODES_EVENT_TYPE_NETWORK,
	INODES_EVENT_TYPE_UNKNOWN
};

//network events subset
enum {
	INODES_EVENT_NETWORK_SETBUF = 1,	//传递buffer地址给模组
	INODES_EVENT_NETWORK_CHANGED,		//网络连接状态变化
	INODES_EVENT_NETWORK_UNKNOWN
};

#endif //__INODES_DEFINES_H__

