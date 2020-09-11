/*********************************************************************
Copyright (c) 2020-2028 by inodes. All rights reserved. 
You are not allowed to copy or distribute the code without permission. 
*********************************************************************/

#ifndef __INODES_PORT_H__
#define __INODES_PORT_H__

#ifndef UNUSED
#define	UNUSED(x) (void)x
#endif

#ifdef __cplusplus
extern "C"
{
#endif

u32 inodes_time_getms(void); //ϵͳ������ĺ�����

void inodes_time_delay(u32 ms); //��ʱָ���ĺ���

#ifdef __cplusplus
}
#endif


#endif //__INODES_PORT_H__

