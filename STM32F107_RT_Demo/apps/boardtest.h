#ifndef __BOARDTEST_H__
#define __BOARDTEST_H__
#include <rtthread.h>
#include "stm32f10x.h"

typedef struct _BOARD_TEST_TYPEDEF_ {
    rt_uint32_t UID[3];		//����ͨ���󣬽�оƬ��UID�������˴�
    rt_uint32_t UTC;			//����ͨ��ʱ��UTCʱ�����FFFFFFFFH����
    rt_uint8_t RS485OK;		//RS485�ӿڲ��Խ����ͨ������ʱΪ5AH
    rt_uint8_t ETHOK;			//��̫���ӿڲ��Խ����ͨ������ʱΪ5AH
} BOARD_TEST_TYPEDEF;

extern BOARD_TEST_TYPEDEF board_test;

extern void test_thread_create(void);

#endif
