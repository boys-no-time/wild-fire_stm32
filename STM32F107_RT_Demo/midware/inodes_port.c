/*********************************************************************
Copyright (c) 2020-2028 by inodes. All rights reserved. 
You are not allowed to copy or distribute the code without permission. 
*********************************************************************/
#include "inodes_include.h"

/* 本文件实现一些对底层功能的封装, 形成统一名称和功能的API。*/

#ifdef HAVE_MS_TICK

/* variables declared in rtt/kernel/clock.c */
extern volatile rt_tick_t inodes_time_tick;
extern volatile rt_tick_t inodes_delay_tick;

/* 基本时间函数: 系统启动后的毫秒数. */
u32 inodes_time_getms(void)
{
	return inodes_time_tick;
}

/* 基本时间函数: 延时指定的毫秒 */
void inodes_time_delay(u32 ms)
{
#if 1 //中断模式下
	inodes_delay_tick = ms;
	while(inodes_delay_tick); //会在中断handler里减
#else
	u32 tmp;
	SysTick->LOAD = ms * fac_ms;    //装载计数值
	SysTick->VAL  = 0x00;			//清空计数器
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk; //开始计数
	do {
		tmp = SysTick->CTRL;
	}
	while((tmp & 0x01)/*enabled*/ && (tmp & (1 << 16)) == 0);
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk; //停止计数
	SysTick->VAL  = 0x00; //清空计数器
#endif
}

#endif //HAVE_MS_TICK

#ifdef INODES_SUPPORT_BEEP

/* 以频率freq发声，响/停各period毫秒，重复times次 */
void inodes_beep_sound(u16 freq, u16 period, u16 times)
{
	//TODO
}

#endif //INODES_SUPPORT_BEEP

