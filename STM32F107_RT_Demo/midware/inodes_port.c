/*********************************************************************
Copyright (c) 2020-2028 by inodes. All rights reserved. 
You are not allowed to copy or distribute the code without permission. 
*********************************************************************/
#include "inodes_include.h"

/* ���ļ�ʵ��һЩ�Եײ㹦�ܵķ�װ, �γ�ͳһ���ƺ͹��ܵ�API��*/

#ifdef HAVE_MS_TICK

/* variables declared in rtt/kernel/clock.c */
extern volatile rt_tick_t inodes_time_tick;
extern volatile rt_tick_t inodes_delay_tick;

/* ����ʱ�亯��: ϵͳ������ĺ�����. */
u32 inodes_time_getms(void)
{
	return inodes_time_tick;
}

/* ����ʱ�亯��: ��ʱָ���ĺ��� */
void inodes_time_delay(u32 ms)
{
#if 1 //�ж�ģʽ��
	inodes_delay_tick = ms;
	while(inodes_delay_tick); //�����ж�handler���
#else
	u32 tmp;
	SysTick->LOAD = ms * fac_ms;    //װ�ؼ���ֵ
	SysTick->VAL  = 0x00;			//��ռ�����
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk; //��ʼ����
	do {
		tmp = SysTick->CTRL;
	}
	while((tmp & 0x01)/*enabled*/ && (tmp & (1 << 16)) == 0);
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk; //ֹͣ����
	SysTick->VAL  = 0x00; //��ռ�����
#endif
}

#endif //HAVE_MS_TICK

#ifdef INODES_SUPPORT_BEEP

/* ��Ƶ��freq��������/ͣ��period���룬�ظ�times�� */
void inodes_beep_sound(u16 freq, u16 period, u16 times)
{
	//TODO
}

#endif //INODES_SUPPORT_BEEP

