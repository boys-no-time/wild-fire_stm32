
#ifndef _FW_UPGRADE_H_
#define _FW_UPGRADE_H_

#define FLAG_UPGRADE       0x900dface
#define FLAG_NORMAL        0x00000000

#define RESP_OK       "{\"ret\":\"ok\"}"
#define RESP_FAIL     "{\"ret\":\"fail\"}"
#define RESP_GETCODE  "{\"ret\":\"getcode\"}"


#include "common.h" //in "board/bsp"

struct up_flash_info {
    uint32_t bootflag;
    uint32_t filelen;
    char ver[12];
};

struct upgrade_msg {
    int32_t cmd;
    char server_addr[32];
    char server_port[8];
    char filename[32];
    char ver[12];

};


int32_t inodes_update_init(void);
void inodes_update_reset(void);
struct upgrade_msg   *inodes_update_get_msg(void);
sys_mbox_t *inodes_update_get_recvmbox(void);

#endif

