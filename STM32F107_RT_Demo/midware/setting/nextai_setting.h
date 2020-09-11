/*********************************************************************
Copyright (c) 2014 by Automation Interactive.
All rights reserved. You are not allowed to
copy or distribute the code without permission.
*********************************************************************/

#ifndef _NEXTAI_SETTING__H_
#define _NEXTAI_SETTING__H_

#include "nextai_defined.h"
#include <rtthread.h>
#include <lwip/netdb.h>
#include <lwip/sockets.h>

#define SW_VERSION "1.0.0"

/*********************************************************************
settingģ�����Ե�ַ
*********************************************************************/

#define SETTING_ADDR_DATAPOST_INTERVAL (1)
#define SETTING_ADDR_USART_BAUDRATE (2)                //����1������
#define SETTING_ADDR_USART_BYTESIZE (3)
#define SETTING_ADDR_USART_PARITY (4)
#define SETTING_ADDR_USART_STOPBITS (5)
#define SETTING_ADDR_DEVICE_NUM (6)                    //�豸��ţ�����Ϊ1
#define SETTING_ADDR_SETTING_CARD (7)
#define SETTING_ADDR_CURRENT_CARD (8)
#define SETTING_ADDR_SERIAL_NUM (9)                    //���к�
//#define SETTING_ADDR_WARNING_SEND_NUMBER (10)          //���淢�ʹ���
#define SETTING_ADDR_MQTT_IPADDR (11) //����
#define SETTING_ADDR_MQTT_IPPORT (12)
#define SETTING_ADDR_MQTT_TOPICNAME (13)
#define SETTING_ADDR_MQTT_CLIENTID (14)
#define SETTING_ADDR_MQTT_USERNAME (15)
#define SETTING_ADDR_MQTT_PASSWORD (16)
#define SETTING_ADDR_MQTT_IPADDR_2 (17) //IP

enum {
    NEXTAI_COM_CBR_110,
    NEXTAI_COM_CBR_300,
    NEXTAI_COM_CBR_600,
    NEXTAI_COM_CBR_1200,
    NEXTAI_COM_CBR_2400,
    NEXTAI_COM_CBR_4800,
    NEXTAI_COM_CBR_9600,
    NEXTAI_COM_CBR_19200,
    NEXTAI_COM_CBR_38400,
    NEXTAI_COM_CBR_56000,
    NEXTAI_COM_CBR_57600,
    NEXTAI_COM_CBR_115200,
    NEXTAI_COM_CBR_128000,
    NEXTAI_COM_CBR_256000
};



#define INODES_DATATYPE_YC (0)
#define INODES_DATATYPE_YX (1)
#define INODES_DATATYPE_YT (2)



typedef struct _nextai_reqdata_info_ {
    rt_uint8_t clientnum;       //�ն�num��
    unsigned short reqaddr;        //�����ַ
    rt_uint8_t reqlen;          //���󳤶�
    rt_uint8_t reqtype;         //�������ͣ�ң��orң��
} nextai_reqdata_info;

//ȫ�����ð��մ�˱������ڴ��У������ȡ
typedef struct _nextai_setting_data_ {
    nextai_uint16 interval;                     //���¼��ʱ��
    //20170913���
    nextai_char localaddr[17];					//����IP��ַ
    
    nextai_char gateway[17];					//��������
    nextai_char mask[17];						//������������
    nextai_char dns[17];						//DNS

    //20170809���
    nextai_char remoteaddr[50];					//������IP��ַ
    nextai_char remoteIP[17];					//Զ��IP��ַ
    nextai_uint16 remoteport;					//�������˿�
    nextai_char mqtt_clientid[32];				//MQTT�ͻ���ID
    nextai_char mqtt_username[50];				//MQTT�û���
    nextai_char mqtt_password[50];				//MQTT����
} nextai_setting_data;

#define SET_REVC_MAX 512

#define SET_SEND_MAX 512

typedef struct _SETTING_UDP_TYPEDEF_ {
    //������SOCK�йصĲ���
    int sock;
    rt_uint32_t addr_len;
    struct sockaddr_in server_addr, client_addr;
    //�����Ƿ�������յĻ��嶨��
    rt_int32_t bytes_revc;
    rt_uint8_t revc[SET_REVC_MAX];				//���յ���ԭʼ����
    rt_uint32_t bytes_send;
    rt_uint8_t send[SET_SEND_MAX];				//��������
    rt_uint8_t dnt_id[128];

    rt_int32_t timeout;										//��ʱʱ��
    rt_thread_t udp;
} SETTING_UDP_TYPEDEF;

typedef struct _INODES_DEFAULT_INFO_ {
    rt_uint8_t software_vers[16];
    rt_uint8_t product_model[24];
    rt_uint8_t server_prodid[32];
    rt_uint8_t master_apikey[32];
} INODES_DEFAULT_INFO;

extern SETTING_UDP_TYPEDEF setthreah;

extern nextai_setting_data setting_para_temp;

#ifdef __cplusplus
extern "C"
{
#endif

nextai_int8 nextai_setting_create(void);

void nextai_setting_destroy(void);

void nextai_setting_default(void *handle);

nextai_int8 nextai_setting_save(void);

nextai_int8 nextai_setting_getproperty(nextai_int16 setting_addr, void *value);

nextai_int8 nextai_setting_setproperty(nextai_int16 setting_addr, void *value);

nextai_int8 nextai_setting_parse(rt_uint8_t *strbuf, rt_uint8_t *dnt_id);

nextai_uint8 nextai_setting_getaddrdata(nextai_uint16 client_num, nextai_uint16 addr, nextai_uint16 len, unsigned int *cur_datalen);

nextai_int8 nextai_setting_getlist(nextai_int16 indx, nextai_reqdata_info *data_info);

rt_uint8_t nextai_setting_getaddrnum(rt_uint8_t client_num, rt_uint8_t *databuf_num);

void nextai_get_local_ip(rt_uint8_t *strIP);

#ifdef __cplusplus
}
#endif

#endif
