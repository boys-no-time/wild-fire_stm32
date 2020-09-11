/*********************************************************************
Copyright (c) 2020-2028 by inodes. All rights reserved. 
You are not allowed to copy or distribute the code without permission. 
*********************************************************************/

#ifndef __SWDOG_H__
#define __SWDOG_H__

#include <rtdef.h>

#ifdef __cplusplus
extern "C" {
#endif

//下面的值不可更改
#define DOG_STATUS_AWEEK	1
#define DOG_STATUS_SLEEP	0

#define RT_SWDOG_THREAD_PRIORITY	24 //RT_THREAD_PRIORITY_MAX / 3
#define RT_SWDOG_THREAD_TICKS		10

#define NUM_DOG_FOOD	64

rt_int8_t swdog_create(rt_thread_t tid, rt_uint8_t status);

void swdog_feed(rt_int8_t index);

rt_mq_t swdog_get_mq(void);

void swdog_thread_create(void);

#ifdef __cplusplus
}
#endif

#endif //__SWDOG_H__

