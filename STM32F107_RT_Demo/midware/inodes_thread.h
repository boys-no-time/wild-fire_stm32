/*********************************************************************
Copyright (c) 2020-2028 by inodes. All rights reserved. 
You are not allowed to copy or distribute the code without permission. 
*********************************************************************/

#ifndef __INODES_THREAD_H__
#define __INODES_THREAD_H__

#include <rtdef.h>
//#include <rtthread.h>

//#define INODES_MUTEX	0 //none
#define INODES_MUTEX	1 //system
//#define INODES_MUTEX	2 //print

/********** 定义一些宏,方便多进程编程的使用 ****************/

//mutex
#if (INODES_MUTEX == 0)
	#define MUTEX_TAKE(m, t)
	#define MUTEX_RELEASE(m)
#elif (INODES_MUTEX == 1)
	#define MUTEX_TAKE(m, t)	rt_mutex_take(m, t)
	#define MUTEX_RELEASE(m)	rt_mutex_release(m)
#elif (INODES_MUTEX == 2)
	/* 封装mutex函数不能保证原子操作，会出问题 */	
	#define MUTEX_TAKE(m, t)	rt_kprintf("[mutex] take from %s(%d).\n", __FILE__, __LINE__); rt_mutex_take(m, t)
	#define MUTEX_RELEASE(m)	rt_kprintf("[mutex] release at %s(%d).\n", __FILE__, __LINE__); rt_mutex_release(m)
#endif

#endif //__INODES_THREAD_H__

