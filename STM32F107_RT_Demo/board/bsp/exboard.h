#ifndef _EXBOARD__H_
#define _EXBOARD__H_
#include <rtthread.h>

#define EXBOARD_REVC_MAX 256

#define EXBOARD_SEND_MAX 256

#define CLIENT_MAX_DATABUF_LENGTH 64

#define BOARD_MQ_MAX 64

typedef struct _EXBOARDTYPEDEF_ {
    rt_thread_t tid;
    /*消息队列*/
    rt_mq_t mq;
    rt_uint8_t mqbuf[BOARD_MQ_MAX];						//收发消息的缓冲区

    rt_device_t RS485;
    rt_uint8_t bytes_revc;
    rt_uint8_t revc[EXBOARD_REVC_MAX];				//接收到的原始数据
    rt_uint8_t bytes_send;
    rt_uint8_t send[EXBOARD_SEND_MAX];				//待发数据，暂时未使用

    rt_int32_t cur_out_event;
    rt_uint8_t cur_out_clientnum;
    rt_uint8_t databuf[CLIENT_MAX_DATABUF_LENGTH];   //读取的数据
    rt_uint8_t datalen;
} EXBOARDTYPEDEF;

enum {
    INODES_CMD_GETSTATUS = 0x1,
    INODES_CMD_OPENKEY = 0x2,
    INODES_CMD_CLOSEKEY = 0x3,
    INODES_CMD_OPENBLUELED = 0x4,
    INODES_CMD_OPENREDLED = 0x5,
    INODES_CMD_CLOSELED = 0x6,
};

//错误状态下返回
enum {
    INODES_RESP_GETSTATUS = 0x81,
    INODES_RESP_OPENKEY = 0x82,
    INODES_RESP_CLOSEKEY = 0x83,
    INODES_RESP_OPENBLUELED = 0x84,
    INODES_RESP_OPENREDLED = 0x85,
    INODES_RESP_CLOSELED = 0x86,
};


enum {
    INODES_KEYBOX_STATUS_NOKEY = 1,
    INODES_KEYBOX_STATUS_KEYNOID = 2,
    INODES_KEYBOX_STATUS_KEYID = 3,
};

typedef struct _INODES_KEY_INFO_ {
    unsigned char respret;
    unsigned char respcmd;
    unsigned char boxaddr;
    unsigned char chanaddr;
    unsigned char keyid[5];
} INODES_KEY_INFO;



extern EXBOARDTYPEDEF exboard;
extern rt_uint8_t inodes_virtual_write(rt_uint8_t clientid, rt_uint8_t status, rt_uint8_t *keyid);
extern rt_uint8_t inodes_virtual_read(rt_uint8_t clientid, rt_uint8_t *status, rt_uint8_t *keyid);
extern rt_int8_t inodes_virtual_cmp(void);
extern rt_uint8_t nextai_client_out_proc(rt_uint8_t clientnum, rt_int32_t cur_cmd);
extern void exboard_thread_create(void);
#endif

