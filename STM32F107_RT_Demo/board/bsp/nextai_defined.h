/*********************************************************************
Copyright (c) 2014 by Automation Interactive.
All rights reserved. You are not allowed to
copy or distribute the code without permission.
*********************************************************************/

#ifndef _NEXTAI_DEFINED__H_
#define _NEXTAI_DEFINED__H_


/*********************************************************************
系统中：温度传感器1指电机温度；
        温度传感器2指控制柜温度
*********************************************************************/

typedef unsigned int nextai_uint32;
typedef signed int nextai_int32;
typedef unsigned short nextai_uint16;
typedef signed short nextai_int16;
typedef unsigned char nextai_uint8;
typedef signed char nextai_int8;
typedef char nextai_char;

/*********************************************************************
返回值定义
*********************************************************************/
#define NEXTAI_NULL (0)
#define NEXTAI_FAIL (-1)
#define NEXTAI_SYSTEM_BUSYING (-2)
#define NEXTAI_MEMORY_LACK (-3)

#define NEXTAI_TRUE (1)
#define NEXTAI_FALSE (0)

#define NEXTAI_CURRENT_VERSION_NUM "0.1.4"

enum {
    NEXTAI_EVENT_TYPE_TIMER = 0,                    //时间事件类
    NEXTAI_EVENT_TYPE_KEY,                          //按键事件类
    NEXTAI_EVENT_TYPE_SENSOR,                       //传感器事件类
    NEXTAI_EVENT_TYPE_COM,                          //com事件类
    NEXTAI_EVENT_TYPE_MAX                           //最大事件类，若需要添加新的事件类型，需在此类型之前
};


enum {
    NEXTAI_TIMER_EVENT_TIMER = 0,
    NEXTAI_TIMER_EVENT_COM_RECV
};


/****************************************************************
*接触器控制事件为网络事件
****************************************************************/
enum {
    NEXTAI_KEY_EVENT_SHUTDOWN = 0x10,   //急停
    NEXTAI_KEY_EVENT_RUN,               //运行
    NEXTAI_KEY_EVENT_STOP,              //停止
    NEXTAI_KEY_EVENT_VALUE,             //正常
    NEXTAI_KEY_EVENT_FIX,               //检修
    NEXTAI_KEY_EVENT_STUDY,             //自学习
    NEXTAI_KEY_EVENT_UP,                //上行
    NEXTAI_KEY_EVENT_DOWN,              //下行
    NEXTAI_KEY_EVENT_HOTSWITCH,         //加热接触器
    NEXTAI_KEY_EVENT_BRAKESWITCH,       //制动接触器
    NEXTAI_KEY_EVENT_MOTORFAN,          //马达风扇接触器
    NEXTAI_KEY_EVENT_BOXFAN1,           //电控柜风扇接触器1
    NEXTAI_KEY_EVENT_BOXFAN2,           //电控柜风扇接触器1
    NEXTAI_KEY_EVENT_RUNSWITCH          //运行接触器
};


enum {
    NEXTAI_SENSOR_EVENT_SAFECIRCUIT = 0x20,     //安全回环故障
    NEXTAI_SENSOR_EVENT_CONTACTOR1,             //接触器1故障
    NEXTAI_SENSOR_EVENT_CONTACTOR2,             //接触器2故障
    NEXTAI_SENSOR_EVENT_CONTACTOR3,             //接触器3故障
    NEXTAI_SENSOR_EVENT_UPCHANGE,               //上变向
    NEXTAI_SENSOR_EVENT_DOWNCHANGE,             //下变向
    NEXTAI_SENSOR_EVENT_TOPLINE,                //上限位
    NEXTAI_SENSOR_EVENT_UNDERLINE,              //下限位
    NEXTAI_SENSOR_EVENT_INVERTERRUN,              //变频器正常运行
    NEXTAI_SENSOR_EVENT_INVERTERERROR,            //变频器故障
    NEXTAI_SENSOR_EVENT_RESERVE1                //预留事件
};

enum {
    NEXTAI_CTRL_CTRL_MODULE,                    //总控模块
    NEXTAI_CTRL_DATA_MODULE,                    //数据处理模块
    NEXTAI_CTRL_LOGIC_MODULE,                   //逻辑处理模块
    NEXTAI_CTRL_MAPPING_MODULE,                 //中转模块
    NEXTAI_CTRL_SERVER_MODULE                    //和UI交互的应用模块
};


#define FLAG_TO_APP (2)
#define FLAG_TO_LOAD (1)




#endif



