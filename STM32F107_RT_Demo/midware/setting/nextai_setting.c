/*********************************************************************
Copyright (c) 2014 by Automation Interactive.
All rights reserved. You are not allowed to
copy or distribute the code without permission.
*********************************************************************/
#include "inodes_include.h"


#define NEXTAI_REQDATA_COUNT (32)
#define NEXTAI_ERROR_DATA_LEN (12)
#define NEXTAI_SETTING_HEAD_LEN (8)
#define NEXTAI_SETTING_BUF_LEN (512)

#define NEXTAI_SETTING_TMPBUF_LEN (512)

#define NEXTAI_SETTING_MAPPING_LEN (3)

SETTING_UDP_TYPEDEF setthreah;

nextai_setting_data setting_para_temp;
//配置空间，长度0x100
nextai_uint8 NEXTAI_SETTING_STATIC_BUF[NEXTAI_SETTING_BUF_LEN]; //__attribute__((at(0x20004088)));

void settingserv(void *);

/*********************************************************************
静态函数声明
*********************************************************************/
//static nextai_int8 nextai_setting_parsedata(void *handle, cJSON *json_p);
static nextai_uint8 nextai_setting_load(void *handle);
//static void nextai_setting_regclientnum(void *handle, rt_uint8_t client_num);
/*********************************************************************
*保存数据都以16bit对齐
---
*flash空间的存储与内存中保持一致性
*********************************************************************/

nextai_int8 nextai_setting_create(void)
{
    nextai_uint8 ret = 0xff;
    nextai_setting_data *me = 0;

    me = (nextai_setting_data *)&setting_para_temp;

    rt_memset(NEXTAI_SETTING_STATIC_BUF, 0, NEXTAI_SETTING_BUF_LEN);

    rt_kprintf("Begin to load setting!\r\n");
    ret = nextai_setting_load(me);
    if (0xff == ret) {
        rt_kprintf("No Setting in flash!\r\n");
        nextai_setting_default(me);
        nextai_setting_save();
    }

    setthreah.udp = rt_thread_create("setthread",  settingserv, RT_NULL,
                                     512, RT_APPS_PRIORITY, 21);
    if (setthreah.udp != RT_NULL) {
        rt_thread_startup(setthreah.udp);
    }

    return 0;
}

void nextai_setting_destroy(void)
{
    rt_memset(NEXTAI_SETTING_STATIC_BUF, 0, NEXTAI_SETTING_BUF_LEN * sizeof(nextai_uint16));
}

static nextai_uint8 nextai_setting_load(void *handle)
{
    UNUSED(handle);

    nextai_int32 ret = -1, datalen = 0;
    rt_uint8_t get_id_buf[100] = {0};
    //nextai_setting_data *me = (nextai_setting_data *)handle;
    rt_uint8_t *inodes_sys_tmpbuf = NEXTAI_SETTING_STATIC_BUF;//inodes_get_systmpbuf();

    rt_memset(inodes_sys_tmpbuf, 0, NEXTAI_SETTING_TMPBUF_LEN);
    ret = nextai_flash_read(NEXTAI_SETTING_WRITE_ADDR, (nextai_uint16 *)&datalen, 2);
    if (ret != 2) {
        return 0xff;
    }

    rt_kprintf("The datalen is %d!\r\n", datalen);

    if ((datalen >= NEXTAI_SETTING_TMPBUF_LEN) || (datalen < 4)) {
        return 0xff;
    }

    rt_memset(inodes_sys_tmpbuf, 0, NEXTAI_SETTING_TMPBUF_LEN);
    ret = nextai_flash_read(NEXTAI_SETTING_WRITE_ADDR + 4, (nextai_uint16 *)inodes_sys_tmpbuf, (datalen + 1) >> 1);
    if (ret != ((datalen + 1) >> 1)) {
        return 0xff;
    }

    ret = nextai_setting_parse((nextai_uint8 *)inodes_sys_tmpbuf, get_id_buf);
    if (ret < 0) {
        return 0xff;
    }

    rt_kprintf("%s\r\n", (char *)inodes_sys_tmpbuf);

    return 0;
}

/*********************************************************************
{"fc":"03","data":[{"type":"yc","addr":"1244","length":"45"},
                   {"type":"yc","addr":"1344","length":"45"},
                   {"type":"yx","addr":"1014","length":"3"},
                   {"type":"yx","addr":"1024","length":"3"}]}
{"type":"ds","itrval":"60","bdrate":"9600","bsize":"8",
"parity":"0","stopbs":"2"}
*********************************************************************/
nextai_int8 nextai_setting_save(void)
{
    //nextai_int8 i = 0;
    unsigned int datalen = 0;
    nextai_setting_data *me = (nextai_setting_data *)&setting_para_temp;
    rt_uint8_t *inodes_sys_tmpbuf = NEXTAI_SETTING_STATIC_BUF;//inodes_get_systmpbuf();

    rt_memset(inodes_sys_tmpbuf, 0, NEXTAI_SETTING_BUF_LEN);
    strncpy((char *)inodes_sys_tmpbuf, "head", 4);

    sprintf((char *)&inodes_sys_tmpbuf[4], "{\"interval\":\"%d\",", me->interval);
    datalen = strlen((const char *)inodes_sys_tmpbuf);

    sprintf((char *)&inodes_sys_tmpbuf[datalen], "\"remoteaddr\":\"%s\",\"remoteport\":\"%d\",\"remoteIP\":\"%s\",", me->remoteaddr, me->remoteport, me->remoteIP);
    datalen = strlen((const char *)inodes_sys_tmpbuf);

    sprintf((char *)&inodes_sys_tmpbuf[datalen], "\"localaddr\":\"%s\",", me->localaddr);
    datalen = strlen((const char *)inodes_sys_tmpbuf);

    sprintf((char *)&inodes_sys_tmpbuf[datalen], "\"gateway\":\"%s\",\"mask\":\"%s\",", me->gateway, me->mask);
    datalen = strlen((const char *)inodes_sys_tmpbuf);

    sprintf((char *)&inodes_sys_tmpbuf[datalen], "\"dns\":\"%s\",", me->dns);
    datalen = strlen((const char *)inodes_sys_tmpbuf);

    sprintf((char *)&inodes_sys_tmpbuf[datalen], "\"clientid\":\"%s\",", me->mqtt_clientid);
    datalen = strlen((const char *)inodes_sys_tmpbuf);

    sprintf((char *)&inodes_sys_tmpbuf[datalen], "\"username\":\"%s\",\"password\":\"%s\"", me->mqtt_username, me->mqtt_password);
    datalen = strlen((const char *)inodes_sys_tmpbuf);

    sprintf((char *)&inodes_sys_tmpbuf[datalen], "}");

    datalen = strlen((const char *)inodes_sys_tmpbuf) - 4;
    memcpy(inodes_sys_tmpbuf, (const char *)&datalen, 4);
    {
        unsigned short writelen = (unsigned short)((datalen + 4 + 1) & 0xffff);
        nextai_flash_write(NEXTAI_SETTING_WRITE_ADDR, (nextai_uint16 *)inodes_sys_tmpbuf, writelen >> 1);
    }

    return 0;
}

#define LOCAL_CONF	0//1
#define REMOTE_CONF	1//0
void nextai_setting_default(void *handle)
{
    nextai_setting_data *me = (nextai_setting_data *)handle;
    //int i = 0;
    rt_uint32_t CpuID[3];
    CpuID[0] = *(rt_uint32_t *)(0x1ffff7e8);
    CpuID[1] = *(rt_uint32_t *)(0x1ffff7ec);
    CpuID[2] = *(rt_uint32_t *)(0x1ffff7f0);
    CpuID[0] = CpuID[0] & 0xfffff;
    CpuID[1] = CpuID[1] & 0xfffff;
    CpuID[2] = CpuID[2] & 0xfffff;
    sprintf((char *)me->mqtt_clientid, "%08x%08x%08x", CpuID[0], CpuID[1], CpuID[2]);

    me->interval = 60 * 5;
	
#if (LOCAL_CONF == 0)
    strcpy(me->localaddr, "192.168.3.168");
    strcpy(me->gateway, "192.168.3.1");
#elif (LOCAL_CONF == 1)
    strcpy(me->localaddr, "10.120.81.111");
    strcpy(me->gateway, "10.120.81.1");
#endif
    strcpy(me->mask, "255.255.255.0");
    strcpy(me->dns, me->gateway);

#if (REMOTE_CONF == 0)
    strcpy(me->remoteaddr, "emqtt.dooioo.com");
	//strcpy(me->remoteIP, "");
    strcpy(me->mqtt_username, "keybox");
    strcpy(me->mqtt_password, "keyboxkz123");
    me->remoteport = 21883;
#elif (REMOTE_CONF == 1)
	strcpy(me->remoteaddr, "101.231.177.3");
	strcpy(me->remoteIP, "101.231.177.3");
    strcpy(me->mqtt_username, "inodes");
    strcpy(me->mqtt_password, "123456");
    me->remoteport = 10883;
#elif (REMOTE_CONF == 2)
	strcpy(me->remoteaddr, "120.24.69.121");
	strcpy(me->remoteIP, "120.24.69.121");
	strcpy(me->mqtt_username, "keybox");
	strcpy(me->mqtt_password, "keyboxkz123");
	me->remoteport = 1883;
#endif
}

/*********************************************************************
*此函数须在初始化完成后，才能调用。切记切记。
*********************************************************************/
nextai_int8 nextai_setting_getproperty(nextai_int16 setting_addr, void *value)
{
    nextai_setting_data *me = (nextai_setting_data *)&setting_para_temp;

    switch (setting_addr) {
    case SETTING_ADDR_DATAPOST_INTERVAL:
        *(unsigned short *)value = me->interval;
        break;
    case SETTING_ADDR_USART_BAUDRATE:                 //串口波特率
//          *(unsigned int *)value = me->usart_baudrate;
        break;
    case SETTING_ADDR_USART_BYTESIZE:
        //*(unsigned short *)value = me->bytesize;
        break;
    case SETTING_ADDR_USART_PARITY:
        //*(unsigned short *)value = me->parity;
        break;
    case SETTING_ADDR_USART_STOPBITS:
        //*(unsigned short *)value = me->stopbits;
        break;
    case SETTING_ADDR_SERIAL_NUM:
        //strcpy((char *)value, (const char *)me->cur_serial_num);
        *(nextai_uint32 *)value = (nextai_uint32)me->mqtt_clientid;
        break;
    case SETTING_ADDR_MQTT_IPADDR:
        *(nextai_uint32 *)value = (nextai_uint32)me->remoteaddr;
        break;
    case SETTING_ADDR_MQTT_IPPORT:
        *(unsigned short *)value = me->remoteport;
        break;
//     case SETTING_ADDR_MQTT_TOPICNAME:
//          strcpy((char *)value, (const char *)me->mqtt_topicname);
//          break;
//     case SETTING_ADDR_MQTT_CLIENTID:
//          strcpy((char *)value, (const char *)me->mqtt_clientid);
//          break;
    case SETTING_ADDR_MQTT_USERNAME:
        *(nextai_uint32 *)value = (nextai_uint32)me->mqtt_username;
        break;
    case SETTING_ADDR_MQTT_PASSWORD:
        *(nextai_uint32 *)value = (nextai_uint32)me->mqtt_password;
        break;
    case SETTING_ADDR_MQTT_IPADDR_2:
        *(nextai_uint32 *)value = (nextai_uint32)me->remoteIP;
        break;
    default:
        break;
    }

    return 0;
}

/*********************************************************************
*此函数须在初始化完成后，才能调用。切记切记。
     nextai_uint32 usart_baudrate;                    //串口波特率
     nextai_uint8 interval;
     nextai_uint8 bytesize;
     nextai_uint8 parity;
     nextai_uint8 stopbits;
*********************************************************************/
nextai_int8 nextai_setting_setproperty(nextai_int16 setting_addr, void *value)
{
    nextai_setting_data *me = (nextai_setting_data *)&setting_para_temp;

    switch (setting_addr) {
    case SETTING_ADDR_DATAPOST_INTERVAL:
        me->interval = *(unsigned short *)value;
        break;
    case SETTING_ADDR_USART_BAUDRATE:                //串口波特率
//          me->usart_baudrate = *(unsigned int *)value;
        break;
    case SETTING_ADDR_USART_BYTESIZE:                //字长
//          me->bytesize = *(unsigned short *)value;
        break;
    case SETTING_ADDR_USART_PARITY:                  //奇偶
        //me->parity = *(unsigned short *)value;
        break;
    case SETTING_ADDR_USART_STOPBITS:                //停止位
        //me->stopbits = *(unsigned short *)value;
        break;
//     case SETTING_ADDR_SERIAL_NUM:
//          strcpy((char *)me->mqtt_clientid, (const char *)value);
//          return 0;
    default:
        break;
    }

    return 0;
}

/*********************************************************************
{"fc":"03","data":[{"type":"yc","addr":"1244","length":"45"},
                   {"type":"yc","addr":"1344","length":"45"},
                   {"type":"yx","addr":"1014","length":"3"},
                   {"type":"yx","addr":"1024","length":"3"}]}
{"type":"ds","itrval":"60","bdrate":"9600","bsize":"8",
"parity":"0","stopbs":"2"}

     nextai_uint8 interval;
     nextai_uint32 usart_baudrate;                    //串口波特率
     nextai_uint8 bytesize;
     nextai_uint8 parity;
     nextai_uint8 stopbits;
*********************************************************************/
nextai_int8 nextai_setting_parse(rt_uint8_t *strbuf, rt_uint8_t *dnt_id)
{
    nextai_setting_data *me = (nextai_setting_data *)&setting_para_temp;
    cJSON *root_p = 0, *cur_p = 0;
//     int ret = 0;

    if ((0 == strbuf) || (0 == me)) {
        return -1;
    }

    rt_kprintf("%s\r\n", strbuf);

    root_p = cJSON_Parse((const char *)strbuf);
    if (0 == root_p) {
        return -1;
    }

    cur_p = root_p->child;
    while (cur_p) {
        if (strncmp(cur_p->string, "interval", 6) == 0) {
            if (cur_p->valuestring) {
                me->interval = strtol((const char *)cur_p->valuestring, 0, 10);
                strcat((char *)dnt_id, (const char *)cur_p->string);
            }
        }
        else if (strncmp(cur_p->string, "remoteaddr", 10) == 0) {
            if (cur_p->valuestring) {
                strcpy((char *)me->remoteaddr, (const char *)cur_p->valuestring);
                strcat((char *)dnt_id, (const char *)cur_p->string);
            }
        }
        else if (strncmp(cur_p->string, "remoteport", 10) == 0) {
            if (cur_p->valuestring) {
                me->remoteport = strtol((const char *)cur_p->valuestring, 0, 10);
                strcat((char *)dnt_id, (const char *)cur_p->string);
            }
        }
        else if (strncmp(cur_p->string, "remoteIP", 8) == 0) {
            if (cur_p->valuestring) {
                strcpy((char *)me->remoteIP, (const char *)cur_p->valuestring);
                strcat((char *)dnt_id, (const char *)cur_p->string);
            }
        }
        else if (strncmp(cur_p->string, "localaddr", 9) == 0) {
            if (cur_p->valuestring) {
                strcpy((char *)me->localaddr, (const char *)cur_p->valuestring);
                strcat((char *)dnt_id, (const char *)cur_p->string);
            }
        }
        else if (strncmp(cur_p->string, "gateway", 7) == 0) {
            if (cur_p->valuestring) {
                strcpy((char *)me->gateway, (const char *)cur_p->valuestring);
                strcat((char *)dnt_id, (const char *)cur_p->string);
            }
        }
        else if (strncmp(cur_p->string, "mask", 4) == 0) {
            if (cur_p->valuestring) {
                strcpy((char *)me->mask, (const char *)cur_p->valuestring);
                strcat((char *)dnt_id, (const char *)cur_p->string);
            }
        }
        else if (strncmp(cur_p->string, "dns", 3) == 0) {
            if (cur_p->valuestring) {
                strcpy((char *)me->dns, (const char *)cur_p->valuestring);
                strcat((char *)dnt_id, (const char *)cur_p->string);
            }
        }
        else if (strncmp(cur_p->string, "clientid", 8) == 0) {
            unsigned char deviceid[32] = {0};
            if (cur_p->valuestring) {
                rt_uint32_t CpuID[3];
                CpuID[0] = *(rt_uint32_t *)(0x1ffff7e8);
                CpuID[1] = *(rt_uint32_t *)(0x1ffff7ec);
                CpuID[2] = *(rt_uint32_t *)(0x1ffff7f0);
                CpuID[0] = CpuID[0] & 0xfffff;
                CpuID[1] = CpuID[1] & 0xfffff;
                CpuID[2] = CpuID[2] & 0xfffff;

                sprintf((char *)deviceid, "%08x%08x%08x",  CpuID[0], CpuID[1], CpuID[2]);

                if (strncmp((const char *)deviceid, (const char *)cur_p->valuestring, 30) == 0) {
                    strcpy((char *)me->mqtt_clientid, (const char *)cur_p->valuestring);
                    strcat((char *)dnt_id, (const char *)cur_p->string);
                }
                else {
                    return -1;
                }
            }
        }
        else if (strncmp(cur_p->string, "username", 8) == 0) {
            if (cur_p->valuestring) {
                strcpy((char *)me->mqtt_username, (const char *)cur_p->valuestring);
                strcat((char *)dnt_id, (const char *)cur_p->string);
            }
        }
        else if (strncmp(cur_p->string, "password", 8) == 0) {
            if (cur_p->valuestring) {
                strcpy((char *)me->mqtt_password, (const char *)cur_p->valuestring);
                strcat((char *)dnt_id, (const char *)cur_p->string);
            }
        }
        cur_p = cur_p->next;
    }

    cJSON_Delete(root_p);

    return 0;
}


//static nextai_int8 nextai_setting_parsedata(void *handle, cJSON *json_p)
//{
//     nextai_setting_data *me = (nextai_setting_data *)handle;
//     cJSON *root_p = json_p, *node_p = 0, *cur_p = 0;
//     unsigned int data_indx = 0, loop_i = 0;
//     nextai_reqdata_info tmp_reqdata = {0};

//     cur_p = root_p->child;

//     while(cur_p)
//     {
//          node_p = cur_p;
//
//          cur_p = node_p->child;
//          memset(&tmp_reqdata, 0, sizeof(nextai_reqdata_info));
//          while(cur_p)
//          {
//               if(strncmp(cur_p->string, "type", 4) == 0)
//               {
//                    if(cur_p->valuestring)
//                    {
//                         if(strncmp("yt", cur_p->valuestring, 2) == 0)
//                         {
//                              break;
//                         }
//                         else if(strncmp("yc", cur_p->valuestring, 2) == 0)
//                         {
//                              tmp_reqdata.reqtype = INODES_DATATYPE_YC;
//                         }
//                         else if(strncmp("yx", cur_p->valuestring, 2) == 0)
//                         {
//                              tmp_reqdata.reqtype = INODES_DATATYPE_YX;
//                         }
//                         else
//                              break;
//                    }
//               }
//               else if(strncmp(cur_p->string, "client", 6) == 0)
//               {
//                    if(cur_p->valuestring)
//                         tmp_reqdata.clientnum = strtol((const char *)cur_p->valuestring, 0, 10);
//               }
//               else if(strncmp(cur_p->string, "addr", 4) == 0)
//               {
//                    if(cur_p->valuestring)
//                         tmp_reqdata.reqaddr = strtol((const char *)cur_p->valuestring, 0, 10);
//               }
//               else if(strncmp(cur_p->string, "length", 6) == 0)
//               {
//                    if(cur_p->valuestring)
//                         tmp_reqdata.reqlen = strtol((const char *)cur_p->valuestring, 0, 10);
//               }
//
//               cur_p = cur_p->next;
//          }
//
//          cur_p = node_p->next;
//     }

//     return 0;
//}



void settingserv(void *parameter)
{
    UNUSED(parameter);

    //挂起自己，等待LWIP初始化完成
    //   rt_thread_suspend(rt_thread_self());
    //   rt_schedule();
    while (1) {
        rt_thread_delay(300);
        if ((setthreah.sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
            continue;
        }

        setthreah.server_addr.sin_family = AF_INET;
        setthreah.server_addr.sin_port = htons(502);
        setthreah.server_addr.sin_addr.s_addr = INADDR_ANY;
        rt_memset(&(setthreah.server_addr.sin_zero), 0, sizeof(setthreah.server_addr.sin_zero));

        if (bind(setthreah.sock, (struct sockaddr *)&setthreah.server_addr,
                 sizeof(struct sockaddr)) == -1) {
            continue;
        }

        setthreah.addr_len = sizeof(struct sockaddr);

        while (1) {
            rt_kprintf("loop\r\n");
            rt_memset(setthreah.revc, 0, SET_REVC_MAX);
            setthreah.bytes_revc = recvfrom(setthreah.sock, setthreah.revc, SET_REVC_MAX, 0, (struct sockaddr *)&setthreah.client_addr, &setthreah.addr_len);
            if ((setthreah.bytes_revc > 0) && (setthreah.bytes_revc != -1)) {
                rt_kprintf("[recv]The datalen is %d!\r\n", setthreah.bytes_revc);
                rt_memset(setthreah.dnt_id, 0, 128);
                if (nextai_setting_parse(setthreah.revc, setthreah.dnt_id) == 0) {
                    rt_kprintf("setting:%s\r\n", setthreah.revc);
                    nextai_setting_save();
                    strcat((char *)setthreah.dnt_id, " OK\r\n");
                    sendto(setthreah.sock, setthreah.dnt_id, strlen((char *)setthreah.dnt_id), 0,
                           (struct sockaddr *)&setthreah.client_addr, setthreah.addr_len);
                }
                else if (strncmp("*GET999999STA#", (const char *)setthreah.revc, 14) == 0) {
                    rt_uint16_t setting_data_len = 0;
                    rt_uint16_t ack_hear_lengh;
                    //rt_uint8_t cur_version[32] = {0};
                    rt_uint16_t ret;

                    rt_kprintf("get setting..\r\n");
                    rt_memset(setthreah.send, 0, SET_SEND_MAX);
                    ret = nextai_flash_read(NEXTAI_SETTING_WRITE_ADDR, (nextai_uint16 *)&setting_data_len, 2);
                    if (ret == 2) {
                        if ((setting_data_len < SET_SEND_MAX) && (setting_data_len >= 4)) {
                            sprintf((char *)setthreah.send, "*GET999999STA#%d#{vers:%s}", 7 + setting_data_len + strlen((char *)SW_VERSION), SW_VERSION);
                            ack_hear_lengh = strlen((char *)setthreah.send);
                            ret = nextai_flash_read(NEXTAI_SETTING_WRITE_ADDR + 4, (nextai_uint16 *)&setthreah.send[ack_hear_lengh], (setting_data_len + 1) >> 1);
                            if (ret == ((setting_data_len + 1) >> 1)) {
                                sendto(setthreah.sock, setthreah.send, strlen((char *)setthreah.send), 0,
                                       (struct sockaddr *)&setthreah.client_addr, setthreah.addr_len);
                            }
                        }
                    }
                }
                else if (strncmp("cmd:rest", (const char *)setthreah.revc, 8) == 0) {
                    NVIC_SystemReset();
                }
                else if (strncmp("cmd:factory settings", (const char *)setthreah.revc, 20) == 0) {
                    rt_uint16_t setting_data_len = 0;
                    rt_uint16_t ret;

                    nextai_setting_default(&setting_para_temp);
                    nextai_setting_save();
                    strcpy((char *)setthreah.send, "Restore factory settings success\r\n");
                    sendto(setthreah.sock, setthreah.send, strlen((char *)setthreah.send), 0,
                           (struct sockaddr *)&setthreah.client_addr, setthreah.addr_len);
                    ret = nextai_flash_read(NEXTAI_SETTING_WRITE_ADDR, (nextai_uint16 *)&setting_data_len, 2);
                    if (ret == 2) {
                        ret = nextai_flash_read(NEXTAI_SETTING_WRITE_ADDR + 4, (nextai_uint16 *)setthreah.send, (setting_data_len + 1) >> 1);
                        if (ret == ((setting_data_len + 1) >> 1)) {
                            strcat((char *)setthreah.send, "\r\n");
                            sendto(setthreah.sock, setthreah.send, strlen((char *)setthreah.send), 0,
                                   (struct sockaddr *)&setthreah.client_addr, setthreah.addr_len);
                        }
                    }
                }
            }
        }
    }
}

#if 0
void nextai_get_local_ip(rt_uint8_t *strIP)
{
    char name[225];

}
#endif


