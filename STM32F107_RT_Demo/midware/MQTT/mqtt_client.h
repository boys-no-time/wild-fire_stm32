#ifndef _MQTT_CLIENT__H_
#define _MQTT_CLIENT__H_

#include <rtthread.h>
#include <lwip/netdb.h>
#include <lwip/sockets.h>

// #define SAVE_FIRST_IP

#define RECV_MAX_BYTE	1024		//接收缓冲最大字节数
#define SEND_MAX_BYTE	1024		//发送缓冲最大字节数
#define CLIENT_MQ_MAX   512 

#define RT_APPS_THREAD_TICKS	10
#define RT_SERVE_THREAD_TICKS	15

#ifdef TEST_VERSION
#define HEART_TIME	5
#else
#define HEART_TIME	15 //心跳包间隔时间，单位：秒
#endif
#define HEART_TIME_OUT_MAX_COUNT	3 //心跳超时最多次数

typedef struct _MQTT_CLIENT_TYPEDEF_ MQTT_CLIENT_TYPEDEF;

typedef struct _SERVERCMDTYPEDEF_ {
    rt_uint8_t cmdtype;
    rt_uint8_t num;
    rt_uint8_t check;
    rt_uint8_t led;
    rt_uint8_t lock;
    rt_uint32_t cmdid;
} SERVERCMDTYPEDEF;

/* do NOT expose data globally!!! */
//extern MQTT_CLIENT_TYPEDEF MQTT_client;

#define PHY_ADDRESS	0 //物理网络地址

enum {
	MQTT_CLIENT_STATUS_NONE,
	MQTT_CLIENT_STATUS_PHY_LINKUP,		//网卡上线
	MQTT_CLIENT_STATUS_CONNECT,			//MQTT连接
	MQTT_CLIENT_STATUS_SUBSCRIBE,		//MQTT订阅
	MQTT_CLIENT_STATUS_PUBLISH,			//MQTT发布
	MQTT_CLIENT_STATUS_ALLDONE,			//流程完成了
	MQTT_CLIENT_STATUS_UNKNOWN
};

//event definitions(bitwise)
#define MQTT_EVENT_NET_CHANGED	0x0001	//网卡状态改变
#define MQTT_EVENT_NET_ONLINE	0x0002	//网卡上线
#define MQTT_EVENT_NET_OFFLINE	0x0004	//网卡下线
#define MQTT_EVENT_DO_PUBLISH	0x0008	//通知MQTT PUBLISH

#ifdef __cplusplus
extern "C" {
#endif

void mqtt_client_create(void);

void MQTT_Publish_From_Server(unsigned char *recv_buf, unsigned int recv_len);

void MQTT_Send2Clientmq(char *buffer, rt_size_t size);

int MQTT_isConnected(void);

rt_mq_t mqtt_get_mq(void);

rt_event_t mqtt_get_ev(void);

//rt_uint8_t mqtt_get_clientstatus(void);

//void mqtt_set_clientstatus(rt_uint8_t cs);

#ifdef __cplusplus
}
#endif

#endif

