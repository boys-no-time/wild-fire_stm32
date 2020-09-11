
#include "lwip/netdb.h"
#include "lwip/sockets.h"
#include "inodes_include.h"


#define INODES_REMOTE_GETINFO (0x1)
#define INODES_REMOTE_GETDATA (0x2)
#define INODES_UPDATE_DOWNLOAD_LEN (1024)

#define RECV_BUF_MAX		1536
#define SEND_BUF_SIZE       128
#define DATA_BUF_SIZE       1038
#define UP_TASK_STACK_SIZE  2048

struct inodes_trslate_info {
    uint32_t filelen;
    uint32_t havedownload;
    uint32_t write_flash_p;
    char download_addr[32];
    char download_port[8];

    char server_filename[32];
    char fw_ver[12];

    struct hostent *host;
    struct sockaddr_in server_addr;
    rt_int32_t sock;
} ;

static uint8_t recv_buf[RECV_BUF_MAX];
static uint8_t send_buf[SEND_BUF_SIZE];
static uint8_t fw_data_buf[DATA_BUF_SIZE];
static struct inodes_trslate_info my_trslate_info;
static struct up_flash_info my_up_info;
static struct upgrade_msg my_up_msg;
static rt_thread_t up_tid;

static sys_mbox_t recvmbox;  //It supposes to use an OSAL; however,nobody did this before, I
// borrow from LWIP right now. Should implement it later.......
static int8_t recvValid = 0;


static int32_t inodes_update_data_packet(uint32_t cmd, uint32_t addr, uint32_t len , uint8_t *filename, uint8_t *dntbuf, uint32_t *dntlen)
{
    uint8_t *cur_p = dntbuf;
    uint8_t tmp_buf[16] = {0};

    if (INODES_REMOTE_GETINFO == cmd) {
        rt_sprintf((char *)&tmp_buf[0], "getinfo");
    } else if (INODES_REMOTE_GETDATA == cmd) {
        rt_sprintf((char *)&tmp_buf[0], "getdata");
    } else
        return -1;

    rt_sprintf((char *)cur_p, "{\"cur_cmd\":\"%s\",\"addr\":%d,\"length\":%d,\"filename\":\"%s\"}", tmp_buf, addr, len, filename);

    *dntlen = strlen((char *)dntbuf);

    return 0;
}


static int32_t inodes_update_data_unpacket(uint8_t *srcdata, uint32_t srclen, uint32_t *respcmd, uint32_t *addr, uint8_t *dntbuf, uint32_t *dntlen)
{
    UNUSED(srclen);

    uint8_t *cur_p = srcdata, *crc_p = srcdata, *write_p = dntbuf;
    uint32_t dnt_crc = 0, src_crc = 0;
    int i = 0;
    *respcmd = 0;

    if (*cur_p++ != 0x88)return -1;
    *respcmd = *cur_p++;

    if (*respcmd == INODES_REMOTE_GETINFO) {
        *addr = 0;
        cur_p += 4;
        *dntlen = *cur_p | (*(cur_p + 1) << 8) | (*(cur_p + 2) << 16) | (*(cur_p + 3) << 24);
        cur_p += 4;

        src_crc = *cur_p | (*(cur_p + 1) << 8);

        crc_p ++;
        while (crc_p < cur_p)  dnt_crc = (dnt_crc + *crc_p++) & 0xffff;
        if (dnt_crc != src_crc)return -1;
        cur_p += 2;
        if (0x66 != *cur_p)return -1;
    } else if (*respcmd == INODES_REMOTE_GETDATA) {
        if (0 == write_p)return -1;

        *addr = *cur_p | (*(cur_p + 1) << 8) | (*(cur_p + 2) << 16) | (*(cur_p + 3) << 24);;
        cur_p += 4;
        *dntlen = *cur_p | (*(cur_p + 1) << 8) | (*(cur_p + 2) << 16) | (*(cur_p + 3) << 24);
        cur_p += 4;

        DPRINTF(("addr =%d dntlen =%d \n", *addr, *dntlen));
        for (i = 0; i < (int)(*dntlen); i++) {
            *write_p++ = *cur_p++;
        }

        src_crc = *cur_p | (*(cur_p + 1) << 8);

        crc_p ++;
        while (crc_p < cur_p)  dnt_crc = (dnt_crc + *crc_p++) & 0xffff;
        if (dnt_crc != src_crc) {
            EPRINTF(("CRC Error: %x %x \n", dnt_crc, src_crc));
            return -1;
        }
        cur_p += 2;
        if (0x66 != *cur_p) {
            EPRINTF(("Tail error.\n"));
            return -1;
        }
    }
    //     else if(*respcmd == 0x81)
    //		 {
    //          return 1;
    //		 }
    //		 else if(*respcmd == 0x82)
    //		 {
    //          return 1;
    //		 }
    else
        return -1;

    return 0;
}

#ifdef __CC_ARM
#pragma diag_suppress 111 //statement is unreachable
#endif

void inodes_update_thread(void *parameter)
{
    UNUSED(parameter);

    struct inodes_trslate_info *me;
    uint16_t socket_port ;
    uint32_t timeout;
    uint32_t send_len;
    uint32_t respcmd;
    uint32_t cur_addr;
    uint32_t cur_download_len ;
    uint32_t dntlen;
    int8_t do_retry = 0;
    int8_t err = 0;
    int8_t error_count  = 0;
    int32_t recv_len;
    int result;
    struct upgrade_msg   *upMsg;

    me = (struct inodes_trslate_info *) &my_trslate_info;

    while (1) {

        if ((sys_arch_mbox_fetch(&recvmbox, (void **) &upMsg, 0) == SYS_ARCH_TIMEOUT)) {
            rt_thread_delay(100);
            continue;
        }

        memset((void *)me, 0, sizeof(struct inodes_trslate_info));
        memset((void *)&my_up_info, 0, sizeof(struct up_flash_info));
        error_count  = 0;
#if 0
        strcpy(me->download_addr, "120.24.69.121");
        strcpy(me->download_port, "7705");
        strcpy(me->server_filename, "191216160255_0.0.1.bin_0.0.1");
        strcpy(me->fw_ver, "0.0.1");
#else
        if (upMsg->cmd == 0x01) {
            strcpy(me->download_addr, upMsg->server_addr);
            strcpy(me->download_port, upMsg->server_port);
            strcpy(me->server_filename, upMsg->filename);
            strcpy(me->fw_ver, upMsg->ver);
        } else {
            MQTT_Send2Clientmq(RESP_FAIL, strlen(RESP_FAIL) + 1);
            rt_thread_delay(100);
            continue;
        }
#endif

        socket_port = (uint16_t)(atoi(me->download_port));

        DPRINTF(("server: %s port:%d \n", me->download_addr, socket_port));

        if ((me->host = gethostbyname(me->download_addr)) == NULL) {
            EPRINTF(("[UPGRADE] Fail to get host name.\n"));
            MQTT_Send2Clientmq(RESP_FAIL, strlen(RESP_FAIL) + 1);
            rt_thread_delay(100);
            continue;
        }

        if ((me->sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
            EPRINTF(("[UPGRADE] Fail to open socket.\n"));
            MQTT_Send2Clientmq(RESP_FAIL, strlen(RESP_FAIL) + 1);
            rt_thread_delay(100);
            continue;
        }

        me->server_addr.sin_len = sizeof(me->server_addr);
        me->server_addr.sin_family = AF_INET;
        me->server_addr.sin_port = PP_HTONS(socket_port);
        me->server_addr.sin_addr = *((struct in_addr *) me->host->h_addr);
        timeout = 5000;

        lwip_setsockopt(me->sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

        if (lwip_connect(me->sock, (struct sockaddr *)(&(me->server_addr)), sizeof(struct sockaddr)) == -1) {
            EPRINTF(("[UPGRADE] Faile to connect server.\n"));
            lwip_close(me->sock);
            MQTT_Send2Clientmq(RESP_FAIL, strlen(RESP_FAIL) + 1);
            rt_thread_delay(100);
            continue;
        }
        if ((error_count == 0) && (me->filelen == 0)) {
            do {
                inodes_update_data_packet(INODES_REMOTE_GETINFO, 0, 0, (uint8_t *)me->server_filename, send_buf, &send_len);
                DPRINTF(("Get file info %s \n", send_buf));
                result = lwip_send(me->sock, send_buf, send_len, MSG_WAITALL);
                if (result != -1) {
                    memset((void *)recv_buf, 0, RECV_BUF_MAX);
                    recv_len = lwip_recv(me->sock, recv_buf, RECV_BUF_MAX, MSG_WAITALL);
                    if (recv_len > 0) {
                        DPRINTF(("recv_len =%d \n", recv_len));
                        result = inodes_update_data_unpacket(recv_buf, recv_len, &respcmd, &cur_addr, 0, &dntlen);
                        if ((result == 0) && (respcmd == INODES_REMOTE_GETINFO)) {
                            error_count = 0;
                            me->filelen = dntlen;
                            DPRINTF(("[UPGRADE] get file info ok.  File len =%d \n", dntlen));
                            break;
                        } else {
                            EPRINTF(("[UPGRADE] Get info error.\n"));
                            error_count ++;
                        }
                    } else {
                        EPRINTF(("[UPGRADE] recv data error.\n"));
                        error_count ++;
                    }
                } else {
                    error_count ++;
                    EPRINTF(("[UPGRADE] Send get info cmd fail.\n"));
                }
            } while (error_count < 5);
        }

        if (error_count == 0) {
            if (me->filelen > FLASH_APP_MAXSIZE) {
                EPRINTF(("[UPGRADE] File too large.\n"));
                MQTT_Send2Clientmq(RESP_FAIL, strlen(RESP_FAIL) + 1);
                rt_thread_delay(100);
                goto exit;
            }
            me->write_flash_p = FLASH_BACKUP_START;

            do {
                if ((me->filelen - me->havedownload) >= INODES_UPDATE_DOWNLOAD_LEN) {
                    cur_download_len = INODES_UPDATE_DOWNLOAD_LEN;
                } else {
                    cur_download_len = me->filelen - me->havedownload;
                }

                inodes_update_data_packet(INODES_REMOTE_GETDATA, me->havedownload, cur_download_len, (uint8_t *)me->server_filename, send_buf, &send_len);
                DPRINTF(("[UPGRADE] Get Data: %s \n", send_buf));
                result = lwip_send(me->sock, send_buf, send_len, MSG_WAITALL);
                if (result < 0) {
                    EPRINTF(("[UPGRADE] send data fail.\n"));
                    error_count ++;
                    continue;
                }
                rt_thread_delay(100);
                do_retry = 0;
                err = 0;
                do {
                    memset((void *)recv_buf, 0, RECV_BUF_MAX);
                    recv_len = lwip_recv(me->sock, recv_buf, RECV_BUF_MAX, MSG_WAITALL);
                    if (recv_len > 0) {
                        result = inodes_update_data_unpacket(recv_buf, recv_len, &respcmd, &cur_addr, fw_data_buf, &dntlen);
                        if ((result == 0) && (respcmd == INODES_REMOTE_GETDATA) && (me->havedownload == cur_addr)) {
                            error_count = 0;
                            err = 0;
                            me->havedownload += dntlen;
                            nextai_flash_write(me->write_flash_p, (uint16_t *)fw_data_buf, (dntlen + 1) / 2);
                            me->write_flash_p += ((dntlen + 1) / 2) * 2;

                            DPRINTF(("[UPGRADE] download %d len: %d  (%d/%d) \n", cur_addr, dntlen, me->havedownload, me->filelen));
                            break;
                        } else {
                            EPRINTF(("[UPGRADE] Parse data failed.\n"));
                            err = 1;
                            break;
                        }
                    } else {
                        EPRINTF(("[UPGRADE] Failed to send data do_retry =%d .\n", do_retry));
                        err = 1;
                        do_retry ++;
                        rt_thread_delay(500);
                    }
                } while (do_retry < 3);

                if (err) {
                    error_count ++;
                }

                if (me->havedownload >= me->filelen) {
                    MQTT_Send2Clientmq(RESP_GETCODE, strlen(RESP_GETCODE) + 1);
                    DPRINTF(("[UPGRADE] download finish.\n"));
                    my_up_info.bootflag = FLAG_UPGRADE ;
                    my_up_info.filelen = (uint32_t) me->filelen;
                    strcpy(my_up_info.ver, me->fw_ver);
                    nextai_flash_write(FLASH_BOOT_FLAG_ADDR, (uint16_t *) & my_up_info, (sizeof(my_up_info) + 1) / 2);

                    break;
                }

            } while (error_count < 10);
        }

exit:
        lwip_close(me->sock);

        if (my_up_info.bootflag == FLAG_UPGRADE) {
            DPRINTF(("[UPGRADE] Waiting to reboot.\n"));
            rt_thread_delay(1000);
            NVIC_SystemReset();
            //never reach here...
        }

        rt_thread_delay(100);

    }

    return ;
}

int32_t inodes_update_init(void)
{
    if (sys_mbox_new(&recvmbox, 1) != ERR_OK) {
        EPRINTF(("[UP] create mbox fail.\n"));
    } else {
        recvValid  = 1;
    }

    up_tid = rt_thread_create("upgrade", inodes_update_thread, RT_NULL, UP_TASK_STACK_SIZE,
                              RT_APPS_PRIORITY - 1 , 20);

    if (up_tid != NULL) rt_thread_startup(up_tid);
    DPRINTF(("Upgrade init done.\n"));
    return 0;
}

void inodes_update_reset(void)
{
    struct up_flash_info *flash_info;

    flash_info = (struct up_flash_info *)FLASH_BOOT_FLAG_ADDR;

    if (flash_info->bootflag != 0) {
        memcpy((void *) &my_up_info, (void *) flash_info, sizeof(struct up_flash_info));
        my_up_info.bootflag = FLAG_NORMAL;
        nextai_flash_write(FLASH_BOOT_FLAG_ADDR, (uint16_t *) & my_up_info, (sizeof(my_up_info) + 1) / 2);
    }

}

struct upgrade_msg   *inodes_update_get_msg(void)
{
    return & my_up_msg;
}

sys_mbox_t *inodes_update_get_recvmbox(void)
{
    if (recvValid)
        return  &recvmbox;
    else
        return NULL;
}





