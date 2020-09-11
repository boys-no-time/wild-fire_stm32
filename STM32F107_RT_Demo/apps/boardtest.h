#ifndef __BOARDTEST_H__
#define __BOARDTEST_H__
#include <rtthread.h>
#include "stm32f10x.h"

typedef struct _BOARD_TEST_TYPEDEF_ {
    rt_uint32_t UID[3];		//测试通过后，将芯片的UID复制至此处
    rt_uint32_t UTC;			//测试通过时的UTC时间戳，FFFFFFFFH保留
    rt_uint8_t RS485OK;		//RS485接口测试结果，通过测试时为5AH
    rt_uint8_t ETHOK;			//以太网接口测试结果，通过测试时为5AH
} BOARD_TEST_TYPEDEF;

extern BOARD_TEST_TYPEDEF board_test;

extern void test_thread_create(void);

#endif
