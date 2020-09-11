/*********************************************************************
Copyright (c) 2020-2028 by inodes. All rights reserved. 
You are not allowed to copy or distribute the code without permission. 
*********************************************************************/
#include "inodes_include.h"

#ifdef INODES_USING_SWDOG

#define MAX_DOG_NUM 10
typedef struct {
    rt_thread_t dogthread;
    rt_mq_t mq_dog; //���������̺߳�˯/���ѹ�
    rt_uint8_t num_dogs;
    rt_thread_t dog_tid[MAX_DOG_NUM];	// thread ID of dog
    rt_uint8_t dog_timer[MAX_DOG_NUM];	// feed count of dog
    rt_uint8_t dog_status[MAX_DOG_NUM];	// status of dog
    rt_mutex_t mtx;						// ������
} SWDOGTYPEDEF;

SWDOGTYPEDEF swdog;

/* ���Ź���ʱ����40KHz. parameter of IWDG_SetReload(): max to 0xFFF=4095 
** ������ʱ��(s)��Tout=Reload/freq (freq=40000/n, n��IWDG_Prescaler_n��n) */

void IWDG_Init(void)
{
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
    IWDG_SetPrescaler(IWDG_Prescaler_256); //��Ƶ��156.25Hz
    IWDG_SetReload(2400); //��Լ30��(���ע���е�����)
    /* �����������ã�ι��ʱ��Ӧ����(s): 2400/156.25=15.36 */
    IWDG_ReloadCounter();
    IWDG_Enable();
	
    swdog.mq_dog = rt_mq_create("swdogMQ", sizeof(rt_int8_t), MAX_DOG_NUM, RT_IPC_FLAG_FIFO);
    swdog.mtx =  rt_mutex_create("swdogMTX", RT_IPC_FLAG_FIFO);
}

void IWDG_Feed(void)
{
    IWDG_ReloadCounter();
}

rt_int8_t swdog_create(rt_thread_t tid, rt_uint8_t status)
{
	rt_int8_t ret = -1;

	MUTEX_TAKE(swdog.mtx, RT_WAITING_FOREVER);
    if (swdog.num_dogs < MAX_DOG_NUM) {
	    swdog.dog_tid[swdog.num_dogs] = tid;
	    swdog.dog_timer[swdog.num_dogs] = NUM_DOG_FOOD; //��ʼ����
	    swdog.dog_status[swdog.num_dogs] = status; //��ʼ״̬
	    ret = swdog.num_dogs ++;
	}
	MUTEX_RELEASE(swdog.mtx);
	rt_kprintf("[swdog_create] created index=%d for %s...\n\n", ret, tid->name);
	return ret;
}

void swdog_feed(rt_int8_t index)
{
	MUTEX_TAKE(swdog.mtx, RT_WAITING_FOREVER);
    if (index >= 0 && index < swdog.num_dogs) {
		/* һ��ι���ٹ�������? */
	    swdog.dog_timer[index] = NUM_DOG_FOOD;
	}
	else {
		rt_kprintf("[swdog_feed] WARNING!!! illeagal index=%d...\n\n", index);
	}
	MUTEX_RELEASE(swdog.mtx);
}

rt_mq_t swdog_get_mq()
{
	return swdog.mq_dog;
}

void swdog_thread_func(void *parameter)
{
    UNUSED(parameter);
    rt_uint8_t feed_flag, i;
	rt_int8_t index;
    static rt_int8_t dog_index;
	
    //�ڴ˴���ʼ��Ӳ�����Ź�
    IWDG_Init();
	
	rt_kprintf("[thread] swdog_thread_func starting...\n\n");
    dog_index = swdog_create(swdog.dogthread, DOG_STATUS_AWEEK);
    while (1) {
		MUTEX_TAKE(swdog.mtx, RT_WAITING_FOREVER);
		//�����û�ж�����
        while (rt_mq_recv(swdog.mq_dog, &index, sizeof(index), RT_WAITING_NO) == RT_EOK) {
			swdog.dog_status[index] = !swdog.dog_status[index];
        }

		//�����û�ж�����
        feed_flag = 1;
        for (i = 0; i < swdog.num_dogs; i++) {
            if (swdog.dog_timer[i] != 0) {
                if(swdog.dog_status[i]==DOG_STATUS_AWEEK)
					swdog.dog_timer[i] --;
            } 
			else {
                feed_flag = 0;
                rt_kprintf("[swdog] dog of thread '%s' starved!\n\n", swdog.dog_tid[i]->name);
				break;
            }
        }
		MUTEX_RELEASE(swdog.mtx);
		
        swdog_feed(dog_index); //�Լ���ʳ

		//���û�ж����ģ��ڴ˴�ιӲ�����Ź�
        if (feed_flag == 1) {
            IWDG_Feed();
            feed_flag = 0;
        }
		
        //rt_thread_delay(100);
        rt_thread_yield();
    }
}

void swdog_thread_create(void)
{
    rt_memset(&swdog, 0, sizeof(swdog));
    swdog.dogthread = rt_thread_create("swdogthread",
                                       swdog_thread_func, RT_NULL,
                                       512, RT_SWDOG_THREAD_PRIORITY, RT_SWDOG_THREAD_TICKS);
    if (swdog.dogthread != RT_NULL) {
		rt_kprintf("[thread] swdog 0x%x created\n\n", swdog.dogthread);
		rt_thread_startup(swdog.dogthread);
    }
}

#endif //INODES_USING_SWDOG

