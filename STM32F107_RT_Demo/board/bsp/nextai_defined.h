/*********************************************************************
Copyright (c) 2014 by Automation Interactive.
All rights reserved. You are not allowed to
copy or distribute the code without permission.
*********************************************************************/

#ifndef _NEXTAI_DEFINED__H_
#define _NEXTAI_DEFINED__H_


/*********************************************************************
ϵͳ�У��¶ȴ�����1ָ����¶ȣ�
        �¶ȴ�����2ָ���ƹ��¶�
*********************************************************************/

typedef unsigned int nextai_uint32;
typedef signed int nextai_int32;
typedef unsigned short nextai_uint16;
typedef signed short nextai_int16;
typedef unsigned char nextai_uint8;
typedef signed char nextai_int8;
typedef char nextai_char;

/*********************************************************************
����ֵ����
*********************************************************************/
#define NEXTAI_NULL (0)
#define NEXTAI_FAIL (-1)
#define NEXTAI_SYSTEM_BUSYING (-2)
#define NEXTAI_MEMORY_LACK (-3)

#define NEXTAI_TRUE (1)
#define NEXTAI_FALSE (0)

#define NEXTAI_CURRENT_VERSION_NUM "0.1.4"

enum {
    NEXTAI_EVENT_TYPE_TIMER = 0,                    //ʱ���¼���
    NEXTAI_EVENT_TYPE_KEY,                          //�����¼���
    NEXTAI_EVENT_TYPE_SENSOR,                       //�������¼���
    NEXTAI_EVENT_TYPE_COM,                          //com�¼���
    NEXTAI_EVENT_TYPE_MAX                           //����¼��࣬����Ҫ����µ��¼����ͣ����ڴ�����֮ǰ
};


enum {
    NEXTAI_TIMER_EVENT_TIMER = 0,
    NEXTAI_TIMER_EVENT_COM_RECV
};


/****************************************************************
*�Ӵ��������¼�Ϊ�����¼�
****************************************************************/
enum {
    NEXTAI_KEY_EVENT_SHUTDOWN = 0x10,   //��ͣ
    NEXTAI_KEY_EVENT_RUN,               //����
    NEXTAI_KEY_EVENT_STOP,              //ֹͣ
    NEXTAI_KEY_EVENT_VALUE,             //����
    NEXTAI_KEY_EVENT_FIX,               //����
    NEXTAI_KEY_EVENT_STUDY,             //��ѧϰ
    NEXTAI_KEY_EVENT_UP,                //����
    NEXTAI_KEY_EVENT_DOWN,              //����
    NEXTAI_KEY_EVENT_HOTSWITCH,         //���ȽӴ���
    NEXTAI_KEY_EVENT_BRAKESWITCH,       //�ƶ��Ӵ���
    NEXTAI_KEY_EVENT_MOTORFAN,          //�����ȽӴ���
    NEXTAI_KEY_EVENT_BOXFAN1,           //��ع���ȽӴ���1
    NEXTAI_KEY_EVENT_BOXFAN2,           //��ع���ȽӴ���1
    NEXTAI_KEY_EVENT_RUNSWITCH          //���нӴ���
};


enum {
    NEXTAI_SENSOR_EVENT_SAFECIRCUIT = 0x20,     //��ȫ�ػ�����
    NEXTAI_SENSOR_EVENT_CONTACTOR1,             //�Ӵ���1����
    NEXTAI_SENSOR_EVENT_CONTACTOR2,             //�Ӵ���2����
    NEXTAI_SENSOR_EVENT_CONTACTOR3,             //�Ӵ���3����
    NEXTAI_SENSOR_EVENT_UPCHANGE,               //�ϱ���
    NEXTAI_SENSOR_EVENT_DOWNCHANGE,             //�±���
    NEXTAI_SENSOR_EVENT_TOPLINE,                //����λ
    NEXTAI_SENSOR_EVENT_UNDERLINE,              //����λ
    NEXTAI_SENSOR_EVENT_INVERTERRUN,              //��Ƶ����������
    NEXTAI_SENSOR_EVENT_INVERTERERROR,            //��Ƶ������
    NEXTAI_SENSOR_EVENT_RESERVE1                //Ԥ���¼�
};

enum {
    NEXTAI_CTRL_CTRL_MODULE,                    //�ܿ�ģ��
    NEXTAI_CTRL_DATA_MODULE,                    //���ݴ���ģ��
    NEXTAI_CTRL_LOGIC_MODULE,                   //�߼�����ģ��
    NEXTAI_CTRL_MAPPING_MODULE,                 //��תģ��
    NEXTAI_CTRL_SERVER_MODULE                    //��UI������Ӧ��ģ��
};


#define FLAG_TO_APP (2)
#define FLAG_TO_LOAD (1)




#endif



